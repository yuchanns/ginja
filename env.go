package ginja

import (
	"context"
	"crypto/md5"
	"encoding/json"
	"fmt"
	"sync"
	"unsafe"

	jffi "github.com/jupiterrider/ffi"
	"go.yuchanns.xyz/ginja/internal/embed"
	"go.yuchanns.xyz/ginja/internal/ffi"
)

type templateCacheEntry struct {
	source string
	hash   string
}

type Environment struct {
	ctx    context.Context
	cancel context.CancelFunc

	inner *mjEnv

	// Template cache to avoid recompiling same templates
	templateCache map[string]*templateCacheEntry
	cacheMutex    sync.RWMutex
}

func New() (env *Environment, err error) {
	path, err := embed.LoadOnce()
	if err != nil {
		return
	}
	ctx, cancel, err := ffi.NewContext(path)
	if err != nil {
		return
	}
	inner := mjEnvNew.Symbol(ctx)()
	env = &Environment{
		ctx:           ctx,
		cancel:        cancel,
		inner:         inner,
		templateCache: make(map[string]*templateCacheEntry),
	}
	return
}

func (env *Environment) Close() {
	if env.inner != nil {
		mjEnvFree.Symbol(env.ctx)(env.inner)
		env.inner = nil
	}
	if env.cancel != nil {
		env.cancel()
		env.cancel = nil
	}
}

func (env *Environment) AddTemplate(name string, source string) (err error) {
	// Check if template is already cached with the same content
	hash := fmt.Sprintf("%x", md5.Sum([]byte(source)))

	env.cacheMutex.RLock()
	if cached, exists := env.templateCache[name]; exists {
		if cached.hash == hash {
			// Template unchanged, no need to recompile
			env.cacheMutex.RUnlock()
			return nil
		}
	}
	env.cacheMutex.RUnlock()

	// Template is new or changed, compile it
	err = mjEnvAddTemplate.Symbol(env.ctx)(env.inner, name, source)
	if err != nil {
		return err
	}

	// Cache the template
	env.cacheMutex.Lock()
	env.templateCache[name] = &templateCacheEntry{
		source: source,
		hash:   hash,
	}
	env.cacheMutex.Unlock()

	return nil
}

func (env *Environment) RenderTemplate(name string, ctx map[string]any) (rendered string, err error) {
	value, err := json.Marshal(ctx)
	if err != nil {
		return
	}
	return mjEnvRender.Symbol(env.ctx)(env.inner, name, value)
}

var mjEnvNew = ffi.NewFFI(ffi.FFIOpts{
	Sym:   "mj_env_new",
	RType: &jffi.TypePointer,
}, func(ctx context.Context, ffiCall ffi.Call) func() *mjEnv {
	return func() *mjEnv {
		var env = &mjEnv{}
		ffiCall(unsafe.Pointer(&env))
		return env
	}
})

var mjEnvFree = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_env_free",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(*mjEnv) {
	return func(env *mjEnv) {
		ffiCall(nil, unsafe.Pointer(&env))
	}
}, true)

var mjEnvAddTemplate = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_env_add_template",
	RType:  &jffi.TypePointer,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(*mjEnv, string, string) error {
	return func(env *mjEnv, name string, source string) (err error) {
		namePtr, err := ffi.BytePtrFromString(name)
		if err != nil {
			return
		}
		sourcePtr, err := ffi.BytePtrFromString(source)
		if err != nil {
			return
		}

		var result *mjError
		ffiCall(
			unsafe.Pointer(&result), unsafe.Pointer(&env),
			unsafe.Pointer(&namePtr), unsafe.Pointer(&sourcePtr),
		)
		if result != nil {
			return parseError(ctx, result)
		}
		return
	}
})

var mjEnvRender = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_env_render",
	RType:  &jffi.TypePointer,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypeUint64},
}, func(ctx context.Context, ffiCall ffi.Call) func(*mjEnv, string, []byte) (string, error) {
	return func(env *mjEnv, name string, data []byte) (rendered string, err error) {
		if len(data) == 0 {
			return
		}
		np, err := ffi.BytePtrFromString(name)
		if err != nil {
			return
		}
		p := &data[0]
		size := uint(len(data))
		var result = &resultMjEnvRenderTemplate{}
		ffiCall(unsafe.Pointer(&result), unsafe.Pointer(&env), unsafe.Pointer(&np), unsafe.Pointer(&p), unsafe.Pointer(&size))
		defer mjResultEnvRenderTemplateFree.Symbol(ctx)(result)
		if result.error != nil {
			err = toError(result.error)
			return
		}
		rendered = ffi.BytePtrToString(result.result)
		return
	}
})

var mjResultEnvRenderTemplateFree = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_result_env_render_template_free",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(*resultMjEnvRenderTemplate) {
	return func(result *resultMjEnvRenderTemplate) {
		ffiCall(nil, unsafe.Pointer(&result))
	}
})

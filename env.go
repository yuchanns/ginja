package ginja

import (
	"context"
	"crypto/md5"
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
	value := newValue(env.ctx)
	defer value.free(env.ctx)

	// Pre-process and batch similar operations to reduce FFI overhead
	err = value.setBatch(env.ctx, ctx)
	if err != nil {
		return
	}

	return mjEnvRenderTemplate.Symbol(env.ctx)(env.inner, name, value.inner)
}

var mjEnvNew = ffi.NewFFI(ffi.FFIOpts{
	Sym:   "mj_env_new",
	RType: typeResultMjEnvNew,
}, func(ctx context.Context, ffiCall ffi.Call) func() *mjEnv {
	return func() *mjEnv {
		var result resultMjEnvNew
		ffiCall(unsafe.Pointer(&result))
		env := result.env
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
	RType:  typeResultMjEnvAddTemplate,
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
		var result resultMjEnvAddTemplate
		ffiCall(
			unsafe.Pointer(&result), unsafe.Pointer(&env),
			unsafe.Pointer(&namePtr), unsafe.Pointer(&sourcePtr),
		)
		if result.error != nil {
			return parseError(ctx, result.error)
		}
		return
	}
})

var mjEnvRenderTemplate = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_env_render_template",
	RType:  typeResultMjEnvRenderTemplate,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(*mjEnv, string, *mjValue) (string, error) {
	return func(env *mjEnv, name string, value *mjValue) (rendered string, err error) {
		namePtr, err := ffi.BytePtrFromString(name)
		if err != nil {
			return
		}
		var result resultMjEnvRenderTemplate
		ffiCall(
			unsafe.Pointer(&result), unsafe.Pointer(&env),
			unsafe.Pointer(&namePtr), unsafe.Pointer(&value),
		)
		if result.error != nil {
			err = parseError(ctx, result.error)
			return
		}
		defer mjStrFree.Symbol(ctx)(result.result)
		rendered = ffi.BytePtrToString(result.result)
		return
	}
})

var mjStrFree = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_str_free",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(*byte) {
	return func(ptr *byte) {
		ffiCall(nil, unsafe.Pointer(&ptr))
	}
}, true)

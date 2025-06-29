package ginja

import (
	"unsafe"

	"github.com/bytedance/sonic"
	"go.yuchanns.xyz/ginja/internal/embed"
)

type templateCacheEntry struct {
	source string
	hash   string
}

type Environment struct {
	ffi *ffi

	inner unsafe.Pointer
}

func New() (env *Environment, err error) {
	path, err := embed.LoadOnce()
	if err != nil {
		return
	}
	ffi, err := newFFI(path)
	if err != nil {
		return
	}
	inner := ffi.MjEnvNew()
	env = &Environment{
		ffi:   ffi,
		inner: inner,
	}
	return
}

func (env *Environment) Close() {
	if env.inner == nil {
		return
	}
	defer FreeLibrary(env.ffi.lib)
	env.ffi.MjEnvFree(env.inner)
	env.inner = nil
}

func (env *Environment) AddTemplate(name string, source string) (err error) {
	nptr, err := BytePtrFromString(name)
	if err != nil {
		return
	}
	sptr, err := BytePtrFromString(source)
	if err != nil {
		return
	}

	ret := env.ffi.MjEnvAddTemplate(env.inner, nptr, sptr)
	if ret != nil {
		defer env.ffi.MjErrorFree(ret)
		err = parseError(ret)
	}

	return
}

func (env *Environment) RenderTemplate(name string, ctx map[string]any) (rendered string, err error) {
	value, err := sonic.Marshal(ctx)
	if err != nil {
		return
	}
	nptr, err := BytePtrFromString(name)
	if err != nil {
		return
	}
	ret := env.ffi.MjEnvRender(env.inner, nptr, &value[0], uint(len(value)))
	defer env.ffi.MjResultEnvRenderTemplateFree(ret)
	result := (*mjResultEnvRenderTemplate)(unsafe.Pointer(ret))
	if result.error != nil {
		err = parseError(result.error)
		return
	}
	rendered = BytePtrToString(result.rendered)
	return
}

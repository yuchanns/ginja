package ginja

import (
	"context"
	"unsafe"

	jffi "github.com/jupiterrider/ffi"
	"go.yuchanns.xyz/ginja/internal/embed"
	"go.yuchanns.xyz/ginja/internal/ffi"
)

type Environment struct {
	ctx    context.Context
	cancel context.CancelFunc

	inner *mjEnv
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
		ctx:    ctx,
		cancel: cancel,
		inner:  inner,
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
	return mjEnvAddTemplate.Symbol(env.ctx)(env.inner, name, source)
}

func (env *Environment) RenderTemplate(name string, ctx map[string]any) (rendered string, err error) {
	value := mjValueNew.Symbol(env.ctx)()
	defer mjValueFree.Symbol(env.ctx)(value)

	for k, v := range ctx {
		switch v := v.(type) {
		case string:
			err = mjValueSetString.Symbol(env.ctx)(value, k, v)
		case int:
			err = mjValueSetInt.Symbol(env.ctx)(value, k, int64(v))
		case int64:
			err = mjValueSetInt.Symbol(env.ctx)(value, k, v)
		case int32:
			err = mjValueSetInt32.Symbol(env.ctx)(value, k, v)
		case int16:
			err = mjValueSetInt16.Symbol(env.ctx)(value, k, v)
		case int8:
			err = mjValueSetInt8.Symbol(env.ctx)(value, k, v)
		case uint:
			err = mjValueSetUint.Symbol(env.ctx)(value, k, uint64(v))
		case uint64:
			err = mjValueSetUint.Symbol(env.ctx)(value, k, v)
		case uint32:
			err = mjValueSetUint32.Symbol(env.ctx)(value, k, v)
		case uint16:
			err = mjValueSetUint16.Symbol(env.ctx)(value, k, v)
		case uint8:
			err = mjValueSetUint8.Symbol(env.ctx)(value, k, v)
		case float64:
			err = mjValueSetFloat.Symbol(env.ctx)(value, k, v)
		case float32:
			err = mjValueSetFloat32.Symbol(env.ctx)(value, k, v)
		case bool:
			err = mjValueSetBool.Symbol(env.ctx)(value, k, v)
		default:
			// no supported type. skip
		}
		if err != nil {
			return
		}
	}

	return mjEnvRenderTemplate.Symbol(env.ctx)(env.inner, name, value)
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
})

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
		rendered = ffi.BytePtrToString(result.result)
		return
	}
})

package ginja

import (
	"context"
	"unsafe"

	jffi "github.com/jupiterrider/ffi"
	"go.yuchanns.xyz/ginja/internal/ffi"
)

type value struct {
	inner *mjValue

	values []*value
}

func newValue(ctx context.Context) *value {
	return &value{
		inner: mjValueNew.Symbol(ctx)(),
	}
}

func (v *value) free(ctx context.Context) {
	for _, val := range v.values {
		val.free(ctx)
	}
	if v.inner == nil {
		return
	}
	mjValueFree.Symbol(ctx)(v.inner)
	v.inner = nil
}

func (v *value) set(ctx context.Context, key string, val any) (err error) {
	value := v.inner
	switch val := val.(type) {
	case string:
		err = mjValueSetString.Symbol(ctx)(value, key, val)
	case int:
		err = mjValueSetInt.Symbol(ctx)(value, key, int64(val))
	case int64:
		err = mjValueSetInt.Symbol(ctx)(value, key, val)
	case int32:
		err = mjValueSetInt32.Symbol(ctx)(value, key, val)
	case int16:
		err = mjValueSetInt16.Symbol(ctx)(value, key, val)
	case int8:
		err = mjValueSetInt8.Symbol(ctx)(value, key, val)
	case uint:
		err = mjValueSetUint.Symbol(ctx)(value, key, uint64(val))
	case uint64:
		err = mjValueSetUint.Symbol(ctx)(value, key, val)
	case uint32:
		err = mjValueSetUint32.Symbol(ctx)(value, key, val)
	case uint16:
		err = mjValueSetUint16.Symbol(ctx)(value, key, val)
	case uint8:
		err = mjValueSetUint8.Symbol(ctx)(value, key, val)
	case float64:
		err = mjValueSetFloat.Symbol(ctx)(value, key, val)
	case float32:
		err = mjValueSetFloat32.Symbol(ctx)(value, key, val)
	case bool:
		err = mjValueSetBool.Symbol(ctx)(value, key, val)
	default:
		// no supported type. skip
	}
	return
}

type mjValue struct{}

var mjValueNew = ffi.NewFFI(ffi.FFIOpts{
	Sym:   "mj_value_new",
	RType: &jffi.TypePointer,
}, func(ctx context.Context, ffiCall ffi.Call) func() *mjValue {
	return func() *mjValue {
		var value = &mjValue{}
		ffiCall(unsafe.Pointer(&value))
		return value
	}
})

var mjValueFree = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_free",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue) {
	return func(value *mjValue) {
		ffiCall(nil, unsafe.Pointer(&value))
	}
})

var mjValueSetString = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_string",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key, val string) (err error) {
	return func(value *mjValue, key, val string) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		valPtr, err := ffi.BytePtrFromString(val)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr))
		return
	}
})

var mjValueSetInt = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_int",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeSint64},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val int64) (err error) {
	return func(value *mjValue, key string, val int64) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetInt32 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_int32",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeSint32},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val int32) (err error) {
	return func(value *mjValue, key string, val int32) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetInt16 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_int16",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeSint16},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val int16) (err error) {
	return func(value *mjValue, key string, val int16) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetInt8 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_int8",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeSint8},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val int8) (err error) {
	return func(value *mjValue, key string, val int8) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetUint = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_uint",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeUint64},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val uint64) (err error) {
	return func(value *mjValue, key string, val uint64) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetUint32 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_uint32",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeUint32},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val uint32) (err error) {
	return func(value *mjValue, key string, val uint32) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetUint16 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_uint16",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeUint16},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val uint16) (err error) {
	return func(value *mjValue, key string, val uint16) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetUint8 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_uint8",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeUint8},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val uint8) (err error) {
	return func(value *mjValue, key string, val uint8) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetFloat = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_float",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeDouble},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val float64) (err error) {
	return func(value *mjValue, key string, val float64) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetFloat32 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_float32",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeFloat},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val float32) (err error) {
	return func(value *mjValue, key string, val float32) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetBool = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_bool",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypeSint32},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val bool) (err error) {
	return func(value *mjValue, key string, val bool) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		var valueUint8 int32
		if val {
			valueUint8 = 1
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valueUint8))
		return
	}
})

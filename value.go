package ginja

import (
	"context"
	"reflect"
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

func (v *value) newNested(ctx context.Context) *value {
	nested := newValue(ctx)
	v.values = append(v.values, nested)
	return nested
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
	case *string:
		err = mjValueSetString.Symbol(ctx)(value, key, *val)
	case int:
		err = mjValueSetInt.Symbol(ctx)(value, key, int64(val))
	case *int:
		err = mjValueSetInt.Symbol(ctx)(value, key, int64(*val))
	case int64:
		err = mjValueSetInt.Symbol(ctx)(value, key, val)
	case *int64:
		err = mjValueSetInt.Symbol(ctx)(value, key, *val)
	case int32:
		err = mjValueSetInt32.Symbol(ctx)(value, key, val)
	case *int32:
		err = mjValueSetInt32.Symbol(ctx)(value, key, *val)
	case int16:
		err = mjValueSetInt16.Symbol(ctx)(value, key, val)
	case *int16:
		err = mjValueSetInt16.Symbol(ctx)(value, key, *val)
	case int8:
		err = mjValueSetInt8.Symbol(ctx)(value, key, val)
	case *int8:
		err = mjValueSetInt8.Symbol(ctx)(value, key, *val)
	case uint:
		err = mjValueSetUint.Symbol(ctx)(value, key, uint64(val))
	case *uint:
		err = mjValueSetUint.Symbol(ctx)(value, key, uint64(*val))
	case uint64:
		err = mjValueSetUint.Symbol(ctx)(value, key, val)
	case *uint64:
		err = mjValueSetUint.Symbol(ctx)(value, key, *val)
	case uint32:
		err = mjValueSetUint32.Symbol(ctx)(value, key, val)
	case *uint32:
		err = mjValueSetUint32.Symbol(ctx)(value, key, *val)
	case uint16:
		err = mjValueSetUint16.Symbol(ctx)(value, key, val)
	case *uint16:
		err = mjValueSetUint16.Symbol(ctx)(value, key, *val)
	case uint8:
		err = mjValueSetUint8.Symbol(ctx)(value, key, val)
	case *uint8:
		err = mjValueSetUint8.Symbol(ctx)(value, key, *val)
	case float64:
		err = mjValueSetFloat.Symbol(ctx)(value, key, val)
	case *float64:
		err = mjValueSetFloat.Symbol(ctx)(value, key, *val)
	case float32:
		err = mjValueSetFloat32.Symbol(ctx)(value, key, val)
	case *float32:
		err = mjValueSetFloat32.Symbol(ctx)(value, key, *val)
	case bool:
		err = mjValueSetBool.Symbol(ctx)(value, key, val)
	case *bool:
		err = mjValueSetBool.Symbol(ctx)(value, key, *val)
	case []string:
		err = mjValueSetListString.Symbol(ctx)(value, key, val)
	case []int:
		int64Slice := make([]int64, len(val))
		for i, v := range val {
			int64Slice[i] = int64(v)
		}
		err = mjValueSetListInt.Symbol(ctx)(value, key, int64Slice)
	case []int64:
		err = mjValueSetListInt.Symbol(ctx)(value, key, val)
	case []int32:
		err = mjValueSetListInt32.Symbol(ctx)(value, key, val)
	case []int16:
		err = mjValueSetListInt16.Symbol(ctx)(value, key, val)
	case []int8:
		err = mjValueSetListInt8.Symbol(ctx)(value, key, val)
	case []uint:
		uint64Slice := make([]uint64, len(val))
		for i, v := range val {
			uint64Slice[i] = uint64(v)
		}
		err = mjValueSetListUint.Symbol(ctx)(value, key, uint64Slice)
	case []uint64:
		err = mjValueSetListUint.Symbol(ctx)(value, key, val)
	case []uint32:
		err = mjValueSetListUint32.Symbol(ctx)(value, key, val)
	case []uint16:
		err = mjValueSetListUint16.Symbol(ctx)(value, key, val)
	case []uint8:
		err = mjValueSetListUint8.Symbol(ctx)(value, key, val)
	case []float64:
		err = mjValueSetListFloat.Symbol(ctx)(value, key, val)
	case []float32:
		err = mjValueSetListFloat32.Symbol(ctx)(value, key, val)
	case []bool:
		err = mjValueSetListBool.Symbol(ctx)(value, key, val)
	case map[string]string:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]int:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]int64:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]int32:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]int16:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]int8:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]uint:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]uint64:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]uint32:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]uint16:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]uint8:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]float64:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]float32:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	case map[string]bool:
		nested := v.newNested(ctx)
		for k, v := range val {
			nested.set(ctx, k, v)
		}
		err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
	default:
		rv := reflect.ValueOf(val)
		if rv.Kind() == reflect.Ptr {
			rv = rv.Elem()
		}
		switch rv.Kind() {
		case reflect.Map:
			nested := v.newNested(ctx)
			for _, key := range rv.MapKeys() {
				mapValue := rv.MapIndex(key)
				nested.set(ctx, key.String(), mapValue.Interface())
			}
			err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
		case reflect.Struct:
			nested := v.newNested(ctx)
			nested.setStructFields(ctx, rv)
			err = mjValueSetValue.Symbol(ctx)(value, key, nested.inner)
		case reflect.Slice:
			// Handle slice of structs
			values := make([]*mjValue, 0, rv.Len())
			for i := range rv.Len() {
				elem := rv.Index(i)
				if elem.Kind() == reflect.Ptr {
					elem = elem.Elem()
				}
				if elem.Kind() != reflect.Struct {
					continue // skip non-struct elements
				}
				nested := v.newNested(ctx)
				nested.setStructFields(ctx, elem)
				values = append(values, nested.inner)
			}
			err = mjValueSetListValue.Symbol(ctx)(value, key, values)
		default:
			// no supported type. skip
		}
	}
	return
}

func (value *value) setStructFields(ctx context.Context, rv reflect.Value) {
	for i := range rv.NumField() {
		field := rv.Field(i)
		fieldType := rv.Type().Field(i)

		if !field.CanInterface() {
			continue // skip unexported fields
		}

		// Handle embedded/anonymous structs
		if fieldType.Anonymous {
			// Dereference pointer if necessary
			fieldValue := field
			if field.Kind() == reflect.Ptr {
				if field.IsNil() {
					continue // skip nil embedded pointer
				}
				fieldValue = field.Elem()
			}

			// If it's a struct, recursively add its fields to the current level
			if fieldValue.Kind() == reflect.Struct {
				value.setStructFields(ctx, fieldValue)
			}
		} else {
			// Regular named field
			fieldName := fieldType.Name
			value.set(ctx, fieldName, field.Interface())
		}
	}
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
}, true)

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

var mjValueSetValue = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_value",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val *mjValue) (err error) {
	return func(value *mjValue, key string, val *mjValue) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&val))
		return
	}
})

var mjValueSetListString = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_string",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []string) (err error) {
	return func(value *mjValue, key string, val []string) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		listString := make([]*byte, len(val))
		for i, str := range val {
			listString[i], err = ffi.BytePtrFromString(str)
			if err != nil {
				return
			}
		}

		if len(listString) == 0 {
			var value byte
			listString = []*byte{&value}
		}

		valPtr := unsafe.Pointer(&listString[0])
		length := len(listString)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListInt = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_int",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []int64) (err error) {
	return func(value *mjValue, key string, val []int64) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []int64{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListInt32 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_int32",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []int32) (err error) {
	return func(value *mjValue, key string, val []int32) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []int32{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListInt16 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_int16",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []int16) (err error) {
	return func(value *mjValue, key string, val []int16) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []int16{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListInt8 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_int8",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []int8) (err error) {
	return func(value *mjValue, key string, val []int8) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []int8{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListUint = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_uint",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []uint64) (err error) {
	return func(value *mjValue, key string, val []uint64) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []uint64{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListUint32 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_uint32",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []uint32) (err error) {
	return func(value *mjValue, key string, val []uint32) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []uint32{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListUint16 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_uint16",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []uint16) (err error) {
	return func(value *mjValue, key string, val []uint16) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []uint16{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListUint8 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_uint8",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []uint8) (err error) {
	return func(value *mjValue, key string, val []uint8) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []uint8{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListFloat = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_float",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []float64) (err error) {
	return func(value *mjValue, key string, val []float64) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []float64{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListFloat32 = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_float32",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []float32) (err error) {
	return func(value *mjValue, key string, val []float32) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []float32{0}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListBool = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_bool",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []bool) (err error) {
	return func(value *mjValue, key string, val []bool) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		if len(val) == 0 {
			val = []bool{false}
		}

		valPtr := unsafe.Pointer(&val[0])
		length := len(val)
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

var mjValueSetListValue = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_value_set_list_value",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer, &jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(value *mjValue, key string, val []*mjValue) (err error) {
	return func(value *mjValue, key string, val []*mjValue) (err error) {
		keyPtr, err := ffi.BytePtrFromString(key)
		if err != nil {
			return
		}

		length := len(val)
		if len(val) == 0 {
			// Create a dummy value for empty arrays
			dummyValue := mjValueNew.Symbol(ctx)()
			defer mjValueFree.Symbol(ctx)(dummyValue)
			val = []*mjValue{dummyValue}
		}

		valPtr := unsafe.Pointer(&val[0])
		ffiCall(nil, unsafe.Pointer(&value), unsafe.Pointer(&keyPtr), unsafe.Pointer(&valPtr), unsafe.Pointer(&length))
		return
	}
})

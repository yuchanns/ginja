package ginja

import (
	"reflect"
	"unsafe"

	"github.com/ebitengine/purego"
	"github.com/iancoleman/strcase"
)

type ffi struct {
	MjEnvNew  func() unsafe.Pointer
	MjEnvFree func(env unsafe.Pointer)

	MjEnvAddTemplate              func(env unsafe.Pointer, name *byte, source *byte) unsafe.Pointer
	MjEnvRender                   func(env unsafe.Pointer, name *byte, data *byte, dataLen uint) unsafe.Pointer
	MjResultEnvRenderTemplateFree func(result unsafe.Pointer)

	MjErrorFree func(err unsafe.Pointer)

	lib uintptr
}

func newFFI(path string) (FFI *ffi, err error) {
	lib, err := LoadLibrary(path)
	if err != nil {
		return nil, err
	}

	FFI = &ffi{
		lib: lib,
	}

	v := reflect.ValueOf(FFI).Elem()
	t := reflect.TypeOf(FFI).Elem()
	for i := range t.NumField() {
		field := t.Field(i)
		if field.Type.Kind() != reflect.Func {
			continue
		}
		fname := strcase.ToSnake(field.Name)
		addr := v.Field(i).Addr().Interface()

		purego.RegisterLibFunc(addr, lib, fname)
	}

	return
}

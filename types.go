package ginja

import "github.com/jupiterrider/ffi"

type mjEnv struct{}

type resultMjEnvNew struct {
	env *mjEnv
}

var typeResultMjEnvNew = &ffi.Type{
	Type: ffi.Struct,
	Elements: &[]*ffi.Type{
		&ffi.TypePointer,
		nil,
	}[0],
}

type mjError struct {
	code    int32
	message *byte
}

var typeMjError = &ffi.Type{
	Type: ffi.Struct,
	Elements: &[]*ffi.Type{
		&ffi.TypeUint16,
		nil,
	}[0],
}

type resultMjEnvAddTemplate struct {
	error *mjError
}

var typeResultMjEnvAddTemplate = &ffi.Type{
	Type: ffi.Struct,
	Elements: &[]*ffi.Type{
		&ffi.TypePointer,
		nil,
	}[0],
}

type resultMjEnvRenderTemplate struct {
	result *byte
	error  *mjError
}

var typeResultMjEnvRenderTemplate = &ffi.Type{
	Type: ffi.Struct,
	Elements: &[]*ffi.Type{
		&ffi.TypePointer,
		&ffi.TypePointer,
		nil,
	}[0],
}

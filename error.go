package ginja

import (
	"context"
	"fmt"
	"unsafe"

	jffi "github.com/jupiterrider/ffi"
	"go.yuchanns.xyz/ginja/internal/ffi"
)

type ErrorCode int32

const (
	CodeNonPrimitive ErrorCode = iota
	CodeNonKey
	CodeInvalidOperation
	CodeSyntaxError
	CodeTemplateNotFound
	CodeTooManyArguments
	CodeMissingArgument
	CodeUnknownFilter
	CodeUnknownTest
	CodeUnknownFunction
	CodeUnknownMethod
	CodeUnknowBadEscape
	CodeUndefinedError
	CodeBadSerialization
	CodeCannotDeserialize
	CodeBadInclude
	CodeEvalBlock
	CodeCannotUnpack
	CodeWriteFailure
	CodeUnknownBlock
)

type Error struct {
	code    ErrorCode
	message string
}

func (e *Error) Error() string {
	return fmt.Sprintf("%d %s", e.code, e.message)
}

func (e *Error) Code() ErrorCode {
	return e.code
}

func (e *Error) Message() string {
	return e.message
}

func parseError(ctx context.Context, err *mjError) error {
	if err == nil {
		return nil
	}
	return &Error{
		code:    ErrorCode(err.code),
		message: ffi.BytePtrToString(err.message),
	}
}

var mjErrorFree = ffi.NewFFI(ffi.FFIOpts{
	Sym:    "mj_error_free",
	RType:  &jffi.TypeVoid,
	ATypes: []*jffi.Type{&jffi.TypePointer},
}, func(ctx context.Context, ffiCall ffi.Call) func(*mjError) {
	return func(e *mjError) {
		ffiCall(nil, unsafe.Pointer(&e))
	}
}, true)

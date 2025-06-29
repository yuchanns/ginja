package ginja

import (
	"fmt"
	"unsafe"
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

func parseError(e unsafe.Pointer) (err error) {
	if e == nil {
		return
	}
	ret := (*mjError)(e)
	return &Error{
		code:    ErrorCode(ret.code),
		message: BytePtrToString(ret.message),
	}
}

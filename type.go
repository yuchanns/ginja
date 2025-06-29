package ginja

import "unsafe"

type mjError struct {
	code    int32
	message *byte
}

type mjResultEnvRenderTemplate struct {
	rendered *byte
	error    unsafe.Pointer
}

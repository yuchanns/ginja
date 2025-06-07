package ffi

import (
	"context"
	"errors"
	"unsafe"

	"github.com/jupiterrider/ffi"
)

type FFIOpts struct {
	Sym    contextKey
	RType  *ffi.Type
	ATypes []*ffi.Type
}

type Call func(rValue unsafe.Pointer, aValues ...unsafe.Pointer)

type contextKey string

func (c contextKey) String() string {
	return string(c)
}

type ContextWithFFI func(ctx context.Context, lib uintptr) (context.Context, error)

type FFI[T any] struct {
	opts     FFIOpts
	withFunc func(ctx context.Context, ffiCall Call) T
}

func NewFFI[T any](opts FFIOpts, withFunc func(ctx context.Context, ffiCall Call) T) *FFI[T] {
	return &FFI[T]{
		opts:     opts,
		withFunc: withFunc,
	}
}

func (f *FFI[T]) Symbol(ctx context.Context) T {
	return ctx.Value(f.opts.Sym).(T)
}

func (f *FFI[T]) WithFFI(ctx context.Context, lib uintptr) (context.Context, error) {
	var cif ffi.Cif
	if status := ffi.PrepCif(
		&cif,
		ffi.DefaultAbi,
		uint32(len(f.opts.ATypes)),
		f.opts.RType,
		f.opts.ATypes...,
	); status != ffi.OK {
		return nil, errors.New(status.String())
	}
	fn, err := GetProcAddress(lib, f.opts.Sym.String())
	if err != nil {
		return nil, err
	}
	val := f.withFunc(ctx, func(rValue unsafe.Pointer, aValues ...unsafe.Pointer) {
		ffi.Call(&cif, fn, rValue, aValues...)
	})
	return context.WithValue(ctx, f.opts.Sym, val), nil
}

package ginja

import (
	"context"

	"go.yuchanns.xyz/ginja/internal/embed"
	"go.yuchanns.xyz/ginja/internal/ffi"
)

func contextWithFFIs() (ctx context.Context, cancel context.CancelFunc, err error) {
	path, err := embed.LoadOnce()
	if err != nil {
		return
	}
	lib, err := ffi.LoadLibrary(path)
	if err != nil {
		return
	}
	ctx = context.Background()
	for _, withFFI := range withFFIs {
		ctx, err = withFFI(ctx, lib)
		if err != nil {
			return
		}
	}
	cancel = func() {
		_ = ffi.FreeLibrary(lib)
	}

	return
}

var withFFIs = []ffi.ContextWithFFI{
	mjEnvFree.WithFFI,
	mjErrorFree.WithFFI,

	mjEnvNew.WithFFI,
	mjEnvAddTemplate.WithFFI,
	mjEnvRenderTemplate.WithFFI,

	mjValueFree.WithFFI,
	mjValueNew.WithFFI,
	mjValueSetString.WithFFI,
	mjValueSetInt.WithFFI,
	mjValueSetInt32.WithFFI,
	mjValueSetInt16.WithFFI,
	mjValueSetInt8.WithFFI,
	mjValueSetUint.WithFFI,
	mjValueSetUint32.WithFFI,
	mjValueSetUint16.WithFFI,
	mjValueSetUint8.WithFFI,
	mjValueSetFloat.WithFFI,
	mjValueSetFloat32.WithFFI,
}

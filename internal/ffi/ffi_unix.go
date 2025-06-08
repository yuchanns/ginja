//go:build !windows

package ffi

import (
	"github.com/ebitengine/purego"
	"golang.org/x/sys/unix"
)

func BytePtrFromString(s string) (*byte, error) {
	if s == "" {
		return new(byte), nil
	}
	return unix.BytePtrFromString(s)
}

func BytePtrToString(p *byte) string {
	if p == nil {
		return ""
	}
	return unix.BytePtrToString(p)
}

func LoadLibrary(path string) (uintptr, error) {
	return purego.Dlopen(path, purego.RTLD_LAZY|purego.RTLD_GLOBAL)
}

func FreeLibrary(handle uintptr) error {
	if handle == 0 {
		return nil
	}
	err := purego.Dlclose(handle)
	if err != nil {
		return err
	}
	return nil
}

func GetProcAddress(handle uintptr, name string) (uintptr, error) {
	if handle == 0 {
		return 0, nil
	}
	addr, err := purego.Dlsym(handle, name)
	if err != nil {
		return 0, err
	}
	return addr, nil
}

package embed

import (
	"bytes"
	"fmt"
	"io"
	"os"
	"sync"

	"github.com/klauspost/compress/zstd"
)

var once = &sync.Once{}

func LoadOnce() (path string, err error) {
	once.Do(func() {
		var data []byte
		data, err = decompressLib(libminijinjaZst)
		if err != nil {
			return
		}
		path, err = writeTempExec(pattern, data)
	})
	return
}

func writeTempExec(pattern string, binary []byte) (path string, err error) {
	f, err := os.CreateTemp("", pattern)
	if err != nil {
		err = fmt.Errorf("cannot create temp file: %s", err)
		return
	}
	defer f.Close()
	_, err = f.Write(binary)
	if err != nil {
		err = fmt.Errorf("cannot write binary: %s", err)
		return
	}
	err = f.Chmod(os.ModePerm)
	if err != nil {
		err = fmt.Errorf("cannot chmod: %s", err)
		return
	}
	path = f.Name()
	return
}

func decompressLib(raw []byte) (data []byte, err error) {
	decoder, err := zstd.NewReader(bytes.NewReader(raw))
	if err != nil {
		err = fmt.Errorf("cannot create zstd reader: %s", err)
		return
	}
	defer decoder.Close()
	data, err = io.ReadAll(decoder)
	if err != nil {
		err = fmt.Errorf("cannot reading decompressed data: %s", err)
	}
	return
}

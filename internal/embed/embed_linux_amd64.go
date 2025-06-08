package embed

import _ "embed"

//go:embed libminijinja_c.linux.amd64.so.zst
var libminijinjaZst []byte

var pattern = "libminijinja_c*.so"

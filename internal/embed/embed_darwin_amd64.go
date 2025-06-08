package embed

import _ "embed"

//go:embed libminijinja_c.darwin.amd64.dylib.zst
var libminijinjaZst []byte

var pattern = "libminijinja_c*.dylib"

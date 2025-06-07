package embed

import _ "embed"

//go:embed libminijinja_c.darwin.arm64.dylib.zst
var libminijinjaZst []byte

var pattern = "libopendal_c*.dylib"

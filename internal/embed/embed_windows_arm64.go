package embed

import _ "embed"

//go:embed minijinja_c.windows.arm64.dll.zst
var libminijinjaZst []byte

var pattern = "minijinja_c*.dll"

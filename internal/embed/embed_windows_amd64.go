package embed

import _ "embed"

//go:embed libminijinja_c.windows.amd64.dll.zst
var libminijinjaZst []byte

var pattern = "minijinja_c*.dll"

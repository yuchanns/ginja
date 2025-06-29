package ginja_test

import (
	"reflect"
	"strings"
	"testing"
	"text/template"
)

type BenchmarkData struct {
	Name   string
	Age    int
	Active bool
	Score  float64
}

type ComplexData struct {
	Title string
	Users []BenchmarkData
	Meta  map[string]any
}

func BenchmarkSuite(b *testing.B) {
	suite := &Suite{}

	err := suite.Setup()
	if err != nil {
		b.Fatal(err)
	}

	b.Cleanup(suite.TearDown)

	tt := reflect.TypeOf(suite)
	for i := range tt.NumMethod() {
		method := tt.Method(i)
		benchFunc, ok := method.Func.Interface().(func(*Suite, *testing.B))
		if !ok {
			continue
		}
		b.Run(strings.TrimPrefix(method.Name, "Bench"), func(b *testing.B) {
			benchFunc(suite, b)
		})
	}
}

// BenchSimpleVariables - Simple variable rendering benchmark
func (s *Suite) BenchSimpleVariables(b *testing.B) {
	ginjaTemplate := "Hello {{ name }}! You are {{ age }} years old."
	goTemplate := "Hello {{ .name }}! You are {{ .age }} years old."

	data := map[string]any{
		"name": "John",
		"age":  30,
	}

	b.Run("ginja", func(b *testing.B) {
		err := s.env.AddTemplate("simple", ginjaTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			result, err := s.env.RenderTemplate("simple", data)
			if err != nil {
				b.Fatal(err)
			}
			_ = result
		}
	})

	b.Run("std", func(b *testing.B) {
		tmpl, err := template.New("simple").Parse(goTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			var buf strings.Builder
			err := tmpl.Execute(&buf, data)
			if err != nil {
				b.Fatal(err)
			}
			_ = buf.String()
		}
	})
}

// BenchLoopSmall - Loop rendering benchmark with small dataset
func (s *Suite) BenchLoopSmall(b *testing.B) {
	ginjaTemplate := "Users: {% for user in users %}{{ user.name }}({{ user.age }}) {% endfor %}"
	goTemplate := "Users: {{ range .users }}{{ .Name }}({{ .Age }}) {{ end }}"

	users := make([]BenchmarkData, 10)
	for i := range 10 {
		users[i] = BenchmarkData{
			Name:   "User" + string(rune('A'+i)),
			Age:    20 + i,
			Active: i%2 == 0,
			Score:  float64(i) * 10.5,
		}
	}

	data := map[string]any{
		"users": users,
	}

	b.Run("ginja", func(b *testing.B) {
		err := s.env.AddTemplate("loop", ginjaTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			result, err := s.env.RenderTemplate("loop", data)
			if err != nil {
				b.Fatal(err)
			}
			_ = result
		}
	})

	b.Run("std", func(b *testing.B) {
		tmpl, err := template.New("loop").Parse(goTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			var buf strings.Builder
			err := tmpl.Execute(&buf, data)
			if err != nil {
				b.Fatal(err)
			}
			_ = buf.String()
		}
	})
}

// BenchLoopLarge - Loop rendering benchmark with large dataset
func (s *Suite) BenchLoopLarge(b *testing.B) {
	ginjaTemplate := "Users: {% for user in users %}{{ user.name }}({{ user.age }}) {% endfor %}"
	goTemplate := "Users: {{ range .users }}{{ .Name }}({{ .Age }}) {{ end }}"

	users := make([]BenchmarkData, 1000)
	for i := range 1000 {
		users[i] = BenchmarkData{
			Name:   "User" + string(rune('A'+(i%26))),
			Age:    20 + (i % 60),
			Active: i%2 == 0,
			Score:  float64(i) * 10.5,
		}
	}

	data := map[string]any{
		"users": users,
	}

	b.Run("ginja", func(b *testing.B) {
		err := s.env.AddTemplate("loop_large", ginjaTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			result, err := s.env.RenderTemplate("loop_large", data)
			if err != nil {
				b.Fatal(err)
			}
			_ = result
		}
	})

	b.Run("std", func(b *testing.B) {
		tmpl, err := template.New("loop").Parse(goTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			var buf strings.Builder
			err := tmpl.Execute(&buf, data)
			if err != nil {
				b.Fatal(err)
			}
			_ = buf.String()
		}
	})
}

// BenchComplexNested - Complex nested data rendering benchmark
func (s *Suite) BenchComplexNested(b *testing.B) {
	ginjaTemplate := `
<h1>{{ title }}</h1>
<div class="users">
{% for user in users %}
  <div class="user">
    <h2>{{ user.name }}</h2>
    <p>Age: {{ user.age }}, Score: {{ user.score }}</p>
    <p>Status: {% if user.active %}Active{% else %}Inactive{% endif %}</p>
  </div>
{% endfor %}
</div>
<div class="meta">
  <p>Version: {{ meta.version }}</p>
  <p>Generated: {{ meta.timestamp }}</p>
</div>`

	goTemplate := `
<h1>{{ .Title }}</h1>
<div class="users">
{{ range .Users }}
  <div class="user">
    <h2>{{ .Name }}</h2>
    <p>Age: {{ .Age }}, Score: {{ .Score }}</p>
    <p>Status: {{ if .Active }}Active{{ else }}Inactive{{ end }}</p>
  </div>
{{ end }}
</div>
<div class="meta">
  <p>Version: {{ .Meta.version }}</p>
  <p>Generated: {{ .Meta.timestamp }}</p>
</div>`

	users := make([]BenchmarkData, 50)
	for i := range 50 {
		users[i] = BenchmarkData{
			Name:   "User" + string(rune('A'+(i%26))),
			Age:    20 + (i % 60),
			Active: i%2 == 0,
			Score:  float64(i) * 10.5,
		}
	}

	ginjaData := map[string]any{
		"title": "User Dashboard",
		"users": users,
		"meta": map[string]any{
			"version":   "1.0.0",
			"timestamp": "2024-01-01T00:00:00Z",
		},
	}

	goData := ComplexData{
		Title: "User Dashboard",
		Users: users,
		Meta: map[string]any{
			"version":   "1.0.0",
			"timestamp": "2024-01-01T00:00:00Z",
		},
	}

	b.Run("ginja", func(b *testing.B) {
		err := s.env.AddTemplate("complex", ginjaTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			result, err := s.env.RenderTemplate("complex", ginjaData)
			if err != nil {
				b.Fatal(err)
			}
			_ = result
		}
	})

	b.Run("std", func(b *testing.B) {
		tmpl, err := template.New("complex").Parse(goTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			var buf strings.Builder
			err := tmpl.Execute(&buf, goData)
			if err != nil {
				b.Fatal(err)
			}
			_ = buf.String()
		}
	})
}

// BenchConditional - Conditional rendering benchmark
func (s *Suite) BenchConditional(b *testing.B) {
	ginjaTemplate := `
{% for user in users %}
  {% if user.active %}
    <div class="active-user">{{ user.name }} ({{ user.age }})</div>
  {% else %}
    <div class="inactive-user">{{ user.name }} ({{ user.age }})</div>
  {% endif %}
{% endfor %}`

	goTemplate := `
{{ range .users }}
  {{ if .Active }}
    <div class="active-user">{{ .Name }} ({{ .Age }})</div>
  {{ else }}
    <div class="inactive-user">{{ .Name }} ({{ .Age }})</div>
  {{ end }}
{{ end }}`

	users := make([]BenchmarkData, 100)
	for i := range 100 {
		users[i] = BenchmarkData{
			Name:   "User" + string(rune('A'+(i%26))),
			Age:    20 + (i % 60),
			Active: i%3 == 0, // 1/3 active
			Score:  float64(i) * 10.5,
		}
	}

	data := map[string]any{
		"users": users,
	}

	b.Run("ginja", func(b *testing.B) {
		err := s.env.AddTemplate("conditional", ginjaTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			result, err := s.env.RenderTemplate("conditional", data)
			if err != nil {
				b.Fatal(err)
			}
			_ = result
		}
	})

	b.Run("std", func(b *testing.B) {
		tmpl, err := template.New("conditional").Parse(goTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ResetTimer()
		for b.Loop() {
			var buf strings.Builder
			err := tmpl.Execute(&buf, data)
			if err != nil {
				b.Fatal(err)
			}
			_ = buf.String()
		}
	})
}

// BenchTemplateCompilation - Template compilation benchmark
func (s *Suite) BenchTemplateCompilation(b *testing.B) {
	ginjaTemplate := `
<h1>{{ title }}</h1>
{% for user in users %}
  <div class="user">
    <h2>{{ user.name }}</h2>
    <p>Age: {{ user.age }}, Score: {{ user.score }}</p>
    {% if user.active %}
      <span class="status active">Active</span>
    {% else %}
      <span class="status inactive">Inactive</span>
    {% endif %}
  </div>
{% endfor %}`

	goTemplate := `
<h1>{{ .Title }}</h1>
{{ range .Users }}
  <div class="user">
    <h2>{{ .Name }}</h2>
    <p>Age: {{ .Age }}, Score: {{ .Score }}</p>
    {{ if .Active }}
      <span class="status active">Active</span>
    {{ else }}
      <span class="status inactive">Inactive</span>
    {{ end }}
  </div>
{{ end }}`

	b.Run("ginja", func(b *testing.B) {
		for b.Loop() {
			err := s.env.AddTemplate("test", ginjaTemplate)
			if err != nil {
				b.Fatal(err)
			}
		}
	})

	b.Run("std", func(b *testing.B) {
		t := template.New("test")
		for b.Loop() {
			_, err := t.Parse(goTemplate)
			if err != nil {
				b.Fatal(err)
			}
		}
	})
}

// BenchMemoryAllocation - Memory allocation benchmark
func (s *Suite) BenchMemoryAllocation(b *testing.B) {
	ginjaTemplate := "Result: {{ value1 }} + {{ value2 }} = {{ result }}"
	goTemplate := "Result: {{ .value1 }} + {{ .value2 }} = {{ .result }}"

	b.Run("ginja", func(b *testing.B) {
		err := s.env.AddTemplate("memory", ginjaTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ReportAllocs()
		b.ResetTimer()
		for i := 0; b.Loop(); i++ {
			data := map[string]any{
				"value1": i,
				"value2": i * 2,
				"result": i + i*2,
			}
			result, err := s.env.RenderTemplate("memory", data)
			if err != nil {
				b.Fatal(err)
			}
			_ = result
		}
	})

	b.Run("std", func(b *testing.B) {
		tmpl, err := template.New("memory").Parse(goTemplate)
		if err != nil {
			b.Fatal(err)
		}

		b.ReportAllocs()
		b.ResetTimer()
		for i := 0; b.Loop(); i++ {
			data := map[string]any{
				"value1": i,
				"value2": i * 2,
				"result": i + i*2,
			}
			var buf strings.Builder
			err := tmpl.Execute(&buf, data)
			if err != nil {
				b.Fatal(err)
			}
			_ = buf.String()
		}
	})
}

package ginja_test

import (
	"github.com/stretchr/testify/require"
)

func (s *Suite) TestRenderTemplateWithAnySlice(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_template", "Items: {{ items }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_template", map[string]any{
		"items": []any{"hello", 42, true, 3.14},
	})
	assert.Nil(err)
	assert.Contains(result, "Items:")
	assert.Contains(result, "hello")
	assert.Contains(result, "42")
	assert.Contains(result, "true")
	assert.Contains(result, "3.14")
}

func (s *Suite) TestRenderTemplateWithEmptyAnySlice(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("empty_any_slice_template", "Empty items: {{ items }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("empty_any_slice_template", map[string]any{
		"items": []any{},
	})
	assert.Nil(err)
	assert.Contains(result, "Empty items:")
}

func (s *Suite) TestRenderTemplateWithAnySliceIteration(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_iter_template", "{% for item in items %}{{ item }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_iter_template", map[string]any{
		"items": []any{"red", 100, false, 2.5},
	})
	assert.Nil(err)
	assert.Equal("red, 100, false, 2.5", result)
}

func (s *Suite) TestRenderTemplateWithMixedAnySlice(assert *require.Assertions) {
	env := s.env

	type Person struct {
		Name string
		Age  int
	}

	err := env.AddTemplate("mixed_any_slice_template", "Mixed: {% for item in items %}{{ item }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("mixed_any_slice_template", map[string]any{
		"items": []any{
			"Alice",
			25,
			true,
			3.14,
			Person{Name: "Bob", Age: 30},
			[]string{"a", "b"},
			map[string]string{"key": "value"},
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Alice")
	assert.Contains(result, "25")
	assert.Contains(result, "true")
	assert.Contains(result, "3.14")
	assert.Contains(result, "Bob")
}

func (s *Suite) TestRenderTemplateWithNestedAnySlice(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("nested_any_slice_template", "Nested: {{ data.items }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("nested_any_slice_template", map[string]any{
		"data": map[string]any{
			"items": []any{
				[]any{1, 2, 3},
				[]any{"a", "b", "c"},
				[]any{true, false},
			},
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Nested:")
}

func (s *Suite) TestRenderTemplateWithAnySliceContainingStructs(assert *require.Assertions) {
	env := s.env

	type User struct {
		ID   int
		Name string
	}

	err := env.AddTemplate("any_slice_structs_template", "Users: {% for user in users %}{{ user.Name }}({{ user.ID }}){% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_structs_template", map[string]any{
		"users": []any{
			User{ID: 1, Name: "Alice"},
			User{ID: 2, Name: "Bob"},
			User{ID: 3, Name: "Charlie"},
		},
	})
	assert.Nil(err)
	assert.Equal("Users: Alice(1), Bob(2), Charlie(3)", result)
}

func (s *Suite) TestRenderTemplateWithAnySliceContainingMaps(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_maps_template", "Data: {% for item in items %}{{ item.name }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_maps_template", map[string]any{
		"items": []any{
			map[string]string{"name": "Item1"},
			map[string]string{"name": "Item2"},
			map[string]string{"name": "Item3"},
		},
	})
	assert.Nil(err)
	assert.Equal("Data: Item1, Item2, Item3", result)
}

func (s *Suite) TestRenderTemplateWithAnySliceContainingNilValues(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_nil_template", "Items: {% for item in items %}{% if item %}{{ item }}{% else %}null{% endif %}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_nil_template", map[string]any{
		"items": []any{"hello", nil, 42, nil, true},
	})
	assert.Nil(err)
	assert.Equal("Items: hello, null, 42, null, true", result)
}

func (s *Suite) TestRenderTemplateWithAnySliceContainingSlices(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_nested_template", "Nested: {% for subarray in arrays %}{{ subarray }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_nested_template", map[string]any{
		"arrays": []any{
			[]int{1, 2, 3},
			[]string{"a", "b", "c"},
			[]bool{true, false},
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Nested:")
	assert.Contains(result, "[1, 2, 3]")
	assert.Contains(result, `["a", "b", "c"]`)
	assert.Contains(result, "[true, false]")
}

func (s *Suite) TestRenderTemplateWithAnySliceLengthFilter(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_length_template", "Array has {{ items|length }} items")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_length_template", map[string]any{
		"items": []any{"a", 1, true, 3.14, nil},
	})
	assert.Nil(err)
	assert.Contains(result, "Array has 5 items")
}

func (s *Suite) TestRenderTemplateWithAnySliceConditionals(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_conditionals_template", "{% if items|length > 0 %}Has items: {{ items|length }}{% else %}No items{% endif %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_conditionals_template", map[string]any{
		"items": []any{"hello", 42, true},
	})
	assert.Nil(err)
	assert.Contains(result, "Has items: 3")
}

func (s *Suite) TestRenderTemplateWithAnySliceComplex(assert *require.Assertions) {
	env := s.env

	type Product struct {
		Name  string
		Price float64
	}

	err := env.AddTemplate("any_slice_complex_template", "Complex: {% for item in items %}{% if item.Name %}{{ item.Name }}: ${{ item.Price }}{% else %}{{ item }}{% endif %}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_complex_template", map[string]any{
		"items": []any{
			Product{Name: "Laptop", Price: 999.99},
			"Simple String",
			42,
			Product{Name: "Mouse", Price: 29.99},
			true,
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Laptop: $999.99")
	assert.Contains(result, "Simple String")
	assert.Contains(result, "42")
	assert.Contains(result, "Mouse: $29.99")
	assert.Contains(result, "true")
}

func (s *Suite) TestRenderTemplateWithAnySliceContainingPointers(assert *require.Assertions) {
	env := s.env

	str := "hello"
	num := 42
	flag := true
	price := 19.99

	err := env.AddTemplate("any_slice_pointers_template", "Items: {% for item in items %}{{ item }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_pointers_template", map[string]any{
		"items": []any{&str, &num, &flag, &price},
	})
	assert.Nil(err)
	assert.Equal("Items: hello, 42, true, 19.99", result)
}

func (s *Suite) TestRenderTemplateWithAnySliceContainingPointerStructs(assert *require.Assertions) {
	env := s.env

	type User struct {
		ID   int
		Name string
	}

	err := env.AddTemplate("any_slice_pointer_structs_template", "Users: {% for user in users %}{{ user.Name }}({{ user.ID }}){% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_pointer_structs_template", map[string]any{
		"users": []any{
			&User{ID: 1, Name: "Alice"},
			&User{ID: 2, Name: "Bob"},
		},
	})
	assert.Nil(err)
	assert.Equal("Users: Alice(1), Bob(2)", result)
}

func (s *Suite) TestRenderTemplateWithAnySliceContaining2DArrays(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_2d_arrays_template", "Arrays: {% for arr in arrays %}{{ arr }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_2d_arrays_template", map[string]any{
		"arrays": []any{
			[][]int{{1, 2}, {3, 4}},
			[][]string{{"a", "b"}, {"c", "d"}},
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Arrays:")
}

func (s *Suite) TestRenderTemplateWithAnySliceContainingMixedPointers(assert *require.Assertions) {
	env := s.env

	type Person struct {
		Name string
		Age  int
	}

	name := "John"
	age := 30
	active := true
	person := &Person{Name: "Jane", Age: 25}

	err := env.AddTemplate("any_slice_mixed_pointers_template", "Mixed: {% for item in items %}{% if item.Name %}{{ item.Name }}({{ item.Age }}){% else %}{{ item }}{% endif %}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_mixed_pointers_template", map[string]any{
		"items": []any{&name, &age, &active, person},
	})
	assert.Nil(err)
	assert.Contains(result, "John")
	assert.Contains(result, "30")
	assert.Contains(result, "true")
	assert.Contains(result, "Jane(25)")
}

func (s *Suite) TestRenderTemplateWithAnySliceWithDifferentNumericTypes(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_numeric_types_template", "Numbers: {% for num in numbers %}{{ num }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_numeric_types_template", map[string]any{
		"numbers": []any{
			int8(8),
			int16(16),
			int32(32),
			int64(64),
			uint8(255),
			uint16(65535),
			uint32(4294967295),
			uint64(18446744073709551615),
			float32(3.14),
			float64(2.71828),
		},
	})
	assert.Nil(err)
	assert.Contains(result, "8")
	assert.Contains(result, "16")
	assert.Contains(result, "32")
	assert.Contains(result, "64")
	assert.Contains(result, "255")
	assert.Contains(result, "65535")
	assert.Contains(result, "4294967295")
	assert.Contains(result, "18446744073709551615")
	assert.Contains(result, "3.14")
	assert.Contains(result, "2.71828")
}

func (s *Suite) TestRenderTemplateWithAnySliceDeepNesting(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_deep_nesting_template", "Deep: {{ data }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_deep_nesting_template", map[string]any{
		"data": []any{
			map[string]any{
				"level1": []any{
					map[string]any{
						"level2": []any{
							"deep_value",
							42,
						},
					},
				},
			},
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Deep:")
}

func (s *Suite) TestRenderTemplateWithAnySliceEmptyAndNil(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("any_slice_empty_nil_template", "Items: {% for item in items %}{% if item == null %}NULL{% elif item == '' %}EMPTY{% else %}{{ item }}{% endif %}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_empty_nil_template", map[string]any{
		"items": []any{
			"",      // empty string
			nil,     // nil value
			0,       // zero
			false,   // false
			"valid", // valid string
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Items:")
}

func (s *Suite) TestRenderTemplateWithAnySliceContainingInterfaces(assert *require.Assertions) {
	env := s.env

	type CustomString string
	
	err := env.AddTemplate("any_slice_interfaces_template", "Items: {% for item in items %}{{ item }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("any_slice_interfaces_template", map[string]any{
		"items": []any{
			CustomString("custom1"),
			CustomString("custom2"),
			"regular_string",
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Items:")
	assert.Contains(result, "custom1")
	assert.Contains(result, "custom2")
	assert.Contains(result, "regular_string")
}


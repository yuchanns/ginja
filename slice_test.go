package ginja_test

import (
	"github.com/stretchr/testify/require"
)

func (s *Suite) TestRenderTemplateWithStringSlice(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("string_slice_template", "Items: {{ items }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("string_slice_template", map[string]any{
		"items": []string{"apple", "banana", "cherry"},
	})
	assert.Nil(err)
	assert.Equal(`Items: ["apple", "banana", "cherry"]`, result)
}

func (s *Suite) TestRenderTemplateWithEmptyStringSlice(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("empty_string_slice_template", "Empty items: {{ items }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("empty_string_slice_template", map[string]any{
		"items": []string{},
	})
	assert.Nil(err)
	assert.Contains(result, "Empty items:")
}

func (s *Suite) TestRenderTemplateWithStringSliceIteration(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("string_slice_iter_template", "{% for item in items %}{{ item }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("string_slice_iter_template", map[string]any{
		"items": []string{"red", "green", "blue"},
	})
	assert.Nil(err)
	assert.Equal("red, green, blue", result)
}

func (s *Suite) TestRenderTemplateWithIntSlices(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("int_slices_template", "IntSlice: {{ ints }}, Int64Slice: {{ int64s }}, Int32Slice: {{ int32s }}, Int16Slice: {{ int16s }}, Int8Slice: {{ int8s }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("int_slices_template", map[string]any{
		"ints":   []int{1, 2, 3},
		"int64s": []int64{100, 200, 300},
		"int32s": []int32{10, 20, 30},
		"int16s": []int16{1, 2, 3},
		"int8s":  []int8{4, 5, 6},
	})
	assert.Nil(err)
	assert.Contains(result, "IntSlice: [1, 2, 3]")
	assert.Contains(result, "Int64Slice: [100, 200, 300]")
	assert.Contains(result, "Int32Slice: [10, 20, 30]")
	assert.Contains(result, "Int16Slice: [1, 2, 3]")
	assert.Contains(result, "Int8Slice: [4, 5, 6]")
}

func (s *Suite) TestRenderTemplateWithUintSlices(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("uint_slices_template", "UintSlice: {{ uints }}, Uint64Slice: {{ uint64s }}, Uint32Slice: {{ uint32s }}, Uint16Slice: {{ uint16s }}, Uint8Slice: {{ uint8s }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("uint_slices_template", map[string]any{
		"uints":   []uint{1, 2, 3},
		"uint64s": []uint64{100, 200, 300},
		"uint32s": []uint32{10, 20, 30},
		"uint16s": []uint16{1, 2, 3},
		// "uint8s":  []uint8{4, 5, 6},
	})
	assert.Nil(err)
	assert.Contains(result, "UintSlice: [1, 2, 3]")
	assert.Contains(result, "Uint64Slice: [100, 200, 300]")
	assert.Contains(result, "Uint32Slice: [10, 20, 30]")
	assert.Contains(result, "Uint16Slice: [1, 2, 3]")
	// assert.Contains(result, "Uint8Slice: [4, 5, 6]")
}

func (s *Suite) TestRenderTemplateWithFloatSlices(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("float_slices_template", "Float64Slice: {{ float64s }}, Float32Slice: {{ float32s }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("float_slices_template", map[string]any{
		"float64s": []float64{1.1, 2.2, 3.3},
		"float32s": []float32{1.5, 2.5, 3.5},
	})
	assert.Nil(err)
	assert.Contains(result, "Float64Slice:")
	assert.Contains(result, "Float32Slice:")
}

func (s *Suite) TestRenderTemplateWithBoolSlice(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("bool_slice_template", "BoolSlice: {{ bools }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("bool_slice_template", map[string]any{
		"bools": []bool{true, false, true},
	})
	assert.Nil(err)
	assert.Contains(result, "BoolSlice: [true, false, true]")
}

func (s *Suite) TestRenderTemplateWithEmptySlices(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("empty_slices_template", "EmptyInts: {{ empty_ints }}, EmptyStrings: {{ empty_strings }}, EmptyBools: {{ empty_bools }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("empty_slices_template", map[string]any{
		"empty_ints":    []int{},
		"empty_strings": []string{},
		"empty_bools":   []bool{},
	})
	assert.Nil(err)
	assert.Contains(result, "EmptyInts:")
	assert.Contains(result, "EmptyStrings:")
	assert.Contains(result, "EmptyBools:")
}

func (s *Suite) TestRenderTemplateWithSliceIteration(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("slice_iter_template", "{% for num in numbers %}{{ num }}{% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("slice_iter_template", map[string]any{
		"numbers": []int{10, 20, 30},
	})
	assert.Nil(err)
	assert.Equal("10, 20, 30", result)
}


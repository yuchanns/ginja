package ginja_test

import (
	"github.com/stretchr/testify/require"
)

func (s *Suite) TestRenderTemplateWithIntTypes(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("int_template", "Age: {{ age }}, Count: {{ count }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("int_template", map[string]any{
		"age":   25,
		"count": int64(100),
	})
	assert.Nil(err)
	assert.Equal("Age: 25, Count: 100", result)
}

func (s *Suite) TestRenderTemplateWithIntSubTypes(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("int_subtypes_template_2", "Int32: {{ val32 }}, Int16: {{ val16 }}, Int8: {{ val8 }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("int_subtypes_template_2", map[string]any{
		"val32": int32(32),
		"val16": int16(16),
		"val8":  int8(8),
	})
	assert.Nil(err)
	assert.Equal("Int32: 32, Int16: 16, Int8: 8", result)
}

func (s *Suite) TestRenderTemplateWithUintTypes(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("uint_template_3", "Size: {{ size }}, Max: {{ max }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("uint_template_3", map[string]any{
		"size": uint(256),
		"max":  uint64(18446744073709551615),
	})
	assert.Nil(err)
	assert.Equal("Size: 256, Max: 18446744073709551615", result)
}

func (s *Suite) TestRenderTemplateWithUintSubTypes(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("uint_subtypes_template_4", "Uint32: {{ val32 }}, Uint16: {{ val16 }}, Uint8: {{ val8 }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("uint_subtypes_template_4", map[string]any{
		"val32": uint32(4294967295),
		"val16": uint16(65535),
		"val8":  uint8(255),
	})
	assert.Nil(err)
	assert.Equal("Uint32: 4294967295, Uint16: 65535, Uint8: 255", result)
}

func (s *Suite) TestRenderTemplateWithFloatTypes(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("float_template_5", "Price: {{ price }}, Rate: {{ rate }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("float_template_5", map[string]any{
		"price": 19.99,
		"rate":  float32(0.15),
	})
	assert.Nil(err)
	// Float32 precision may cause slight differences in output
	assert.True(result == "Price: 19.99, Rate: 0.15" || result == "Price: 19.99, Rate: 0.15000000596046448",
		"Expected 'Price: 19.99, Rate: 0.15' or similar float precision, got '%s'", result)
}

func (s *Suite) TestRenderTemplateWithBoolTypes(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("bool_template_7", "IsActive: {{ active }}, IsEnabled: {{ enabled }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("bool_template_7", map[string]any{
		"active":  true,
		"enabled": false,
	})
	assert.Nil(err)
	assert.Equal("IsActive: true, IsEnabled: false", result)
}

func (s *Suite) TestRenderTemplateWithMixedTypes(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("mixed_template_6", "Name: {{ name }}, Age: {{ age }}, Score: {{ score }}, Active: {{ active }}, IsEnabled: {{ enabled }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("mixed_template_6", map[string]any{
		"name":    "Alice",
		"age":     int32(30),
		"score":   95.5,
		"active":  uint8(1),
		"enabled": false,
	})
	assert.Nil(err)
	assert.Equal("Name: Alice, Age: 30, Score: 95.5, Active: 1, IsEnabled: false", result)
}


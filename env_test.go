package ginja_test

import (
	"testing"

	"github.com/stretchr/testify/require"
	"go.yuchanns.xyz/ginja"
)

var env *ginja.Environment

func TestMain(m *testing.M) {
	var err error
	env, err = ginja.NewEnvironment()
	if err != nil {
		panic(err)
	}
	defer env.Close()

	m.Run()
}

func TestAddTemplate(t *testing.T) {
	t.Parallel()

	err := env.AddTemplate("test_template", "Hello, {{ name }}!")
	require.NoError(t, err)
	result, err := env.RenderTemplate("test_template", map[string]any{
		"name": "World",
	})
	require.NoError(t, err)
	require.Equal(t, "Hello, World!", result)
}

func TestRenderTemplateWithIntTypes(t *testing.T) {
	t.Parallel()

	err := env.AddTemplate("int_template", "Age: {{ age }}, Count: {{ count }}")
	require.NoError(t, err)

	result, err := env.RenderTemplate("int_template", map[string]any{
		"age":   25,
		"count": int64(100),
	})
	require.NoError(t, err)
	require.Equal(t, "Age: 25, Count: 100", result)
}

func TestRenderTemplateWithIntSubTypes(t *testing.T) {
	t.Parallel()

	err := env.AddTemplate("int_subtypes_template_2", "Int32: {{ val32 }}, Int16: {{ val16 }}, Int8: {{ val8 }}")
	require.NoError(t, err)

	result, err := env.RenderTemplate("int_subtypes_template_2", map[string]any{
		"val32": int32(32),
		"val16": int16(16),
		"val8":  int8(8),
	})
	require.NoError(t, err)
	require.Equal(t, "Int32: 32, Int16: 16, Int8: 8", result)
}

func TestRenderTemplateWithUintTypes(t *testing.T) {
	t.Parallel()

	err := env.AddTemplate("uint_template_3", "Size: {{ size }}, Max: {{ max }}")
	require.NoError(t, err)

	result, err := env.RenderTemplate("uint_template_3", map[string]any{
		"size": uint(256),
		"max":  uint64(18446744073709551615),
	})
	require.NoError(t, err)
	require.Equal(t, "Size: 256, Max: 18446744073709551615", result)
}

func TestRenderTemplateWithUintSubTypes(t *testing.T) {
	t.Parallel()

	err := env.AddTemplate("uint_subtypes_template_4", "Uint32: {{ val32 }}, Uint16: {{ val16 }}, Uint8: {{ val8 }}")
	require.NoError(t, err)

	result, err := env.RenderTemplate("uint_subtypes_template_4", map[string]any{
		"val32": uint32(4294967295),
		"val16": uint16(65535),
		"val8":  uint8(255),
	})
	require.NoError(t, err)
	require.Equal(t, "Uint32: 4294967295, Uint16: 65535, Uint8: 255", result)
}

func TestRenderTemplateWithFloatTypes(t *testing.T) {
	t.Parallel()

	err := env.AddTemplate("float_template_5", "Price: {{ price }}, Rate: {{ rate }}")
	require.NoError(t, err)

	result, err := env.RenderTemplate("float_template_5", map[string]any{
		"price": 19.99,
		"rate":  float32(0.15),
	})
	require.NoError(t, err)
	// Float32 precision may cause slight differences in output
	require.True(t, result == "Price: 19.99, Rate: 0.15" || result == "Price: 19.99, Rate: 0.15000000596046448",
		"Expected 'Price: 19.99, Rate: 0.15' or similar float precision, got '%s'", result)
}

func TestRenderTemplateWithMixedTypes(t *testing.T) {
	t.Parallel()

	err := env.AddTemplate("mixed_template_6", "Name: {{ name }}, Age: {{ age }}, Score: {{ score }}, Active: {{ active }}")
	require.NoError(t, err)

	result, err := env.RenderTemplate("mixed_template_6", map[string]any{
		"name":   "Alice",
		"age":    int32(30),
		"score":  95.5,
		"active": uint8(1),
	})
	require.NoError(t, err)
	require.Equal(t, "Name: Alice, Age: 30, Score: 95.5, Active: 1", result)
}

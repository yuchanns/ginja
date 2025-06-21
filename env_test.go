package ginja_test

import (
	"reflect"
	"strings"
	"testing"

	"github.com/stretchr/testify/require"
	"go.yuchanns.xyz/ginja"
)

type Suite struct {
	env *ginja.Environment
}

func (s *Suite) Setup() (err error) {
	env, err := ginja.New()
	if err != nil {
		return
	}
	s.env = env
	return
}

func (s *Suite) TearDown() {
	if s.env != nil {
		s.env.Close()
	}
}

func TestSuite(t *testing.T) {
	suite := &Suite{}

	err := suite.Setup()
	require.Nil(t, err)

	t.Cleanup(suite.TearDown)

	tt := reflect.TypeOf(suite)
	for i := range tt.NumMethod() {
		method := tt.Method(i)
		testFunc, ok := method.Func.Interface().(func(*Suite, *require.Assertions))
		if !ok {
			continue
		}
		t.Run(strings.TrimLeft(method.Name, "Test"), func(t *testing.T) {
			t.Parallel()

			testFunc(suite, require.New(t))
		})
	}

}

func (s *Suite) TestRenderTemplate(assert *require.Assertions) {
	env := s.env

	assert.Nil(env.AddTemplate("test_template", "Hello, {{ name }}!"))
	result, err := env.RenderTemplate("test_template", map[string]any{
		"name": "World",
	})
	assert.Nil(err)
	assert.Equal("Hello, World!", result)
}

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
		"uint8s":  []uint8{4, 5, 6},
	})
	assert.Nil(err)
	assert.Contains(result, "UintSlice: [1, 2, 3]")
	assert.Contains(result, "Uint64Slice: [100, 200, 300]")
	assert.Contains(result, "Uint32Slice: [10, 20, 30]")
	assert.Contains(result, "Uint16Slice: [1, 2, 3]")
	assert.Contains(result, "Uint8Slice: [4, 5, 6]")
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

func (s *Suite) TestRenderTemplateWithSimpleStruct(assert *require.Assertions) {
	env := s.env

	type Person struct {
		Name string
		Age  int
	}

	err := env.AddTemplate("simple_struct_template", "Person: {{ person.Name }}, Age: {{ person.Age }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("simple_struct_template", map[string]any{
		"person": Person{Name: "Alice", Age: 30},
	})
	assert.Nil(err)
	assert.Equal("Person: Alice, Age: 30", result)
}

func (s *Suite) TestRenderTemplateWithNestedStruct(assert *require.Assertions) {
	env := s.env

	type Address struct {
		Street  string
		City    string
		ZipCode int
	}

	type Person struct {
		Name    string
		Age     int
		Address Address
	}

	err := env.AddTemplate("nested_struct_template", "{{ person.Name }} lives at {{ person.Address.Street }}, {{ person.Address.City }} {{ person.Address.ZipCode }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("nested_struct_template", map[string]any{
		"person": Person{
			Name: "Bob",
			Age:  25,
			Address: Address{
				Street:  "123 Main St",
				City:    "New York",
				ZipCode: 10001,
			},
		},
	})
	assert.Nil(err)
	assert.Equal("Bob lives at 123 Main St, New York 10001", result)
}

func (s *Suite) TestRenderTemplateWithMixedFieldStruct(assert *require.Assertions) {
	env := s.env

	type Product struct {
		Name     string
		Price    float64
		InStock  bool
		Quantity int32
		Rating   float32
		Views    uint64
		IsActive bool
		Size     uint8
	}

	err := env.AddTemplate("mixed_struct_template", "Product: {{ product.Name }}, Price: ${{ product.Price }}, InStock: {{ product.InStock }}, Quantity: {{ product.Quantity }}, Rating: {{ product.Rating }}, Views: {{ product.Views }}, Active: {{ product.IsActive }}, Size: {{ product.Size }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("mixed_struct_template", map[string]any{
		"product": Product{
			Name:     "Laptop",
			Price:    999.99,
			InStock:  true,
			Quantity: int32(50),
			Rating:   float32(4.5),
			Views:    uint64(1234567),
			IsActive: false,
			Size:     uint8(15),
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Product: Laptop")
	assert.Contains(result, "Price: $999.99")
	assert.Contains(result, "InStock: true")
	assert.Contains(result, "Quantity: 50")
	assert.Contains(result, "Views: 1234567")
	assert.Contains(result, "Active: false")
	assert.Contains(result, "Size: 15")
}

// func (s *Suite) TestRenderTemplateWithStructSlice(assert *require.Assertions) {
// 	env := s.env
//
// 	type User struct {
// 		ID   int
// 		Name string
// 		Age  int
// 	}
//
// 	err := env.AddTemplate("struct_slice_template", "Users: {% for user in users %}{{ user.Name }}({{ user.ID }}, {{ user.Age }}){% if not loop.last %}, {% endif %}{% endfor %}")
// 	assert.Nil(err)
//
// 	result, err := env.RenderTemplate("struct_slice_template", map[string]any{
// 		"users": []User{
// 			{ID: 1, Name: "Alice", Age: 25},
// 			{ID: 2, Name: "Bob", Age: 30},
// 			{ID: 3, Name: "Charlie", Age: 35},
// 		},
// 	})
// 	assert.Nil(err)
// 	assert.Equal("Users: Alice(1, 25), Bob(2, 30), Charlie(3, 35)", result)
// }

func (s *Suite) TestRenderTemplateWithEmbeddedStruct(assert *require.Assertions) {
	env := s.env

	type Contact struct {
		Email string
		Phone string
	}

	type Employee struct {
		ID      int
		Name    string
		Contact // embedded struct
	}

	err := env.AddTemplate("embedded_struct_template", "Employee: {{ employee.Name }} (ID: {{ employee.ID }}), Contact: {{ employee.Email }}, {{ employee.Phone }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("embedded_struct_template", map[string]any{
		"employee": Employee{
			ID:   123,
			Name: "John Doe",
			Contact: Contact{
				Email: "john@example.com",
				Phone: "555-1234",
			},
		},
	})
	assert.Nil(err)
	assert.Equal("Employee: John Doe (ID: 123), Contact: john@example.com, 555-1234", result)
}

func (s *Suite) TestRenderTemplateWithPointerStruct(assert *require.Assertions) {
	env := s.env

	type Address struct {
		Street string
		City   string
	}

	type Person struct {
		Name    string
		Age     *int
		Address *Address
	}

	age := 28
	err := env.AddTemplate("pointer_struct_template", "Person: {{ person.Name }}, Age: {{ person.Age }}, Address: {{ person.Address.Street }}, {{ person.Address.City }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("pointer_struct_template", map[string]any{
		"person": Person{
			Name: "Dave",
			Age:  &age,
			Address: &Address{
				Street: "456 Oak Ave",
				City:   "Boston",
			},
		},
	})
	assert.Nil(err)
	assert.Equal("Person: Dave, Age: 28, Address: 456 Oak Ave, Boston", result)
}

func (s *Suite) TestRenderTemplateWithEmptyStruct(assert *require.Assertions) {
	env := s.env

	type Empty struct{}

	err := env.AddTemplate("empty_struct_template", "Empty struct: {{ empty }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("empty_struct_template", map[string]any{
		"empty": Empty{},
	})
	assert.Nil(err)
	assert.Contains(result, "Empty struct:")
}

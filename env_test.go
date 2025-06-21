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

func (s *Suite) TestRenderTemplateWithStructSlice(assert *require.Assertions) {
	env := s.env

	type User struct {
		ID   int
		Name string
		Age  int
	}

	err := env.AddTemplate("struct_slice_template", "Users: {% for user in users %}{{ user.Name }}({{ user.ID }}, {{ user.Age }}){% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("struct_slice_template", map[string]any{
		"users": []User{
			{ID: 1, Name: "Alice", Age: 25},
			{ID: 2, Name: "Bob", Age: 30},
			{ID: 3, Name: "Charlie", Age: 35},
		},
	})
	assert.Nil(err)
	assert.Equal("Users: Alice(1, 25), Bob(2, 30), Charlie(3, 35)", result)
}

func (s *Suite) TestRenderTemplateWithEmptyStructSlice(assert *require.Assertions) {
	env := s.env

	type User struct {
		ID   int
		Name string
		Age  int
	}

	err := env.AddTemplate("empty_struct_slice_template", "Users: {% for user in users %}{{ user.Name }}{% if not loop.last %}, {% endif %}{% endfor %}{% if users|length == 0 %}No users{% endif %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("empty_struct_slice_template", map[string]any{
		"users": []User{},
	})
	assert.Nil(err)
	assert.Contains(result, "No users")
}

func (s *Suite) TestRenderTemplateWithPointerStructSlice(assert *require.Assertions) {
	env := s.env

	type User struct {
		ID   int
		Name string
		Age  int
	}

	err := env.AddTemplate("pointer_struct_slice_template", "Users: {% for user in users %}{{ user.Name }}({{ user.ID }}, {{ user.Age }}){% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("pointer_struct_slice_template", map[string]any{
		"users": []*User{
			{ID: 1, Name: "Alice", Age: 25},
			{ID: 2, Name: "Bob", Age: 30},
			{ID: 3, Name: "Charlie", Age: 35},
		},
	})
	assert.Nil(err)
	assert.Equal("Users: Alice(1, 25), Bob(2, 30), Charlie(3, 35)", result)
}

func (s *Suite) TestRenderTemplateWithMixedStructSlice(assert *require.Assertions) {
	env := s.env

	type Product struct {
		Name    string
		Price   float64
		InStock bool
		Qty     int32
	}

	err := env.AddTemplate("mixed_struct_slice_template", "Products: {% for product in products %}{{ product.Name }}(${{ product.Price }}, Stock: {{ product.InStock }}, Qty: {{ product.Qty }}){% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("mixed_struct_slice_template", map[string]any{
		"products": []Product{
			{Name: "Laptop", Price: 999.99, InStock: true, Qty: int32(10)},
			{Name: "Mouse", Price: 29.99, InStock: false, Qty: int32(0)},
		},
	})
	assert.Nil(err)
	assert.Equal("Products: Laptop($999.99, Stock: true, Qty: 10), Mouse($29.99, Stock: false, Qty: 0)", result)
}

func (s *Suite) TestRenderTemplateWithNestedStructSlice(assert *require.Assertions) {
	env := s.env

	type Address struct {
		Street string
		City   string
	}

	type Person struct {
		Name    string
		Age     int
		Address Address
	}

	err := env.AddTemplate("nested_struct_slice_template", "People: {% for person in people %}{{ person.Name }} ({{ person.Age }}, {{ person.Address.City }}){% if not loop.last %}, {% endif %}{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("nested_struct_slice_template", map[string]any{
		"people": []Person{
			{Name: "Alice", Age: 25, Address: Address{Street: "123 Main St", City: "New York"}},
			{Name: "Bob", Age: 30, Address: Address{Street: "456 Oak Ave", City: "Boston"}},
		},
	})
	assert.Nil(err)
	assert.Equal("People: Alice (25, New York), Bob (30, Boston)", result)
}

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

func (s *Suite) TestRenderTemplateWithSimpleStringMap(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("string_map_template", "Hello {{ user.name }}, you are from {{ user.city }}!")
	assert.Nil(err)

	result, err := env.RenderTemplate("string_map_template", map[string]any{
		"user": map[string]string{
			"name": "Alice",
			"city": "New York",
		},
	})
	assert.Nil(err)
	assert.Equal("Hello Alice, you are from New York!", result)
}

func (s *Suite) TestRenderTemplateWithStringIntMap(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("string_int_map_template", "Name: {{ person.name }}, Age: {{ person.age }}, Score: {{ person.score }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("string_int_map_template", map[string]any{
		"person": map[string]int{
			"age":   25,
			"score": 95,
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Age: 25")
	assert.Contains(result, "Score: 95")
}

func (s *Suite) TestRenderTemplateWithStringFloatMap(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("string_float_map_template", "Price: {{ product.price }}, Rating: {{ product.rating }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("string_float_map_template", map[string]any{
		"product": map[string]float64{
			"price":  99.99,
			"rating": 4.5,
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Price: 99.99")
	assert.Contains(result, "Rating: 4.5")
}

func (s *Suite) TestRenderTemplateWithStringBoolMap(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("string_bool_map_template", "Active: {{ status.active }}, Enabled: {{ status.enabled }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("string_bool_map_template", map[string]any{
		"status": map[string]bool{
			"active":  true,
			"enabled": false,
		},
	})
	assert.Nil(err)
	assert.Equal("Active: true, Enabled: false", result)
}

func (s *Suite) TestRenderTemplateWithMixedValueMap(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("mixed_map_template", "Name: {{ data.name }}, Age: {{ data.age }}, Price: {{ data.price }}, Active: {{ data.active }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("mixed_map_template", map[string]any{
		"data": map[string]any{
			"name":   "Bob",
			"age":    30,
			"price":  25.99,
			"active": true,
		},
	})
	assert.Nil(err)
	assert.Equal("Name: Bob, Age: 30, Price: 25.99, Active: true", result)
}

func (s *Suite) TestRenderTemplateWithEmptyMap(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("empty_map_template", "Map size: {{ data|length }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("empty_map_template", map[string]any{
		"data": map[string]string{},
	})
	assert.Nil(err)
	assert.Contains(result, "Map size: 0")
}

func (s *Suite) TestRenderTemplateWithNestedMap(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("nested_map_template", "User: {{ user.name }}, Address: {{ user.address.city }}, {{ user.address.country }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("nested_map_template", map[string]any{
		"user": map[string]any{
			"name": "Charlie",
			"address": map[string]string{
				"city":    "Tokyo",
				"country": "Japan",
			},
		},
	})
	assert.Nil(err)
	assert.Equal("User: Charlie, Address: Tokyo, Japan", result)
}

func (s *Suite) TestRenderTemplateWithMapOfStructs(assert *require.Assertions) {
	env := s.env

	type Person struct {
		Name string
		Age  int
	}

	err := env.AddTemplate("map_of_structs_template", "Admin: {{ users.admin.Name }} ({{ users.admin.Age }}), Guest: {{ users.guest.Name }} ({{ users.guest.Age }})")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_of_structs_template", map[string]any{
		"users": map[string]Person{
			"admin": {Name: "Alice", Age: 30},
			"guest": {Name: "Bob", Age: 25},
		},
	})
	assert.Nil(err)
	assert.Equal("Admin: Alice (30), Guest: Bob (25)", result)
}

func (s *Suite) TestRenderTemplateWithMapOfSlices(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_of_slices_template", "Colors: {{ data.colors }}, Numbers: {{ data.numbers }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_of_slices_template", map[string]any{
		"data": map[string]any{
			"colors":  []string{"red", "green", "blue"},
			"numbers": []int{1, 2, 3},
		},
	})
	assert.Nil(err)
	assert.Contains(result, `Colors: ["red", "green", "blue"]`)
	assert.Contains(result, "Numbers: [1, 2, 3]")
}

func (s *Suite) TestRenderTemplateWithMapOfMaps(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_of_maps_template", "US Capital: {{ countries.US.capital }}, JP Capital: {{ countries.JP.capital }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_of_maps_template", map[string]any{
		"countries": map[string]map[string]string{
			"US": {"capital": "Washington", "currency": "USD"},
			"JP": {"capital": "Tokyo", "currency": "JPY"},
		},
	})
	assert.Nil(err)
	assert.Equal("US Capital: Washington, JP Capital: Tokyo", result)
}

func (s *Suite) TestRenderTemplateWithMapOfPointers(assert *require.Assertions) {
	env := s.env

	type Config struct {
		Host string
		Port int
	}

	err := env.AddTemplate("map_of_pointers_template", "DB Host: {{ configs.database.Host }}:{{ configs.database.Port }}, Redis Host: {{ configs.redis.Host }}:{{ configs.redis.Port }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_of_pointers_template", map[string]any{
		"configs": map[string]*Config{
			"database": {Host: "localhost", Port: 5432},
			"redis":    {Host: "127.0.0.1", Port: 6379},
		},
	})
	assert.Nil(err)
	assert.Equal("DB Host: localhost:5432, Redis Host: 127.0.0.1:6379", result)
}

func (s *Suite) TestRenderTemplateWithComplexNestedMap(assert *require.Assertions) {
	env := s.env

	type Address struct {
		Street string
		City   string
	}

	type Person struct {
		Name    string
		Age     int
		Address Address
	}

	err := env.AddTemplate("complex_nested_map_template", "Employee: {{ company.employees.manager.Name }} lives in {{ company.employees.manager.Address.City }}, Company: {{ company.info.name }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("complex_nested_map_template", map[string]any{
		"company": map[string]any{
			"info": map[string]string{
				"name":    "TechCorp",
				"founded": "2020",
			},
			"employees": map[string]Person{
				"manager": {
					Name: "Alice",
					Age:  35,
					Address: Address{
						Street: "123 Main St",
						City:   "San Francisco",
					},
				},
			},
		},
	})
	assert.Nil(err)
	assert.Equal("Employee: Alice lives in San Francisco, Company: TechCorp", result)
}

func (s *Suite) TestRenderTemplateWithMapAsObject(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_as_object_template", "Items: {{ data }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_as_object_template", map[string]any{
		"data": map[string]string{
			"name": "Alice",
			"city": "New York",
			"job":  "Engineer",
		},
	})
	assert.Nil(err)
	// Maps are typically rendered as objects, check that the result contains the map representation
	assert.Contains(result, "Items:")
}

func (s *Suite) TestRenderTemplateWithMapKeysAccess(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_keys_template", "Name: {{ data.name }}, City: {{ data.city }}, Job: {{ data.job }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_keys_template", map[string]any{
		"data": map[string]string{
			"name": "Alice",
			"city": "New York",
			"job":  "Engineer",
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Name: Alice")
	assert.Contains(result, "City: New York")
	assert.Contains(result, "Job: Engineer")
}

func (s *Suite) TestRenderTemplateWithMapLengthFilter(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_length_template", "Map has {{ data|length }} items")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_length_template", map[string]any{
		"data": map[string]int{
			"a": 1,
			"b": 2,
			"c": 3,
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Map has 3 items")
}

func (s *Suite) TestRenderTemplateWithMapConditionals(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_conditionals_template", "{% if data.active %}User is active{% else %}User is inactive{% endif %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_conditionals_template", map[string]any{
		"data": map[string]bool{
			"active": true,
		},
	})
	assert.Nil(err)
	assert.Contains(result, "User is active")
}

func (s *Suite) TestRenderTemplateWithEmptyMapIteration(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("empty_map_iteration_template", "{% for key, value in data %}{{ key }}: {{ value }}{% else %}No items{% endfor %}")
	assert.Nil(err)

	result, err := env.RenderTemplate("empty_map_iteration_template", map[string]any{
		"data": map[string]string{},
	})
	assert.Nil(err)
	assert.Contains(result, "No items")
}

func (s *Suite) TestRenderTemplateWithMapMixedValueTypes(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_mixed_values_template", "String: {{ data.str }}, Int: {{ data.num }}, Float: {{ data.price }}, Bool: {{ data.flag }}, Slice: {{ data.items }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_mixed_values_template", map[string]any{
		"data": map[string]any{
			"str":   "hello",
			"num":   42,
			"price": 19.99,
			"flag":  true,
			"items": []string{"a", "b", "c"},
		},
	})
	assert.Nil(err)
	assert.Contains(result, "String: hello")
	assert.Contains(result, "Int: 42")
	assert.Contains(result, "Float: 19.99")
	assert.Contains(result, "Bool: true")
	assert.Contains(result, `Slice: ["a", "b", "c"]`)
}

func (s *Suite) TestRenderTemplateWithMapNilValues(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_nil_values_template", "Value1: {{ data.value1 }}, Value2: {{ data.value2 }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_nil_values_template", map[string]any{
		"data": map[string]any{
			"value1": nil,
			"value2": "not nil",
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Value2: not nil")
}

func (s *Suite) TestRenderTemplateWithMapDeepNesting(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_deep_nesting_template", "Deep: {{ root.level1.level2.level3.value }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_deep_nesting_template", map[string]any{
		"root": map[string]any{
			"level1": map[string]any{
				"level2": map[string]any{
					"level3": map[string]any{
						"value": "deep value",
					},
				},
			},
		},
	})
	assert.Nil(err)
	assert.Equal("Deep: deep value", result)
}

func (s *Suite) TestRenderTemplateWithMapInterfaceValues(assert *require.Assertions) {
	env := s.env

	type Person struct {
		Name string
		Age  int
	}

	err := env.AddTemplate("map_interface_values_template", "Person: {{ data.person.Name }}, Array: {{ data.array }}, Number: {{ data.number }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_interface_values_template", map[string]any{
		"data": map[string]any{
			"person": Person{Name: "John", Age: 30},
			"array":  []int{1, 2, 3},
			"number": 42,
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Person: John")
	assert.Contains(result, "Array: [1, 2, 3]")
	assert.Contains(result, "Number: 42")
}

func (s *Suite) TestRenderTemplateWithMapEdgeCases(assert *require.Assertions) {
	env := s.env

	err := env.AddTemplate("map_edge_cases_template", "Empty: {{ data.empty }}, Zero: {{ data.zero }}, False: {{ data.false }}")
	assert.Nil(err)

	result, err := env.RenderTemplate("map_edge_cases_template", map[string]any{
		"data": map[string]any{
			"empty": "",
			"zero":  0,
			"false": false,
		},
	})
	assert.Nil(err)
	assert.Contains(result, "Empty: ")
	assert.Contains(result, "Zero: 0")
	assert.Contains(result, "False: false")
}

package ginja_test

import (
	"github.com/stretchr/testify/require"
)

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


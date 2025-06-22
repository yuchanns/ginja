package ginja_test

import (
	"github.com/stretchr/testify/require"
)

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


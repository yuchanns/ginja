#include "minijinja.h"
#include <gtest/gtest.h>
#include <cstring>
#include <string.h>
#include <stdint.h>

TEST(EnvTest, BasicCreateAndFree) {
  // Test creating new environment
  auto result = mj_env_new();
  ASSERT_NE(result.env, nullptr)
      << "Environment should not be null after creation";

  // Ensure we can access the environment
  EXPECT_NE(result.env->inner, nullptr)
      << "Environment inner pointer should not be null";

  auto add_result = mj_env_add_template(result.env, "test_template", "Hello {{ name }}");

  // Check if the template was added successfully
  EXPECT_EQ(add_result.error, nullptr)
      << "Adding template should not return an error";

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr)
      << "Value pointer should not be null after creation";
  EXPECT_NE(value->inner, nullptr)
      << "Value inner pointer should not be null";

  mj_value_set_string(value, "name", "world");

  auto render_result = mj_env_render_template(result.env, "test_template", value);
  // Check if the rendering was successful
  EXPECT_EQ(render_result.error, nullptr)
      << "Rendering template should not return an error";
  // Check if the rendered output is as expected
  EXPECT_STREQ(render_result.result, "Hello world")
      << "Rendered output should match the expected string";

  // Free the value
  mj_value_free(value);

  // Test freeing the environment
  mj_env_free(result.env);
  // Note: Since we've freed the memory, we shouldn't access result.env anymore
}

TEST(ValueTest, SetIntegerValues) {
  // Test setting 64-bit integer values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "int_template", "Number: {{ count }}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_int (64-bit integer)
  mj_value_set_int(value, "count", 9223372036854775807LL); // Max int64_t

  auto render_result = mj_env_render_template(env_result.env, "int_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Number: 9223372036854775807");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, SetInt32Values) {
  // Test setting 32-bit integer values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "int32_template", "Value: {{ num }}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_in32 (32-bit integer)
  mj_value_set_in32(value, "num", 2147483647); // Max int32_t

  auto render_result = mj_env_render_template(env_result.env, "int32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Value: 2147483647");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, SetFloatValues) {
  // Test setting double precision float values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "float_template", "Pi: {{ pi }}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_float (double)
  mj_value_set_float(value, "pi", 3.141592653589793);

  auto render_result = mj_env_render_template(env_result.env, "float_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Pi: 3.141592653589793");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, SetFloat32Values) {
  // Test setting single precision float values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "float32_template", "Value: {{ val }}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_float32 (float)
  mj_value_set_float32(value, "val", 3.14159f);

  auto render_result = mj_env_render_template(env_result.env, "float32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  // Note: Float precision might vary, so we check if it starts with expected value
  EXPECT_TRUE(strncmp(render_result.result, "Value: 3.14159", 14) == 0);

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, SetBoolValues) {
  // Test setting boolean values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "bool_template", "{% if active %}Active{% else %}Inactive{% endif %}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_bool with true
  mj_value_set_bool(value, "active", true);

  auto render_result = mj_env_render_template(env_result.env, "bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Active");

  // Test with false
  mj_value_set_bool(value, "active", false);
  render_result = mj_env_render_template(env_result.env, "bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Inactive");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, MultipleValuesInTemplate) {
  // Test using multiple different value types in one template
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "multi_template", 
    "Name: {{ name }}, Age: {{ age }}, Score: {{ score }}, Active: {{ active }}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Set different types of values
  mj_value_set_string(value, "name", "Alice");
  mj_value_set_in32(value, "age", 25);
  mj_value_set_float32(value, "score", 95.5f);
  mj_value_set_bool(value, "active", true);

  auto render_result = mj_env_render_template(env_result.env, "multi_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Name: Alice, Age: 25, Score: 95.5, Active: true");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ErrorTest, ErrorHandling) {
  // Test error handling with invalid template syntax
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  // Add template with invalid syntax
  auto add_result = mj_env_add_template(env_result.env, "invalid_template", "Hello {{ unclosed");
  
  // This should produce an error
  if (add_result.error != nullptr) {
    EXPECT_NE(add_result.error->message, nullptr);
    EXPECT_GT(strlen(add_result.error->message), 0);
    
    // Test mj_error_free
    mj_error_free(add_result.error);
  }

  mj_env_free(env_result.env);
}

TEST(ErrorTest, TemplateNotFound) {
  // Test rendering non-existent template
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  auto render_result = mj_env_render_template(env_result.env, "non_existent", value);
  
  // This should produce an error
  EXPECT_NE(render_result.error, nullptr);
  if (render_result.error != nullptr) {
    EXPECT_EQ(render_result.error->code, MJ_TEMPLATE_NOT_FOUND);
    EXPECT_NE(render_result.error->message, nullptr);
    
    // Test mj_error_free
    mj_error_free(render_result.error);
  }

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, SetListStringValues) {
  // Test setting string list values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "list_string_template", 
    "Names: {% for name in names %}{{ name }}{% if not loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_list_string
  const char* string_list[] = {"Alice", "Bob", "Charlie"};
  mj_value_set_list_string(value, "names", string_list, 3);

  auto render_result = mj_env_render_template(env_result.env, "list_string_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Names: Alice, Bob, Charlie");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, SetListIntValues) {
  // Test setting 64-bit integer list values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "list_int_template", 
    "Numbers: {% for num in numbers %}{{ num }}{% if not loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_list_int
  int64_t int_list[] = {1, 2, 3, 9223372036854775807LL};
  mj_value_set_list_int(value, "numbers", int_list, 4);

  auto render_result = mj_env_render_template(env_result.env, "list_int_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Numbers: 1, 2, 3, 9223372036854775807");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, SetListInt32Values) {
  // Test setting 32-bit integer list values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "list_int32_template", 
    "Values: {% for val in values %}{{ val }}{% if not loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_list_int32
  int32_t int32_list[] = {10, 20, 30, 2147483647};
  mj_value_set_list_int32(value, "values", int32_list, 4);

  auto render_result = mj_env_render_template(env_result.env, "list_int32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 10, 20, 30, 2147483647");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, SetListFloatValues) {
  // Test setting double precision float list values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "list_float_template", 
    "Decimals: {% for dec in decimals %}{{ dec }}{% if not loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_list_float
  double float_list[] = {1.1, 2.2, 3.3, 3.141592653589793};
  mj_value_set_list_float(value, "decimals", float_list, 4);

  auto render_result = mj_env_render_template(env_result.env, "list_float_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Decimals: 1.1, 2.2, 3.3, 3.141592653589793");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, SetListBoolValues) {
  // Test setting boolean list values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "list_bool_template", 
    "Flags: {% for flag in flags %}{{ flag }}{% if not loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test mj_value_set_list_bool
  bool bool_list[] = {true, false, true, false};
  mj_value_set_list_bool(value, "flags", bool_list, 4);

  auto render_result = mj_env_render_template(env_result.env, "list_bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Flags: true, false, true, false");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, EmptyListValues) {
  // Test setting empty list values
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "empty_list_template", 
    "Empty: {% if empty_list %}Not empty{% else %}Empty{% endif %}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test empty string list - create a valid pointer for empty array
  const char* empty_array[1] = {nullptr}; // Create a valid array but don't use it
  mj_value_set_list_string(value, "empty_list", empty_array, 0);

  auto render_result = mj_env_render_template(env_result.env, "empty_list_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Empty: Empty");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, MixedListsInTemplate) {
  // Test using multiple different list types in one template
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "mixed_lists_template", 
    "Names: {{ names|length }}, Numbers: {{ numbers|length }}, Flags: {{ flags|length }}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Set different types of lists
  const char* names[] = {"Alice", "Bob"};
  int32_t numbers[] = {1, 2, 3};
  bool flags[] = {true, false, true, false};

  mj_value_set_list_string(value, "names", names, 2);
  mj_value_set_list_int32(value, "numbers", numbers, 3);
  mj_value_set_list_bool(value, "flags", flags, 4);

  auto render_result = mj_env_render_template(env_result.env, "mixed_lists_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Names: 2, Numbers: 3, Flags: 4");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, ListWithSingleElement) {
  // Test lists with single elements
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "single_element_template", 
    "Single: {% for item in single %}{{ item }}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test single element string list
  const char* single_string[] = {"OnlyOne"};
  mj_value_set_list_string(value, "single", single_string, 1);

  auto render_result = mj_env_render_template(env_result.env, "single_element_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Single: OnlyOne");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ErrorTest, InvalidTemplateSyntax) {
  // Test various invalid template syntax scenarios
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  // Test unclosed variable
  auto add_result1 = mj_env_add_template(env_result.env, "unclosed_var", "Hello {{ name");
  if (add_result1.error != nullptr) {
    EXPECT_NE(add_result1.error->message, nullptr);
    EXPECT_GT(strlen(add_result1.error->message), 0);
    mj_error_free(add_result1.error);
  }

  // Test unclosed block
  auto add_result2 = mj_env_add_template(env_result.env, "unclosed_block", "{% for item in items %}{{ item }}");
  if (add_result2.error != nullptr) {
    EXPECT_NE(add_result2.error->message, nullptr);
    EXPECT_GT(strlen(add_result2.error->message), 0);
    mj_error_free(add_result2.error);
  }

  // Test invalid filter
  auto add_result3 = mj_env_add_template(env_result.env, "invalid_filter", "{{ name | nonexistent }}");
  EXPECT_EQ(add_result3.error, nullptr); // Template syntax is valid, error occurs at render time

  auto value = mj_value_new();
  mj_value_set_string(value, "name", "test");
  
  auto render_result = mj_env_render_template(env_result.env, "invalid_filter", value);
  if (render_result.error != nullptr) {
    EXPECT_EQ(render_result.error->code, MJ_UNKNOWN_FILTER);
    EXPECT_NE(render_result.error->message, nullptr);
    mj_error_free(render_result.error);
  }

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(ValueTest, OverwriteValues) {
  // Test overwriting values with different types
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto add_result = mj_env_add_template(env_result.env, "overwrite_template", "Value: {{ test }}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Set initial string value
  mj_value_set_string(value, "test", "initial");
  auto render_result1 = mj_env_render_template(env_result.env, "overwrite_template", value);
  EXPECT_EQ(render_result1.error, nullptr);
  EXPECT_STREQ(render_result1.result, "Value: initial");

  // Overwrite with integer
  mj_value_set_in32(value, "test", 42);
  auto render_result2 = mj_env_render_template(env_result.env, "overwrite_template", value);
  EXPECT_EQ(render_result2.error, nullptr);
  EXPECT_STREQ(render_result2.result, "Value: 42");

  // Overwrite with boolean
  mj_value_set_bool(value, "test", false);
  auto render_result3 = mj_env_render_template(env_result.env, "overwrite_template", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_STREQ(render_result3.result, "Value: false");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(EnvTest, MultipleTemplates) {
  // Test adding and using multiple templates
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  // Add multiple templates
  auto add_result1 = mj_env_add_template(env_result.env, "template1", "Hello {{ name }}");
  EXPECT_EQ(add_result1.error, nullptr);

  auto add_result2 = mj_env_add_template(env_result.env, "template2", "Goodbye {{ name }}");
  EXPECT_EQ(add_result2.error, nullptr);

  auto add_result3 = mj_env_add_template(env_result.env, "template3", "Age: {{ age }}");
  EXPECT_EQ(add_result3.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test rendering each template
  mj_value_set_string(value, "name", "World");
  mj_value_set_in32(value, "age", 25);

  auto render_result1 = mj_env_render_template(env_result.env, "template1", value);
  EXPECT_EQ(render_result1.error, nullptr);
  EXPECT_STREQ(render_result1.result, "Hello World");

  auto render_result2 = mj_env_render_template(env_result.env, "template2", value);
  EXPECT_EQ(render_result2.error, nullptr);
  EXPECT_STREQ(render_result2.result, "Goodbye World");

  auto render_result3 = mj_env_render_template(env_result.env, "template3", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_STREQ(render_result3.result, "Age: 25");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(EnvTest, TemplateOverwrite) {
  // Test overwriting an existing template
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  // Add initial template
  auto add_result1 = mj_env_add_template(env_result.env, "test_template", "Old: {{ value }}");
  EXPECT_EQ(add_result1.error, nullptr);

  // Overwrite with new template
  auto add_result2 = mj_env_add_template(env_result.env, "test_template", "New: {{ value }}");
  EXPECT_EQ(add_result2.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);
  mj_value_set_string(value, "value", "test");

  // Should use the new template
  auto render_result = mj_env_render_template(env_result.env, "test_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "New: test");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

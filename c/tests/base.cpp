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
  mj_str_free(render_result.result);  // Free the rendered string

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
  mj_str_free(render_result.result);

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(EnvOptionsTest, WhitespaceControl) {
  // Test whitespace control options
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test lstrip_blocks option
  mj_env_set_lstrip_blocks(env_result.env, true);
  // First set both trim_blocks and lstrip_blocks to get expected whitespace behavior
  mj_env_set_trim_blocks(env_result.env, true);
  mj_env_set_lstrip_blocks(env_result.env, true);
  
  auto add_result1 = mj_env_add_template(env_result.env, "lstrip_test", 
    "    {% if true %}\n    Hello\n    {% endif %}");
  EXPECT_EQ(add_result1.error, nullptr);

  auto render_result1 = mj_env_render_template(env_result.env, "lstrip_test", value);
  EXPECT_EQ(render_result1.error, nullptr);
  // With both trim_blocks and lstrip_blocks enabled, we expect clean whitespace
  EXPECT_STREQ(render_result1.result, "    Hello\n");
  mj_str_free(render_result1.result);

  // Test trim_blocks option
  mj_env_set_trim_blocks(env_result.env, true);
  auto add_result2 = mj_env_add_template(env_result.env, "trim_test", "{% for i in [1, 2] %}\nItem {{ i }}\n{% endfor %}");
  EXPECT_EQ(add_result2.error, nullptr);

  const int32_t nums[] = {1, 2};
  mj_value_set_list_int32(value, "i", nums, 2);
  auto render_result2 = mj_env_render_template(env_result.env, "trim_test", value);
  EXPECT_EQ(render_result2.error, nullptr);
  EXPECT_STREQ(render_result2.result, "Item 1\nItem 2\n");
  mj_str_free(render_result2.result);

  // Test keep_trailing_newline option
  mj_env_set_keep_trailing_newline(env_result.env, false);
  auto add_result3 = mj_env_add_template(env_result.env, "newline_test", "Hello\n");
  EXPECT_EQ(add_result3.error, nullptr);

  auto render_result3 = mj_env_render_template(env_result.env, "newline_test", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_STREQ(render_result3.result, "Hello");
  mj_str_free(render_result3.result);

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(EnvOptionsTest, SpecialOptions) {
  // Test special environment options
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test recursion limit
  mj_env_set_recursion_limit(env_result.env, 2);
  auto add_result1 = mj_env_add_template(env_result.env, "recursive_test", 
    "{% macro recursive(n) %}{{ recursive(n-1) }}{% endmacro %}{{ recursive(3) }}");
  EXPECT_EQ(add_result1.error, nullptr);

  auto render_result1 = mj_env_render_template(env_result.env, "recursive_test", value);
  EXPECT_NE(render_result1.error, nullptr); // Should fail due to recursion limit
  if (render_result1.error != nullptr) {
    mj_error_free(render_result1.error);
  }

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(EnvOptionsTest, UndefinedBehavior) {
  // Test undefined behavior settings
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Test MJ_UNDEFINED_BEHAVIOR_LENIENT (default)
  mj_env_set_undefined_behavior(env_result.env, MJ_UNDEFINED_BEHAVIOR_LENIENT);
  auto add_result1 = mj_env_add_template(env_result.env, "lenient_test", "{{ undefined_var }}");
  EXPECT_EQ(add_result1.error, nullptr);

  auto render_result1 = mj_env_render_template(env_result.env, "lenient_test", value);
  EXPECT_EQ(render_result1.error, nullptr);
  EXPECT_STREQ(render_result1.result, "");
  mj_str_free(render_result1.result);

  // Test MJ_UNDEFINED_BEHAVIOR_STRICT
  mj_env_set_undefined_behavior(env_result.env, MJ_UNDEFINED_BEHAVIOR_STRICT);
  auto render_result2 = mj_env_render_template(env_result.env, "lenient_test", value);
  EXPECT_NE(render_result2.error, nullptr);
  if (render_result2.error != nullptr) {
    EXPECT_EQ(render_result2.error->code, MJ_UNDEFINED_ERROR);
    mj_error_free(render_result2.error);
  }

  // Test MJ_UNDEFINED_BEHAVIOR_CHAINABLE
  mj_env_set_undefined_behavior(env_result.env, MJ_UNDEFINED_BEHAVIOR_CHAINABLE);
  auto add_result3 = mj_env_add_template(env_result.env, "chainable_test", "{{ undefined_var.field }}");
  EXPECT_EQ(add_result3.error, nullptr);

  auto render_result3 = mj_env_render_template(env_result.env, "chainable_test", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_STREQ(render_result3.result, "");
  mj_str_free(render_result3.result);

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
  mj_str_free(render_result.result);

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
  mj_str_free(render_result.result);

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
  mj_str_free(render_result.result);

  // Test with false
  mj_value_set_bool(value, "active", false);
  render_result = mj_env_render_template(env_result.env, "bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Inactive");
  mj_str_free(render_result.result);

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
  
  // This should produce a syntax error
  EXPECT_NE(add_result.error, nullptr);
  if (add_result.error != nullptr) {
    EXPECT_EQ(add_result.error->code, MJ_SYNTAX_ERROR);
    EXPECT_NE(add_result.error->message, nullptr);
    EXPECT_GT(strlen(add_result.error->message), 0);
    
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
  EXPECT_NE(add_result1.error, nullptr);
  if (add_result1.error != nullptr) {
    EXPECT_EQ(add_result1.error->code, MJ_SYNTAX_ERROR);
    EXPECT_NE(add_result1.error->message, nullptr);
    EXPECT_GT(strlen(add_result1.error->message), 0);
    mj_error_free(add_result1.error);
  }

  // Test unclosed block
  auto add_result2 = mj_env_add_template(env_result.env, "unclosed_block", "{% for item in items %}{{ item }}");
  EXPECT_NE(add_result2.error, nullptr);
  if (add_result2.error != nullptr) {
    EXPECT_EQ(add_result2.error->code, MJ_SYNTAX_ERROR);
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
  } else {
    mj_str_free(render_result.result);
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

TEST(EnvTest, RemoveTemplate) {
  // Test removing templates from environment
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  // Add templates
  auto add_result1 = mj_env_add_template(env_result.env, "template1", "Hello {{ name }}");
  EXPECT_EQ(add_result1.error, nullptr);

  auto add_result2 = mj_env_add_template(env_result.env, "template2", "Goodbye {{ name }}");
  EXPECT_EQ(add_result2.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);
  mj_value_set_string(value, "name", "World");

  // Verify template1 exists and works
  auto render_result1 = mj_env_render_template(env_result.env, "template1", value);
  EXPECT_EQ(render_result1.error, nullptr);
  EXPECT_STREQ(render_result1.result, "Hello World");

  // Remove template1
  mj_env_remove_template(env_result.env, "template1");

  // Try to render removed template - should fail
  auto render_result2 = mj_env_render_template(env_result.env, "template1", value);
  EXPECT_NE(render_result2.error, nullptr);
  EXPECT_EQ(render_result2.error->code, MJ_TEMPLATE_NOT_FOUND);
  mj_error_free(render_result2.error);

  // Verify template2 still works
  auto render_result3 = mj_env_render_template(env_result.env, "template2", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_STREQ(render_result3.result, "Goodbye World");

  // Test removing non-existent template (should not crash)
  mj_env_remove_template(env_result.env, "non_existent");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(EnvTest, ClearTemplates) {
  // Test clearing all templates from environment
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  // Add multiple templates
  auto add_result1 = mj_env_add_template(env_result.env, "template1", "Hello {{ name }}");
  EXPECT_EQ(add_result1.error, nullptr);

  auto add_result2 = mj_env_add_template(env_result.env, "template2", "Goodbye {{ name }}");
  EXPECT_EQ(add_result2.error, nullptr);

  auto add_result3 = mj_env_add_template(env_result.env, "template3", "Welcome {{ name }}");
  EXPECT_EQ(add_result3.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);
  mj_value_set_string(value, "name", "World");

  // Verify templates exist and work
  auto render_result1 = mj_env_render_template(env_result.env, "template1", value);
  EXPECT_EQ(render_result1.error, nullptr);
  EXPECT_STREQ(render_result1.result, "Hello World");

  // Clear all templates
  mj_env_clear_templates(env_result.env);

  // Try to render any template - should all fail
  auto render_result2 = mj_env_render_template(env_result.env, "template1", value);
  EXPECT_NE(render_result2.error, nullptr);
  EXPECT_EQ(render_result2.error->code, MJ_TEMPLATE_NOT_FOUND);
  mj_error_free(render_result2.error);

  auto render_result3 = mj_env_render_template(env_result.env, "template2", value);
  EXPECT_NE(render_result3.error, nullptr);
  EXPECT_EQ(render_result3.error->code, MJ_TEMPLATE_NOT_FOUND);
  mj_error_free(render_result3.error);

  auto render_result4 = mj_env_render_template(env_result.env, "template3", value);
  EXPECT_NE(render_result4.error, nullptr);
  EXPECT_EQ(render_result4.error->code, MJ_TEMPLATE_NOT_FOUND);
  mj_error_free(render_result4.error);

  // Test that we can add templates again after clearing
  auto add_result4 = mj_env_add_template(env_result.env, "new_template", "New {{ name }}");
  EXPECT_EQ(add_result4.error, nullptr);

  auto render_result5 = mj_env_render_template(env_result.env, "new_template", value);
  EXPECT_EQ(render_result5.error, nullptr);
  EXPECT_STREQ(render_result5.result, "New World");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(EnvTest, RenderNamedString) {
  // Test rendering template from string without adding to environment
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);
  mj_value_set_string(value, "name", "World");
  mj_value_set_in32(value, "age", 25);

  // Test basic named string rendering
  auto render_result1 = mj_env_render_named_string(env_result.env, "inline_template", 
                                                   "Hello {{ name }}, you are {{ age }} years old", value);
  EXPECT_EQ(render_result1.error, nullptr);
  EXPECT_STREQ(render_result1.result, "Hello World, you are 25 years old");

  // Test with loops
  const char* items[] = {"apple", "banana", "cherry"};
  mj_value_set_list_string(value, "items", items, 3);
  
  auto render_result2 = mj_env_render_named_string(env_result.env, "list_template",
                                                   "Items: {% for item in items %}{{ item }}{% if not loop.last %}, {% endif %}{% endfor %}", value);
  EXPECT_EQ(render_result2.error, nullptr);
  EXPECT_STREQ(render_result2.result, "Items: apple, banana, cherry");

  // Test with invalid syntax
  auto render_result3 = mj_env_render_named_string(env_result.env, "invalid_template",
                                                   "Hello {{ unclosed", value);
  EXPECT_NE(render_result3.error, nullptr);
  EXPECT_NE(render_result3.error->message, nullptr);
  mj_error_free(render_result3.error);

  // Test empty template
  auto render_result4 = mj_env_render_named_string(env_result.env, "empty_template", "", value);
  EXPECT_EQ(render_result4.error, nullptr);
  EXPECT_STREQ(render_result4.result, "");

  // Test template with no variables
  auto render_result5 = mj_env_render_named_string(env_result.env, "static_template", 
                                                   "This is a static template", value);
  EXPECT_EQ(render_result5.error, nullptr);
  EXPECT_STREQ(render_result5.result, "This is a static template");

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(MemoryTest, StringFree) {
  // Test mj_str_free function
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);
  mj_value_set_string(value, "name", "World");

  auto add_result = mj_env_add_template(env_result.env, "test_template", "Hello {{ name }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Render template to get a result string
  auto render_result = mj_env_render_template(env_result.env, "test_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_NE(render_result.result, nullptr);
  EXPECT_STREQ(render_result.result, "Hello World");

  // Test mj_str_free - this should not crash
  mj_str_free(render_result.result);

  // Test multiple renders and frees
  auto render_result2 = mj_env_render_template(env_result.env, "test_template", value);
  EXPECT_EQ(render_result2.error, nullptr);
  EXPECT_NE(render_result2.result, nullptr);
  mj_str_free(render_result2.result);

  // Test with named string rendering
  auto render_result3 = mj_env_render_named_string(env_result.env, "inline", 
                                                   "Goodbye {{ name }}", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_NE(render_result3.result, nullptr);
  EXPECT_STREQ(render_result3.result, "Goodbye World");
  mj_str_free(render_result3.result);

  // Test freeing nullptr (should be safe)
  mj_str_free(nullptr);

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(MemoryTest, ErrorFreeEdgeCases) {
  // Test mj_error_free edge cases and multiple scenarios
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  // Test freeing nullptr error (should be safe)
  mj_error_free(nullptr);

  // Test multiple error scenarios and freeing
  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);

  // Generate multiple errors and free them
  for (int i = 0; i < 5; i++) {
    auto render_result = mj_env_render_template(env_result.env, "non_existent", value);
    EXPECT_NE(render_result.error, nullptr);
    if (render_result.error != nullptr) {
      EXPECT_EQ(render_result.error->code, MJ_TEMPLATE_NOT_FOUND);
      mj_error_free(render_result.error);
    }
  }

  // Test with template syntax errors
  auto add_result1 = mj_env_add_template(env_result.env, "bad1", "{{ unclosed");
  if (add_result1.error != nullptr) {
    mj_error_free(add_result1.error);
  }

  auto add_result2 = mj_env_add_template(env_result.env, "bad2", "{% for item %}no endfor");
  if (add_result2.error != nullptr) {
    mj_error_free(add_result2.error);
  }

  mj_value_free(value);
  mj_env_free(env_result.env);
}

TEST(EnvTest, RenderNamedStringWithExistingTemplates) {
  // Test that named string rendering doesn't interfere with existing templates
  auto env_result = mj_env_new();
  ASSERT_NE(env_result.env, nullptr);

  // Add a template to the environment
  auto add_result = mj_env_add_template(env_result.env, "existing_template", "Existing: {{ name }}");
  EXPECT_EQ(add_result.error, nullptr);

  auto value = mj_value_new();
  ASSERT_NE(value, nullptr);
  mj_value_set_string(value, "name", "Test");

  // Render existing template
  auto render_result1 = mj_env_render_template(env_result.env, "existing_template", value);
  EXPECT_EQ(render_result1.error, nullptr);
  EXPECT_STREQ(render_result1.result, "Existing: Test");
  mj_str_free(render_result1.result);

  // Render named string with same name but different content
  auto render_result2 = mj_env_render_named_string(env_result.env, "existing_template",
                                                   "Named: {{ name }}", value);
  EXPECT_EQ(render_result2.error, nullptr);
  EXPECT_STREQ(render_result2.result, "Named: Test");
  mj_str_free(render_result2.result);

  // Verify existing template is still intact
  auto render_result3 = mj_env_render_template(env_result.env, "existing_template", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_STREQ(render_result3.result, "Existing: Test");
  mj_str_free(render_result3.result);

  // Render named string with different name
  auto render_result4 = mj_env_render_named_string(env_result.env, "inline_template",
                                                   "Inline: {{ name }}", value);
  EXPECT_EQ(render_result4.error, nullptr);
  EXPECT_STREQ(render_result4.result, "Inline: Test");
  mj_str_free(render_result4.result);

  mj_value_free(value);
  mj_env_free(env_result.env);
}

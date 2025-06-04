#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, SetIntegerValues) {
  // Test setting 64-bit integer values
  auto add_result =
      mj_env_add_template(env, "int_template", "Number: {{ count }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_int (64-bit integer)
  mj_value_set_int(value, "count", 9223372036854775807LL); // Max int64_t

  auto render_result = mj_env_render_template(env, "int_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Number: 9223372036854775807");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetInt32Values) {
  // Test setting 32-bit integer values
  auto add_result =
      mj_env_add_template(env, "int32_template", "Value: {{ num }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_in32 (32-bit integer)
  mj_value_set_in32(value, "num", 2147483647); // Max int32_t

  auto render_result = mj_env_render_template(env, "int32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Value: 2147483647");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetFloatValues) {
  // Test setting double precision float values
  auto add_result = mj_env_add_template(env, "float_template", "Pi: {{ pi }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_float (double)
  mj_value_set_float(value, "pi", 3.141592653589793);

  auto render_result = mj_env_render_template(env, "float_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Pi: 3.141592653589793");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetFloat32Values) {
  // Test setting single precision float values
  auto add_result =
      mj_env_add_template(env, "float32_template", "Value: {{ val }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_float32 (float)
  mj_value_set_float32(value, "val", 3.14159f);

  auto render_result = mj_env_render_template(env, "float32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  // Note: Float precision might vary, so we check if it starts with expected
  // value
  EXPECT_TRUE(strncmp(render_result.result, "Value: 3.14159", 14) == 0);
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetBoolValues) {
  // Test setting boolean values
  auto add_result =
      mj_env_add_template(env, "bool_template",
                          "{% if active %}Active{% else %}Inactive{% endif %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_bool with true
  mj_value_set_bool(value, "active", true);

  auto render_result = mj_env_render_template(env, "bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Active");
  mj_str_free(render_result.result);

  // Test with false
  mj_value_set_bool(value, "active", false);
  render_result = mj_env_render_template(env, "bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Inactive");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetStringValues) {
  // Test setting string values
  auto add_result =
      mj_env_add_template(env, "string_template", "Hello, {{ name }}!");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_string
  mj_value_set_string(value, "name", "World");

  auto render_result = mj_env_render_template(env, "string_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Hello, World!");
  mj_str_free(render_result.result);

  // Test with empty string
  mj_value_set_string(value, "name", "");
  render_result = mj_env_render_template(env, "string_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Hello, !");
  mj_str_free(render_result.result);

  // Test with special characters
  mj_value_set_string(value, "name", "Alice & Bob");
  render_result = mj_env_render_template(env, "string_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Hello, Alice & Bob!");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetNestedValues) {
  // Test setting nested value objects
  auto add_result = mj_env_add_template(
      env, "nested_template",
      "User: {{ user.name }}, Age: {{ user.age }}, Active: {{ user.active }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Create nested value object
  auto nested_value = mj_value_new();
  mj_value_set_string(nested_value, "name", "Alice");
  mj_value_set_int(nested_value, "age", 30);
  mj_value_set_bool(nested_value, "active", true);

  // Test mj_value_set_value
  mj_value_set_value(value, "user", nested_value);

  auto render_result = mj_env_render_template(env, "nested_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "User: Alice, Age: 30, Active: true");
  mj_str_free(render_result.result);

  mj_value_free(nested_value);
}

TEST_F(MiniJinjaTest, SetMultipleFieldTypes) {
  // Test setting multiple field types in same value object
  auto add_result = mj_env_add_template(
      env, "multi_type_template",
      "{{ name }}: {{ age }} years, Score: {{ score }}, Active: {{ active }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Set different types of values
  mj_value_set_string(value, "name", "John");
  mj_value_set_int(value, "age", 25);
  mj_value_set_float(value, "score", 95.5);
  mj_value_set_bool(value, "active", true);

  auto render_result =
      mj_env_render_template(env, "multi_type_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result,
               "John: 25 years, Score: 95.5, Active: true");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetSpecialStringValues) {
  // Test setting strings with special characters and encoding
  auto add_result =
      mj_env_add_template(env, "special_string_template", "Message: {{ msg }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test with newlines and special characters
  mj_value_set_string(value, "msg", "Line 1\nLine 2\tTabbed");

  auto render_result =
      mj_env_render_template(env, "special_string_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Message: Line 1\nLine 2\tTabbed");
  mj_str_free(render_result.result);

  // Test with quotes
  mj_value_set_string(value, "msg", "He said \"Hello\" and 'Goodbye'");
  render_result = mj_env_render_template(env, "special_string_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result,
               "Message: He said \"Hello\" and 'Goodbye'");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetDeepNestedValues) {
  // Test setting deeply nested value objects
  auto add_result = mj_env_add_template(
      env, "deep_nested_template",
      "{{ person.address.city }}, {{ person.address.country }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Create nested structure: person -> address -> city/country
  auto address = mj_value_new();
  mj_value_set_string(address, "city", "New York");
  mj_value_set_string(address, "country", "USA");

  auto person = mj_value_new();
  mj_value_set_value(person, "address", address);

  mj_value_set_value(value, "person", person);

  auto render_result =
      mj_env_render_template(env, "deep_nested_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "New York, USA");
  mj_str_free(render_result.result);

  mj_value_free(address);
  mj_value_free(person);
}

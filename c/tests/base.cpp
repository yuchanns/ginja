#include "minijinja.h"
#include <gtest/gtest.h>
#include <cstring>
#include <string.h>

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

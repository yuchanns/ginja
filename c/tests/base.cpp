#include "minijinja.h"
#include <gtest/gtest.h>

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

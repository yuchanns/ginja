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

  // Test freeing the environment
  mj_env_free(result.env);
  // Note: Since we've freed the memory, we shouldn't access result.env anymore
}

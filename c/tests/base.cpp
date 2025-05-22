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

  // Test freeing the environment
  mj_env_free(result.env);
  // Note: Since we've freed the memory, we shouldn't access result.env anymore
}

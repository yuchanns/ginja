#pragma once

#include "minijinja.h"
#include <gtest/gtest.h>
#include <stdint.h>
#include <string.h>

class MiniJinjaTest : public ::testing::Test {
protected:
  struct mj_result_env_new env_result;
  mj_env *env;
  mj_value *value;

  void SetUp() override {
    // Set up code that will be called before each test
    env_result = mj_env_new();
    ASSERT_NE(env_result.env, nullptr)
        << "Environment should not be null after creation";
    env = env_result.env;

    // Ensure we can access the environment
    EXPECT_NE(env->inner, nullptr)
        << "Environment inner pointer should not be null";

    value = mj_value_new();
    ASSERT_NE(value, nullptr)
        << "Value pointer should not be null after creation";

    // Ensure we can access the value
    EXPECT_NE(value->inner, nullptr) << "Value inner pointer should not be null";
  }

  void TearDown() override {
    // Clean up code that will be called after each test
    if (value) {
      mj_value_free(value);
    }
    if (env) {
      mj_env_free(env);
    }
  }
};


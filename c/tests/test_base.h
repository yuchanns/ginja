#pragma once

#include "minijinja.h"
#include <gtest/gtest.h>
#include <stdint.h>
#include <string.h>

class MiniJinjaTest : public ::testing::Test {
protected:
  mj_env *env;

  void SetUp() override {
    // Set up code that will be called before each test
    env = mj_env_new();

    // Ensure we can access the environment
    EXPECT_NE(env->inner, nullptr)
        << "Environment inner pointer should not be null";
  }

  void TearDown() override {
    // Clean up code that will be called after each test
    if (env) {
      mj_env_free(env);
    }
  }

  // Helper function to create JSON data
  std::string createJsonData(const std::string& json) {
    return json;
  }

  // Helper function to render template with JSON data
  struct mj_result_env_render_template* renderTemplate(const char* name, const std::string& json_data) {
    return mj_env_render(env, name, reinterpret_cast<const uint8_t*>(json_data.c_str()), json_data.length());
  }

  // Helper function to render named string template with JSON data
  struct mj_result_env_render_template* renderNamedString(const char* name, const char* source, const std::string& json_data) {
    return mj_env_render_named_string(env, name, source, reinterpret_cast<const uint8_t*>(json_data.c_str()), json_data.length());
  }
};


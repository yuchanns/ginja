#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, SetIntegerValues) {
  // Test setting 64-bit integer values
  auto add_result = mj_env_add_template(env, "int_template",
                                      "Number: {{ count }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_int (64-bit integer)
  mj_value_set_int(value, "count", 9223372036854775807LL); // Max int64_t

  auto render_result =
      mj_env_render_template(env, "int_template", value);
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

  auto render_result =
      mj_env_render_template(env, "int32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Value: 2147483647");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetFloatValues) {
  // Test setting double precision float values
  auto add_result =
      mj_env_add_template(env, "float_template", "Pi: {{ pi }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_float (double)
  mj_value_set_float(value, "pi", 3.141592653589793);

  auto render_result =
      mj_env_render_template(env, "float_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Pi: 3.141592653589793");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetFloat32Values) {
  // Test setting single precision float values
  auto add_result = mj_env_add_template(env, "float32_template",
                                      "Value: {{ val }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_float32 (float)
  mj_value_set_float32(value, "val", 3.14159f);

  auto render_result =
      mj_env_render_template(env, "float32_template", value);
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

  auto render_result =
      mj_env_render_template(env, "bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Active");
  mj_str_free(render_result.result);

  // Test with false
  mj_value_set_bool(value, "active", false);
  render_result =
      mj_env_render_template(env, "bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Inactive");
  mj_str_free(render_result.result);
}


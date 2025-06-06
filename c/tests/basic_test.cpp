#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, MultipleValuesInTemplate) {
  // Test using multiple different value types in one template
  auto add_result =
      mj_env_add_template(env, "multi_template",
                        "Name: {{ name }}, Age: {{ age }}, Score: {{ score "
                        "}}, Active: {{ active }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Set different types of values
  mj_value_set_string(value, "name", "Alice");
  mj_value_set_int32(value, "age", 25);
  mj_value_set_float32(value, "score", 95.5f);
  mj_value_set_bool(value, "active", true);

  auto render_result =
      mj_env_render_template(env, "multi_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result,
             "Name: Alice, Age: 25, Score: 95.5, Active: true");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, OverwriteValues) {
  // Test overwriting values with different types
  auto add_result =
      mj_env_add_template(env, "test_value", "Value: {{ val }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Set string value
  mj_value_set_string(value, "val", "Hello");
  auto render_result1 =
      mj_env_render_template(env, "test_value", value);
  EXPECT_EQ(render_result1.error, nullptr);
  EXPECT_STREQ(render_result1.result, "Value: Hello");
  mj_str_free(render_result1.result);

  // Overwrite with integer
  mj_value_set_int(value, "val", 42);
  auto render_result2 =
      mj_env_render_template(env, "test_value", value);
  EXPECT_EQ(render_result2.error, nullptr);
  EXPECT_STREQ(render_result2.result, "Value: 42");
  mj_str_free(render_result2.result);

  // Overwrite with float
  mj_value_set_float(value, "val", 3.14);
  auto render_result3 =
      mj_env_render_template(env, "test_value", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_STREQ(render_result3.result, "Value: 3.14");
  mj_str_free(render_result3.result);

  // Overwrite with boolean
  mj_value_set_bool(value, "val", true);
  auto render_result4 =
      mj_env_render_template(env, "test_value", value);
  EXPECT_EQ(render_result4.error, nullptr);
  EXPECT_STREQ(render_result4.result, "Value: true");
  mj_str_free(render_result4.result);
}


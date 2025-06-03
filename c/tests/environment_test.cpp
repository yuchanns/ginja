#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, WhitespaceControl) {
  // Test whitespace control options
  // Test lstrip_blocks option
  mj_env_set_lstrip_blocks(env, true);
  // First set both trim_blocks and lstrip_blocks to get expected whitespace
  // behavior
  mj_env_set_trim_blocks(env, true);
  mj_env_set_lstrip_blocks(env, true);

  auto add_result1 =
      mj_env_add_template(env, "lstrip_test",
                          "{% if true %}\n    Hello\n    {% endif %}");
  EXPECT_EQ(add_result1.error, nullptr);

  auto render_result1 =
      mj_env_render_template(env, "lstrip_test", value);
  EXPECT_EQ(render_result1.error, nullptr);
  // With both trim_blocks and lstrip_blocks enabled, we expect clean whitespace
  EXPECT_STREQ(render_result1.result, "    Hello\n");
  mj_str_free(render_result1.result);

  // Test trim_blocks option
  mj_env_set_trim_blocks(env, true);
  auto add_result2 =
      mj_env_add_template(env, "trim_test",
                          "{% for i in [1, 2] %}\nItem {{ i }}\n{% endfor %}");
  EXPECT_EQ(add_result2.error, nullptr);

  const int32_t nums[] = {1, 2};
  mj_value_set_list_int32(value, "i", nums, 2);
  auto render_result2 =
      mj_env_render_template(env, "trim_test", value);
  EXPECT_EQ(render_result2.error, nullptr);
  EXPECT_STREQ(render_result2.result, "Item 1\nItem 2\n");
  mj_str_free(render_result2.result);

  // Test keep_trailing_newline option
  mj_env_set_keep_trailing_newline(env, false);
  auto add_result3 =
      mj_env_add_template(env, "newline_test", "Hello\n");
  EXPECT_EQ(add_result3.error, nullptr);

  auto render_result3 =
      mj_env_render_template(env, "newline_test", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_STREQ(render_result3.result, "Hello");
  mj_str_free(render_result3.result);
}

TEST_F(MiniJinjaTest, SpecialOptions) {
  // Test special environment options
  // Test recursion limit
  mj_env_set_recursion_limit(env, 2);
  auto add_result1 =
      mj_env_add_template(env, "recursive_test",
                          "{% macro recursive(n) %}{{ recursive(n-1) }}{% "
                          "endmacro %}{{ recursive(3) }}");
  EXPECT_EQ(add_result1.error, nullptr);

  auto render_result1 =
      mj_env_render_template(env, "recursive_test", value);
  EXPECT_NE(render_result1.error,
            nullptr); // Should fail due to recursion limit
  if (render_result1.error != nullptr) {
    mj_error_free(render_result1.error);
  }
}

TEST_F(MiniJinjaTest, UndefinedBehavior) {
  // Test undefined behavior settings
  // Test MJ_UNDEFINED_BEHAVIOR_LENIENT (default)
  mj_env_set_undefined_behavior(env, MJ_UNDEFINED_BEHAVIOR_LENIENT);
  auto add_result1 = mj_env_add_template(env, "lenient_test",
                                         "{{ undefined_var }}");
  EXPECT_EQ(add_result1.error, nullptr);

  auto render_result1 =
      mj_env_render_template(env, "lenient_test", value);
  EXPECT_EQ(render_result1.error, nullptr);
  EXPECT_STREQ(render_result1.result, "");
  mj_str_free(render_result1.result);

  // Test MJ_UNDEFINED_BEHAVIOR_STRICT
  mj_env_set_undefined_behavior(env, MJ_UNDEFINED_BEHAVIOR_STRICT);
  auto render_result2 =
      mj_env_render_template(env, "lenient_test", value);
  EXPECT_NE(render_result2.error, nullptr);
  if (render_result2.error != nullptr) {
    EXPECT_EQ(render_result2.error->code, MJ_UNDEFINED_ERROR);
    mj_error_free(render_result2.error);
  }

  // Test MJ_UNDEFINED_BEHAVIOR_CHAINABLE
  mj_env_set_undefined_behavior(env,
                                MJ_UNDEFINED_BEHAVIOR_CHAINABLE);
  auto add_result3 = mj_env_add_template(env, "chainable_test",
                                         "{{ undefined_var.field }}");
  EXPECT_EQ(add_result3.error, nullptr);

  auto render_result3 =
      mj_env_render_template(env, "chainable_test", value);
  EXPECT_EQ(render_result3.error, nullptr);
  EXPECT_STREQ(render_result3.result, "");
  mj_str_free(render_result3.result);
}


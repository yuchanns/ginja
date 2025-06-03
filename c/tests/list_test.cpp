#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, SetListStringValues) {
  // Test setting string list values
  auto add_result =
      mj_env_add_template(env, "list_string_template",
                          "Names: {% for name in names %}{{ name }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_list_string
  const char *string_list[] = {"Alice", "Bob", "Charlie"};
  mj_value_set_list_string(value, "names", string_list, 3);

  auto render_result =
      mj_env_render_template(env, "list_string_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Names: Alice, Bob, Charlie");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListIntValues) {
  // Test setting 64-bit integer list values
  auto add_result =
      mj_env_add_template(env, "list_int_template",
                          "Numbers: {% for num in numbers %}{{ num }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_list_int
  int64_t int_list[] = {1, 2, 3, 9223372036854775807LL};
  mj_value_set_list_int(value, "numbers", int_list, 4);

  auto render_result =
      mj_env_render_template(env, "list_int_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Numbers: 1, 2, 3, 9223372036854775807");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListInt32Values) {
  // Test setting 32-bit integer list values
  auto add_result =
      mj_env_add_template(env, "list_int32_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_list_int32
  int32_t int32_list[] = {10, 20, 30, 2147483647};
  mj_value_set_list_int32(value, "values", int32_list, 4);

  auto render_result =
      mj_env_render_template(env, "list_int32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 10, 20, 30, 2147483647");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListFloatValues) {
  // Test setting double precision float list values
  auto add_result =
      mj_env_add_template(env, "list_float_template",
                          "Decimals: {% for dec in decimals %}{{ dec }}{% if "
                          "not loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_list_float
  double float_list[] = {1.1, 2.2, 3.3, 3.141592653589793};
  mj_value_set_list_float(value, "decimals", float_list, 4);

  auto render_result =
      mj_env_render_template(env, "list_float_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result,
               "Decimals: 1.1, 2.2, 3.3, 3.141592653589793");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListBoolValues) {
  // Test setting boolean list values
  auto add_result =
      mj_env_add_template(env, "list_bool_template",
                          "Flags: {% for flag in flags %}{{ flag }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_list_bool
  bool bool_list[] = {true, false, true, false};
  mj_value_set_list_bool(value, "flags", bool_list, 4);

  auto render_result =
      mj_env_render_template(env, "list_bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Flags: true, false, true, false");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, EmptyListValues) {
  // Test setting empty list values
  auto add_result = mj_env_add_template(
      env, "empty_list_template",
      "Empty: {% if empty_list %}Not empty{% else %}Empty{% endif %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test empty string list - create a valid pointer for empty array
  const char *empty_array[1] = {
      nullptr}; // Create a valid array but don't use it
  mj_value_set_list_string(value, "empty_list", empty_array, 0);

  auto render_result =
      mj_env_render_template(env, "empty_list_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Empty: Empty");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, MixedListsInTemplate) {
  // Test using multiple different list types in one template
  auto add_result =
      mj_env_add_template(env, "mixed_lists_template",
                          "Names: {{ names|length }}, Numbers: {{ "
                          "numbers|length }}, Flags: {{ flags|length }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Set different types of lists
  const char *names[] = {"Alice", "Bob"};
  int32_t numbers[] = {1, 2, 3};
  bool flags[] = {true, false, true, false};

  mj_value_set_list_string(value, "names", names, 2);
  mj_value_set_list_int32(value, "numbers", numbers, 3);
  mj_value_set_list_bool(value, "flags", flags, 4);

  auto render_result =
      mj_env_render_template(env, "mixed_lists_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Names: 2, Numbers: 3, Flags: 4");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, ListWithSingleElement) {
  // Test lists with single elements
  auto add_result = mj_env_add_template(
      env, "single_element_template",
      "Single: {% for item in single %}{{ item }}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test single element string list
  const char *single_string[] = {"OnlyOne"};
  mj_value_set_list_string(value, "single", single_string, 1);

  auto render_result =
      mj_env_render_template(env, "single_element_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Single: OnlyOne");
  mj_str_free(render_result.result);
}


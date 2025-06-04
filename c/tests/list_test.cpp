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

  auto render_result = mj_env_render_template(env, "list_int_template", value);
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

  auto render_result = mj_env_render_template(env, "list_bool_template", value);
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

TEST_F(MiniJinjaTest, SetListFloat32Values) {
  // Test setting single precision float list values
  auto add_result =
      mj_env_add_template(env, "list_float32_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test mj_value_set_list_float32
  float float32_list[] = {1.0f, 2.0f, 3.0f,
                          4.5f}; // These are exactly representable
  mj_value_set_list_float32(value, "values", float32_list, 4);

  auto render_result =
      mj_env_render_template(env, "list_float32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  // Use exact values that can be precisely represented in float32
  EXPECT_STREQ(render_result.result, "Values: 1.0, 2.0, 3.0, 4.5");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetLargeListValues) {
  // Test setting large list values
  auto add_result =
      mj_env_add_template(env, "large_list_template",
                          "Count: {{ numbers|length }}, First: {{ numbers[0] "
                          "}}, Last: {{ numbers[-1] }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Create large array
  const int LIST_SIZE = 1000;
  int64_t *large_list = new int64_t[LIST_SIZE];
  for (int i = 0; i < LIST_SIZE; i++) {
    large_list[i] = i + 1;
  }

  mj_value_set_list_int(value, "numbers", large_list, LIST_SIZE);

  auto render_result =
      mj_env_render_template(env, "large_list_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Count: 1000, First: 1, Last: 1000");
  mj_str_free(render_result.result);

  delete[] large_list;
}

TEST_F(MiniJinjaTest, SetListWithNegativeValues) {
  // Test setting list with negative values
  auto add_result =
      mj_env_add_template(env, "negative_list_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test negative integers
  int64_t negative_ints[] = {-100, -50, 0, 50, 100};
  mj_value_set_list_int(value, "values", negative_ints, 5);

  auto render_result =
      mj_env_render_template(env, "negative_list_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: -100, -50, 0, 50, 100");
  mj_str_free(render_result.result);

  // Test negative floats
  double negative_floats[] = {-3.14, -2.71, 0.0, 2.71, 3.14};
  mj_value_set_list_float(value, "values", negative_floats, 5);

  render_result = mj_env_render_template(env, "negative_list_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: -3.14, -2.71, 0.0, 2.71, 3.14");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListWithSpecialStringValues) {
  // Test setting list with strings containing special characters
  auto add_result =
      mj_env_add_template(env, "special_string_list_template",
                          "Items: {% for item in items %}'{{ item }}'{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test strings with special characters
  const char *special_strings[] = {
      "Hello\nWorld", "Tab\tSeparated", "Quote\"Test", "Apostrophe's Test",
      "" // Empty string
  };
  mj_value_set_list_string(value, "items", special_strings, 5);

  auto render_result =
      mj_env_render_template(env, "special_string_list_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Items: 'Hello\nWorld', 'Tab\tSeparated', "
                                     "'Quote\"Test', 'Apostrophe's Test', ''");
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListWithMaxMinValues) {
  // Test setting list with maximum and minimum values
  auto add_result =
      mj_env_add_template(env, "extremes_template",
                          "Int64: {{ int64_vals[0] }}, {{ int64_vals[1] }}; "
                          "Int32: {{ int32_vals[0] }}, {{ int32_vals[1] }}");
  EXPECT_EQ(add_result.error, nullptr);

  // Test with max and min int64 values
  int64_t int64_extremes[] = {INT64_MAX, INT64_MIN};
  mj_value_set_list_int(value, "int64_vals", int64_extremes, 2);

  // Test with max and min int32 values
  int32_t int32_extremes[] = {INT32_MAX, INT32_MIN};
  mj_value_set_list_int32(value, "int32_vals", int32_extremes, 2);

  auto render_result = mj_env_render_template(env, "extremes_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  // Check that the values are rendered (exact string comparison may be platform
  // dependent)
  EXPECT_TRUE(strstr(render_result.result, "9223372036854775807") != nullptr);
  EXPECT_TRUE(strstr(render_result.result, "2147483647") != nullptr);
  mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListWithNestedValues) {
  // Test setting list with nested value objects
  auto add_result =
      mj_env_add_template(env, "nested_list_template",
                          "Users: {% for user in users %}{{ user.name }} (Age: "
                          "{{ user.age }}){% if "
                          "not loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Create nested value objects
  auto user1 = mj_value_new();
  mj_value_set_string(user1, "name", "Alice");
  mj_value_set_int(user1, "age", 30);

  auto user2 = mj_value_new();
  mj_value_set_string(user2, "name", "Bob");
  mj_value_set_int(user2, "age", 25);

  const mj_value *values[] = {user1, user2};
  // Set list of nested values
  mj_value_set_list_value(value, "users", values, 2);

  auto render_result =
      mj_env_render_template(env, "nested_list_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Users: Alice (Age: 30), Bob (Age: 25)");
  mj_str_free(render_result.result);

  // Free nested values
  mj_value_free(user1);
  mj_value_free(user2);
}

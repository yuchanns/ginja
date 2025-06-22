#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, AppendStringValues) {
  // Test appending string values to a list
  auto list = mj_value_new_list();

  // Append string values
  mj_value_append_string(list, "Hello");
  mj_value_append_string(list, "World");
  mj_value_append_string(list, "!");

  auto add_result =
      mj_env_add_template(env, "append_string_template",
                          "Result: {% for item in items %}{{ item }}{% if not "
                          "loop.last %} {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "items", list);

  auto render_result =
      mj_env_render_template(env, "append_string_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Result: Hello World !");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendIntValues) {
  // Test appending 64-bit integer values to a list
  auto list = mj_value_new_list();

  // Append integer values
  mj_value_append_int(list, 1);
  mj_value_append_int(list, 42);
  mj_value_append_int(list, 9223372036854775807LL); // Max int64_t

  auto add_result =
      mj_env_add_template(env, "append_int_template",
                          "Numbers: {% for num in numbers %}{{ num }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "numbers", list);

  auto render_result =
      mj_env_render_template(env, "append_int_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Numbers: 1, 42, 9223372036854775807");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendInt32Values) {
  // Test appending 32-bit integer values to a list
  auto list = mj_value_new_list();

  // Append int32 values
  mj_value_append_int32(list, 10);
  mj_value_append_int32(list, -20);
  mj_value_append_int32(list, 2147483647); // Max int32_t

  auto add_result =
      mj_env_add_template(env, "append_int32_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_int32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 10, -20, 2147483647");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendInt16Values) {
  // Test appending 16-bit integer values to a list
  auto list = mj_value_new_list();

  // Append int16 values
  mj_value_append_int16(list, 100);
  mj_value_append_int16(list, -200);
  mj_value_append_int16(list, 32767); // Max int16_t

  auto add_result =
      mj_env_add_template(env, "append_int16_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_int16_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 100, -200, 32767");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendInt8Values) {
  // Test appending 8-bit integer values to a list
  auto list = mj_value_new_list();

  // Append int8 values
  mj_value_append_int8(list, 50);
  mj_value_append_int8(list, -100);
  mj_value_append_int8(list, 127); // Max int8_t

  auto add_result =
      mj_env_add_template(env, "append_int8_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_int8_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 50, -100, 127");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendUintValues) {
  // Test appending 64-bit unsigned integer values to a list
  auto list = mj_value_new_list();

  // Append uint64 values
  mj_value_append_uint(list, 0);
  mj_value_append_uint(list, 1000000000ULL);
  mj_value_append_uint(list, 18446744073709551615ULL); // Max uint64_t

  auto add_result =
      mj_env_add_template(env, "append_uint_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_uint_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result,
               "Values: 0, 1000000000, 18446744073709551615");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendUint32Values) {
  // Test appending 32-bit unsigned integer values to a list
  auto list = mj_value_new_list();

  // Append uint32 values
  mj_value_append_uint32(list, 0);
  mj_value_append_uint32(list, 2000000000U);
  mj_value_append_uint32(list, 4294967295U); // Max uint32_t

  auto add_result =
      mj_env_add_template(env, "append_uint32_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_uint32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 0, 2000000000, 4294967295");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendUint16Values) {
  // Test appending 16-bit unsigned integer values to a list
  auto list = mj_value_new_list();

  // Append uint16 values
  mj_value_append_uint16(list, 0);
  mj_value_append_uint16(list, 30000);
  mj_value_append_uint16(list, 65535); // Max uint16_t

  auto add_result =
      mj_env_add_template(env, "append_uint16_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_uint16_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 0, 30000, 65535");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendUint8Values) {
  // Test appending 8-bit unsigned integer values to a list
  auto list = mj_value_new_list();

  // Append uint8 values
  mj_value_append_uint8(list, 0);
  mj_value_append_uint8(list, 128);
  mj_value_append_uint8(list, 255); // Max uint8_t

  auto add_result =
      mj_env_add_template(env, "append_uint8_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_uint8_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 0, 128, 255");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendFloatValues) {
  // Test appending 64-bit floating-point values to a list
  auto list = mj_value_new_list();

  // Append float values
  mj_value_append_float(list, 3.14);
  mj_value_append_float(list, -2.71);
  mj_value_append_float(list, 0.0);

  auto add_result =
      mj_env_add_template(env, "append_float_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_float_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 3.14, -2.71, 0.0");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendFloat32Values) {
  // Test appending 32-bit floating-point values to a list
  auto list = mj_value_new_list();

  // Append float32 values
  mj_value_append_float32(list, 1.5f);
  mj_value_append_float32(list, -2.5f);
  mj_value_append_float32(list, 0.0f);

  auto add_result =
      mj_env_add_template(env, "append_float32_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_float32_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: 1.5, -2.5, 0.0");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendBoolValues) {
  // Test appending boolean values to a list
  auto list = mj_value_new_list();

  // Append bool values
  mj_value_append_bool(list, true);
  mj_value_append_bool(list, false);
  mj_value_append_bool(list, true);

  auto add_result =
      mj_env_add_template(env, "append_bool_template",
                          "Values: {% for val in values %}{{ val }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_bool_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Values: true, false, true");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendValueObjects) {
  // Test appending mj_value objects to a list
  auto list = mj_value_new_list();

  // Create nested value objects
  auto obj1 = mj_value_new();
  mj_value_set_string(obj1, "name", "Alice");
  mj_value_set_int(obj1, "age", 30);

  auto obj2 = mj_value_new();
  mj_value_set_string(obj2, "name", "Bob");
  mj_value_set_int(obj2, "age", 25);

  // Append value objects
  mj_value_append_value(list, obj1);
  mj_value_append_value(list, obj2);

  auto add_result = mj_env_add_template(
      env, "append_value_template",
      "Users: {% for user in users %}{{ user.name }} ({{ user.age }}){% if not "
      "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "users", list);

  auto render_result =
      mj_env_render_template(env, "append_value_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Users: Alice (30), Bob (25)");
  mj_str_free(render_result.result);

  mj_value_free(obj1);
  mj_value_free(obj2);
  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendMixedTypes) {
  // Test appending different types to the same list
  auto list = mj_value_new_list();

  // Append different types
  mj_value_append_string(list, "Hello");
  mj_value_append_int(list, 42);
  mj_value_append_float(list, 3.14);
  mj_value_append_bool(list, true);

  auto add_result =
      mj_env_add_template(env, "append_mixed_template",
                          "Mixed: {% for item in items %}{{ item }}{% if not "
                          "loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "items", list);

  auto render_result =
      mj_env_render_template(env, "append_mixed_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Mixed: Hello, 42, 3.14, true");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendToEmptyList) {
  // Test appending to an empty list
  auto list = mj_value_new_list();

  // Initially empty, then append one item
  auto add_result =
      mj_env_add_template(env, "append_empty_template",
                          "Length: {{ items|length }}{% if items|length > 0 "
                          "%}, First: {{ items[0] }}{% endif %}");
  EXPECT_EQ(add_result.error, nullptr);

  // Check empty list first
  mj_value_set_value(value, "items", list);
  auto render_result =
      mj_env_render_template(env, "append_empty_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Length: 0");
  mj_str_free(render_result.result);

  // Now append one item and update the value again
  mj_value_append_string(list, "First Item");
  mj_value_set_value(value, "items", list); // Need to update the value again
  render_result = mj_env_render_template(env, "append_empty_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Length: 1, First: First Item");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendSequentialValues) {
  // Test appending values sequentially and checking order
  auto list = mj_value_new_list();

  // Append values in sequence
  for (int i = 1; i <= 5; i++) {
    mj_value_append_int(list, i);
  }

  auto add_result =
      mj_env_add_template(env, "append_sequential_template",
                          "Sequence: {% for num in numbers %}{{ num }}{% if "
                          "not loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "numbers", list);

  auto render_result =
      mj_env_render_template(env, "append_sequential_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result, "Sequence: 1, 2, 3, 4, 5");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

TEST_F(MiniJinjaTest, AppendBoundaryValues) {
  // Test appending boundary values for different integer types
  auto list = mj_value_new_list();

  // Append boundary values
  mj_value_append_int8(list, INT8_MIN);
  mj_value_append_int8(list, INT8_MAX);
  mj_value_append_uint8(list, 0);
  mj_value_append_uint8(list, UINT8_MAX);
  mj_value_append_int16(list, INT16_MIN);
  mj_value_append_int16(list, INT16_MAX);

  auto add_result =
      mj_env_add_template(env, "append_boundary_template",
                          "Boundaries: {% for val in values %}{{ val }}{% if "
                          "not loop.last %}, {% endif %}{% endfor %}");
  EXPECT_EQ(add_result.error, nullptr);

  mj_value_set_value(value, "values", list);

  auto render_result =
      mj_env_render_template(env, "append_boundary_template", value);
  EXPECT_EQ(render_result.error, nullptr);
  EXPECT_STREQ(render_result.result,
               "Boundaries: -128, 127, 0, 255, -32768, 32767");
  mj_str_free(render_result.result);

  mj_value_free(list);
}

#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, SetListStringValues)
{
    // Test setting string list values
    auto add_result = mj_env_add_template(env, "list_string_template",
        "Names: {% for name in names %}{{ name }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_string
    const char* string_list[] = { "Alice", "Bob", "Charlie" };
    mj_value_set_list_string(value, "names", string_list, 3);

    auto render_result = mj_env_render_template(env, "list_string_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Names: Alice, Bob, Charlie");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListIntValues)
{
    // Test setting 64-bit integer list values
    auto add_result = mj_env_add_template(env, "list_int_template",
        "Numbers: {% for num in numbers %}{{ num }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_int
    int64_t int_list[] = { 1, 2, 3, 9223372036854775807LL };
    mj_value_set_list_int(value, "numbers", int_list, 4);

    auto render_result = mj_env_render_template(env, "list_int_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Numbers: 1, 2, 3, 9223372036854775807");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListInt32Values)
{
    // Test setting 32-bit integer list values
    auto add_result = mj_env_add_template(env, "list_int32_template",
        "Values: {% for val in values %}{{ val }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_int32
    int32_t int32_list[] = { 10, 20, 30, 2147483647 };
    mj_value_set_list_int32(value, "values", int32_list, 4);

    auto render_result = mj_env_render_template(env, "list_int32_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Values: 10, 20, 30, 2147483647");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListFloatValues)
{
    // Test setting double precision float list values
    auto add_result = mj_env_add_template(env, "list_float_template",
        "Decimals: {% for dec in decimals %}{{ dec }}{% if "
        "not loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_float
    double float_list[] = { 1.1, 2.2, 3.3, 3.141592653589793 };
    mj_value_set_list_float(value, "decimals", float_list, 4);

    auto render_result = mj_env_render_template(env, "list_float_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result,
        "Decimals: 1.1, 2.2, 3.3, 3.141592653589793");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListBoolValues)
{
    // Test setting boolean list values
    auto add_result = mj_env_add_template(env, "list_bool_template",
        "Flags: {% for flag in flags %}{{ flag }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_bool
    bool bool_list[] = { true, false, true, false };
    mj_value_set_list_bool(value, "flags", bool_list, 4);

    auto render_result = mj_env_render_template(env, "list_bool_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Flags: true, false, true, false");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, EmptyListValues)
{
    // Test setting empty list values
    auto add_result = mj_env_add_template(
        env, "empty_list_template",
        "Empty: {% if empty_list %}Not empty{% else %}Empty{% endif %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test empty string list - create a valid pointer for empty array
    const char* empty_array[1] = {
        nullptr
    }; // Create a valid array but don't use it
    mj_value_set_list_string(value, "empty_list", empty_array, 0);

    auto render_result = mj_env_render_template(env, "empty_list_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Empty: Empty");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, MixedListsInTemplate)
{
    // Test using multiple different list types in one template
    auto add_result = mj_env_add_template(env, "mixed_lists_template",
        "Names: {{ names|length }}, Numbers: {{ "
        "numbers|length }}, Flags: {{ flags|length }}");
    EXPECT_EQ(add_result.error, nullptr);

    // Set different types of lists
    const char* names[] = { "Alice", "Bob" };
    int32_t numbers[] = { 1, 2, 3 };
    bool flags[] = { true, false, true, false };

    mj_value_set_list_string(value, "names", names, 2);
    mj_value_set_list_int32(value, "numbers", numbers, 3);
    mj_value_set_list_bool(value, "flags", flags, 4);

    auto render_result = mj_env_render_template(env, "mixed_lists_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Names: 2, Numbers: 3, Flags: 4");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, ListWithSingleElement)
{
    // Test lists with single elements
    auto add_result = mj_env_add_template(
        env, "single_element_template",
        "Single: {% for item in single %}{{ item }}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test single element string list
    const char* single_string[] = { "OnlyOne" };
    mj_value_set_list_string(value, "single", single_string, 1);

    auto render_result = mj_env_render_template(env, "single_element_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Single: OnlyOne");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListFloat32Values)
{
    // Test setting single precision float list values
    auto add_result = mj_env_add_template(env, "list_float32_template",
        "Values: {% for val in values %}{{ val }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_float32
    float float32_list[] = { 1.0f, 2.0f, 3.0f,
        4.5f }; // These are exactly representable
    mj_value_set_list_float32(value, "values", float32_list, 4);

    auto render_result = mj_env_render_template(env, "list_float32_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    // Use exact values that can be precisely represented in float32
    EXPECT_STREQ(render_result.result, "Values: 1.0, 2.0, 3.0, 4.5");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetLargeListValues)
{
    // Test setting large list values
    auto add_result = mj_env_add_template(env, "large_list_template",
        "Count: {{ numbers|length }}, First: {{ numbers[0] "
        "}}, Last: {{ numbers[-1] }}");
    EXPECT_EQ(add_result.error, nullptr);

    // Create large array
    const int LIST_SIZE = 1000;
    int64_t* large_list = new int64_t[LIST_SIZE];
    for (int i = 0; i < LIST_SIZE; i++) {
        large_list[i] = i + 1;
    }

    mj_value_set_list_int(value, "numbers", large_list, LIST_SIZE);

    auto render_result = mj_env_render_template(env, "large_list_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Count: 1000, First: 1, Last: 1000");
    mj_str_free(render_result.result);

    delete[] large_list;
}

TEST_F(MiniJinjaTest, SetListWithNegativeValues)
{
    // Test setting list with negative values
    auto add_result = mj_env_add_template(env, "negative_list_template",
        "Values: {% for val in values %}{{ val }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test negative integers
    int64_t negative_ints[] = { -100, -50, 0, 50, 100 };
    mj_value_set_list_int(value, "values", negative_ints, 5);

    auto render_result = mj_env_render_template(env, "negative_list_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Values: -100, -50, 0, 50, 100");
    mj_str_free(render_result.result);

    // Test negative floats
    double negative_floats[] = { -3.14, -2.71, 0.0, 2.71, 3.14 };
    mj_value_set_list_float(value, "values", negative_floats, 5);

    render_result = mj_env_render_template(env, "negative_list_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Values: -3.14, -2.71, 0.0, 2.71, 3.14");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListWithSpecialStringValues)
{
    // Test setting list with strings containing special characters
    auto add_result = mj_env_add_template(env, "special_string_list_template",
        "Items: {% for item in items %}'{{ item }}'{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test strings with special characters
    const char* special_strings[] = {
        "Hello\nWorld", "Tab\tSeparated", "Quote\"Test", "Apostrophe's Test",
        "" // Empty string
    };
    mj_value_set_list_string(value, "items", special_strings, 5);

    auto render_result = mj_env_render_template(env, "special_string_list_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Items: 'Hello\nWorld', 'Tab\tSeparated', "
                                       "'Quote\"Test', 'Apostrophe's Test', ''");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListWithMaxMinValues)
{
    // Test setting list with maximum and minimum values
    auto add_result = mj_env_add_template(env, "extremes_template",
        "Int64: {{ int64_vals[0] }}, {{ int64_vals[1] }}; "
        "Int32: {{ int32_vals[0] }}, {{ int32_vals[1] }}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test with max and min int64 values
    int64_t int64_extremes[] = { INT64_MAX, INT64_MIN };
    mj_value_set_list_int(value, "int64_vals", int64_extremes, 2);

    // Test with max and min int32 values
    int32_t int32_extremes[] = { INT32_MAX, INT32_MIN };
    mj_value_set_list_int32(value, "int32_vals", int32_extremes, 2);

    auto render_result = mj_env_render_template(env, "extremes_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    // Check that the values are rendered (exact string comparison may be platform
    // dependent)
    EXPECT_TRUE(strstr(render_result.result, "9223372036854775807") != nullptr);
    EXPECT_TRUE(strstr(render_result.result, "2147483647") != nullptr);
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListWithNestedValues)
{
    // Test setting list with nested value objects
    auto add_result = mj_env_add_template(env, "nested_list_template",
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

    const mj_value* values[] = { user1, user2 };
    // Set list of nested values
    mj_value_set_list_value(value, "users", values, 2);

    auto render_result = mj_env_render_template(env, "nested_list_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Users: Alice (Age: 30), Bob (Age: 25)");
    mj_str_free(render_result.result);

    // Free nested values
    mj_value_free(user1);
    mj_value_free(user2);
}

TEST_F(MiniJinjaTest, SetListInt8Values)
{
    // Test setting 8-bit integer list values
    auto add_result = mj_env_add_template(env, "list_int8_template",
        "Values: {% for val in values %}{{ val }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_int8
    int8_t int8_list[] = { -128, -50, 0, 50, 127 };
    mj_value_set_list_int8(value, "values", int8_list, 5);

    auto render_result = mj_env_render_template(env, "list_int8_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Values: -128, -50, 0, 50, 127");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListUintValues)
{
    // Test setting 64-bit unsigned integer list values
    auto add_result = mj_env_add_template(env, "list_uint_template",
        "Values: {% for val in values %}{{ val }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_uint
    uint64_t uint_list[] = { 0, 1000000000000ULL, 9223372036854775808ULL,
        18446744073709551615ULL };
    mj_value_set_list_uint(value, "values", uint_list, 4);

    auto render_result = mj_env_render_template(env, "list_uint_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(
        render_result.result,
        "Values: 0, 1000000000000, 9223372036854775808, 18446744073709551615");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListUint32Values)
{
    // Test setting 32-bit unsigned integer list values
    auto add_result = mj_env_add_template(env, "list_uint32_template",
        "Values: {% for val in values %}{{ val }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_uint32
    uint32_t uint32_list[] = { 0, 1000000, 2147483648U, 4294967295U };
    mj_value_set_list_uint32(value, "values", uint32_list, 4);

    auto render_result = mj_env_render_template(env, "list_uint32_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result,
        "Values: 0, 1000000, 2147483648, 4294967295");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListUint16Values)
{
    // Test setting 16-bit unsigned integer list values
    auto add_result = mj_env_add_template(env, "list_uint16_template",
        "Values: {% for val in values %}{{ val }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_uint16
    uint16_t uint16_list[] = { 0, 1000, 32768, 65535 };
    mj_value_set_list_uint16(value, "values", uint16_list, 4);

    auto render_result = mj_env_render_template(env, "list_uint16_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Values: 0, 1000, 32768, 65535");
    mj_str_free(render_result.result);
}

TEST_F(MiniJinjaTest, SetListUint8Values)
{
    // Test setting 8-bit unsigned integer list values
    auto add_result = mj_env_add_template(env, "list_uint8_template",
        "Values: {% for val in values %}{{ val }}{% if not "
        "loop.last %}, {% endif %}{% endfor %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Test mj_value_set_list_uint8
    uint8_t uint8_list[] = { 0, 50, 128, 200, 255 };
    mj_value_set_list_uint8(value, "values", uint8_list, 5);

    auto render_result = mj_env_render_template(env, "list_uint8_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result, "Values: 0, 50, 128, 200, 255");
    mj_str_free(render_result.result);
}

// Test mixing different list types
TEST_F(MiniJinjaTest, SetMixedListTypes)
{
    auto add_result = mj_env_add_template(
        env, "mixed_list_template",
        "int8: {{ i8_list|length }}, uint8: {{ u8_list|length }}, "
        "int16: {{ i16_list|length }}, uint16: {{ u16_list|length }}, "
        "uint32: {{ u32_list|length }}, uint64: {{ u64_list|length }}");
    EXPECT_EQ(add_result.error, nullptr);

    // Set different list types
    int8_t i8_vals[] = { -1, 0, 1 };
    uint8_t u8_vals[] = { 0, 100, 255 };
    int16_t i16_vals[] = { -1000, 0, 1000 };
    uint16_t u16_vals[] = { 0, 30000, 65535 };
    uint32_t u32_vals[] = { 0, 2000000000U, 4294967295U };
    uint64_t u64_vals[] = { 0, 10000000000ULL, 18446744073709551615ULL };

    mj_value_set_list_int8(value, "i8_list", i8_vals, 3);
    mj_value_set_list_uint8(value, "u8_list", u8_vals, 3);
    mj_value_set_list_int16(value, "i16_list", i16_vals, 3);
    mj_value_set_list_uint16(value, "u16_list", u16_vals, 3);
    mj_value_set_list_uint32(value, "u32_list", u32_vals, 3);
    mj_value_set_list_uint(value, "u64_list", u64_vals, 3);

    auto render_result = mj_env_render_template(env, "mixed_list_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result,
        "int8: 3, uint8: 3, int16: 3, uint16: 3, uint32: 3, uint64: 3");
    mj_str_free(render_result.result);
}

// Test edge cases with empty lists for new types
TEST_F(MiniJinjaTest, SetEmptyListsForNewTypes)
{
    auto add_result = mj_env_add_template(
        env, "empty_lists_template",
        "{% if int8_list %}Not empty{% else %}Empty{% endif %} - "
        "{% if uint8_list %}Not empty{% else %}Empty{% endif %} - "
        "{% if int16_list %}Not empty{% else %}Empty{% endif %} - "
        "{% if uint16_list %}Not empty{% else %}Empty{% endif %} - "
        "{% if uint32_list %}Not empty{% else %}Empty{% endif %} - "
        "{% if uint64_list %}Not empty{% else %}Empty{% endif %}");
    EXPECT_EQ(add_result.error, nullptr);

    // Set empty lists for all new types
    int8_t empty_i8[1] = { 0 };
    uint8_t empty_u8[1] = { 0 };
    int16_t empty_i16[1] = { 0 };
    uint16_t empty_u16[1] = { 0 };
    uint32_t empty_u32[1] = { 0 };
    uint64_t empty_u64[1] = { 0 };

    mj_value_set_list_int8(value, "int8_list", empty_i8, 0);
    mj_value_set_list_uint8(value, "uint8_list", empty_u8, 0);
    mj_value_set_list_int16(value, "int16_list", empty_i16, 0);
    mj_value_set_list_uint16(value, "uint16_list", empty_u16, 0);
    mj_value_set_list_uint32(value, "uint32_list", empty_u32, 0);
    mj_value_set_list_uint(value, "uint64_list", empty_u64, 0);

    auto render_result = mj_env_render_template(env, "empty_lists_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(render_result.result,
        "Empty - Empty - Empty - Empty - Empty - Empty");
    mj_str_free(render_result.result);
}

// Test boundary values for all integer types
TEST_F(MiniJinjaTest, SetBoundaryValues)
{
    auto add_result = mj_env_add_template(env, "boundary_template",
        "int8: {{ i8_min }}, {{ i8_max }}; "
        "uint8: {{ u8_min }}, {{ u8_max }}; "
        "int16: {{ i16_min }}, {{ i16_max }}; "
        "uint16: {{ u16_min }}, {{ u16_max }}");
    EXPECT_EQ(add_result.error, nullptr);

    // Set boundary values
    mj_value_set_int8(value, "i8_min", INT8_MIN);
    mj_value_set_int8(value, "i8_max", INT8_MAX);
    mj_value_set_uint8(value, "u8_min", 0);
    mj_value_set_uint8(value, "u8_max", UINT8_MAX);
    mj_value_set_int16(value, "i16_min", INT16_MIN);
    mj_value_set_int16(value, "i16_max", INT16_MAX);
    mj_value_set_uint16(value, "u16_min", 0);
    mj_value_set_uint16(value, "u16_max", UINT16_MAX);

    auto render_result = mj_env_render_template(env, "boundary_template", value);
    EXPECT_EQ(render_result.error, nullptr);
    EXPECT_STREQ(
        render_result.result,
        "int8: -128, 127; uint8: 0, 255; int16: -32768, 32767; uint16: 0, 65535");
    mj_str_free(render_result.result);
}

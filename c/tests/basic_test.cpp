#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, MultipleValuesInTemplate)
{
    // Test using multiple different value types in one template
    auto error = mj_env_add_template(env, "multi_template",
        "Name: {{ name }}, Age: {{ age }}, Score: {{ score "
        "}}, Active: {{ active }}");
    EXPECT_EQ(error, nullptr);

    // Create JSON data with different types of values
    std::string json_data = R"({
        "name": "Alice",
        "age": 25,
        "score": 95.5,
        "active": true
    })";

    auto render_result = renderTemplate("multi_template", json_data);
    EXPECT_EQ(render_result->error, nullptr);
    EXPECT_STREQ(render_result->result,
        "Name: Alice, Age: 25, Score: 95.5, Active: true");
    mj_result_env_render_template_free(render_result);
}

TEST_F(MiniJinjaTest, OverwriteValues)
{
    // Test different value types using different templates
    auto error = mj_env_add_template(env, "test_value", "Value: {{ val }}");
    EXPECT_EQ(error, nullptr);

    // Test string value
    std::string json_data1 = R"({"val": "Hello"})";
    auto render_result1 = renderTemplate("test_value", json_data1);
    EXPECT_EQ(render_result1->error, nullptr);
    EXPECT_STREQ(render_result1->result, "Value: Hello");
    mj_result_env_render_template_free(render_result1);

    // Test integer value
    std::string json_data2 = R"({"val": 42})";
    auto render_result2 = renderTemplate("test_value", json_data2);
    EXPECT_EQ(render_result2->error, nullptr);
    EXPECT_STREQ(render_result2->result, "Value: 42");
    mj_result_env_render_template_free(render_result2);

    // Test float value
    std::string json_data3 = R"({"val": 3.14})";
    auto render_result3 = renderTemplate("test_value", json_data3);
    EXPECT_EQ(render_result3->error, nullptr);
    EXPECT_STREQ(render_result3->result, "Value: 3.14");
    mj_result_env_render_template_free(render_result3);

    // Test boolean value
    std::string json_data4 = R"({"val": true})";
    auto render_result4 = renderTemplate("test_value", json_data4);
    EXPECT_EQ(render_result4->error, nullptr);
    EXPECT_STREQ(render_result4->result, "Value: true");
    mj_result_env_render_template_free(render_result4);
}

#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, RenderNamedString)
{
    // Test rendering a named string template
    std::string json_data = R"({"name": "World"})";
    
    auto render_result = renderNamedString("test_template", "Hello {{ name }}!", json_data);
    EXPECT_EQ(render_result->error, nullptr);
    EXPECT_STREQ(render_result->result, "Hello World!");
    
    mj_result_env_render_template_free(render_result);
}

TEST_F(MiniJinjaTest, RenderNamedStringWithComplexData)
{
    // Test rendering with complex JSON data
    std::string json_data = R"({
        "user": {
            "name": "Alice",
            "age": 30
        },
        "items": ["apple", "banana", "cherry"]
    })";
    
    const char* template_str = R"(
User: {{ user.name }} ({{ user.age }} years old)
Items:
{% for item in items -%}
- {{ item }}
{% endfor -%}
)";
    
    auto render_result = renderNamedString("complex_template", template_str, json_data);
    EXPECT_EQ(render_result->error, nullptr);
    
    const char* expected = R"(
User: Alice (30 years old)
Items:
- apple
- banana
- cherry
)";
    
    EXPECT_STREQ(render_result->result, expected);
    
    mj_result_env_render_template_free(render_result);
}

TEST_F(MiniJinjaTest, RenderNamedStringError)
{
    // Test error handling with invalid template syntax in named string
    std::string json_data = "{}";
    
    auto render_result = renderNamedString("error_template", "Hello {{ unclosed", json_data);
    EXPECT_NE(render_result->error, nullptr);
    
    if (render_result->error != nullptr) {
        EXPECT_EQ(render_result->error->code, MJ_SYNTAX_ERROR);
        EXPECT_NE(render_result->error->message, nullptr);
    }
    
    mj_result_env_render_template_free(render_result);
}


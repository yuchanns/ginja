#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, ErrorHandling)
{
    // Test error handling with invalid template syntax
    // Add template with invalid syntax
    auto error = mj_env_add_template(env, "invalid_template",
        "Hello {{ unclosed");

    // This should produce a syntax error
    EXPECT_NE(error, nullptr);
        EXPECT_EQ(error->code, MJ_SYNTAX_ERROR);
        EXPECT_NE(error->message, nullptr);
        EXPECT_GT(strlen(error->message), 0);

    mj_error_free(error);
}

TEST_F(MiniJinjaTest, TemplateNotFound)
{
    // Test rendering non-existent template
    std::string json_data = "{}";
    auto render_result = renderTemplate("non_existent", json_data);

    // This should produce an error
    EXPECT_NE(render_result->error, nullptr);
    if (render_result->error != nullptr) {
        EXPECT_EQ(render_result->error->code, MJ_TEMPLATE_NOT_FOUND);
        EXPECT_NE(render_result->error->message, nullptr);
    }
    mj_result_env_render_template_free(render_result);
}

TEST_F(MiniJinjaTest, InvalidTemplateSyntax)
{
    // Test various invalid template syntax scenarios

    // Test unclosed variable
    auto error = mj_env_add_template(env, "unclosed_var", "Hello {{ name");
    EXPECT_NE(error, nullptr);
    if (error != nullptr) {
        EXPECT_EQ(error->code, MJ_SYNTAX_ERROR);
        EXPECT_NE(error->message, nullptr);
        EXPECT_GT(strlen(error->message), 0);
        mj_error_free(error);
    }

    // Test unclosed block
    error = mj_env_add_template(env, "unclosed_block",
        "{% for item in items %}{{ item }");
    EXPECT_NE(error, nullptr);
    if (error != nullptr) {
        EXPECT_EQ(error->code, MJ_SYNTAX_ERROR);
        EXPECT_NE(error->message, nullptr);
        EXPECT_GT(strlen(error->message), 0);
        mj_error_free(error);
    }

    // Test invalid filter
    error = mj_env_add_template(env, "invalid_filter",
        "{{ name | nonexistent }}");
    EXPECT_EQ(error,
        nullptr); // Template syntax is valid, error occurs at render time

    std::string json_data = R"({"name": "test"})";
    auto render_result = renderTemplate("invalid_filter", json_data);
    if (render_result->error != nullptr) {
        EXPECT_EQ(render_result->error->code, MJ_UNKNOWN_FILTER);
        EXPECT_NE(render_result->error->message, nullptr);
    }
    mj_result_env_render_template_free(render_result);
}

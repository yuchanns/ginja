#include "test_base.h"
#include <cstring>

TEST_F(MiniJinjaTest, ErrorHandling)
{
    // Test error handling with invalid template syntax
    // Add template with invalid syntax
    auto add_result = mj_env_add_template(env, "invalid_template",
        "Hello {{ unclosed");

    // This should produce a syntax error
    EXPECT_NE(add_result.error, nullptr);
    if (add_result.error != nullptr) {
        EXPECT_EQ(add_result.error->code, MJ_SYNTAX_ERROR);
        EXPECT_NE(add_result.error->message, nullptr);
        EXPECT_GT(strlen(add_result.error->message), 0);

        mj_error_free(add_result.error);
    }
}

TEST_F(MiniJinjaTest, TemplateNotFound)
{
    // Test rendering non-existent template
    auto render_result = mj_env_render_template(env, "non_existent", value);

    // This should produce an error
    EXPECT_NE(render_result.error, nullptr);
    if (render_result.error != nullptr) {
        EXPECT_EQ(render_result.error->code, MJ_TEMPLATE_NOT_FOUND);
        EXPECT_NE(render_result.error->message, nullptr);

        // Test mj_error_free
        mj_error_free(render_result.error);
    }
}

TEST_F(MiniJinjaTest, InvalidTemplateSyntax)
{
    // Test various invalid template syntax scenarios

    // Test unclosed variable
    auto add_result1 = mj_env_add_template(env, "unclosed_var", "Hello {{ name");
    EXPECT_NE(add_result1.error, nullptr);
    if (add_result1.error != nullptr) {
        EXPECT_EQ(add_result1.error->code, MJ_SYNTAX_ERROR);
        EXPECT_NE(add_result1.error->message, nullptr);
        EXPECT_GT(strlen(add_result1.error->message), 0);
        mj_error_free(add_result1.error);
    }

    // Test unclosed block
    auto add_result2 = mj_env_add_template(env, "unclosed_block",
        "{% for item in items %}{{ item }");
    EXPECT_NE(add_result2.error, nullptr);
    if (add_result2.error != nullptr) {
        EXPECT_EQ(add_result2.error->code, MJ_SYNTAX_ERROR);
        EXPECT_NE(add_result2.error->message, nullptr);
        EXPECT_GT(strlen(add_result2.error->message), 0);
        mj_error_free(add_result2.error);
    }

    // Test invalid filter
    auto add_result3 = mj_env_add_template(env, "invalid_filter",
        "{{ name | nonexistent }}");
    EXPECT_EQ(add_result3.error,
        nullptr); // Template syntax is valid, error occurs at render time

    mj_value_set_string(value, "name", "test");

    auto render_result = mj_env_render_template(env, "invalid_filter", value);
    if (render_result.error != nullptr) {
        EXPECT_EQ(render_result.error->code, MJ_UNKNOWN_FILTER);
        EXPECT_NE(render_result.error->message, nullptr);
        mj_error_free(render_result.error);
    } else {
        mj_str_free(render_result.result);
    }
}

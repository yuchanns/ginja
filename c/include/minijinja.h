/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


#ifndef _MINIJINJA_H
#define _MINIJINJA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * \brief Represents error codes for different types of MiniJinja errors.
 *
 * This enumeration defines all possible error conditions that can occur
 * during template compilation, rendering, and value operations in MiniJinja.
 * Each error code corresponds to a specific error kind from the core MiniJinja library.
 *
 * @see mj_error This structure contains both the error code and message
 *
 * \note These error codes are C-compatible and can be used for error handling
 * in C/C++ applications.
 */
typedef enum mj_code {
  /**
   * Error when attempting to use a non-primitive value where a primitive is expected
   */
  MJ_NON_PRIMITIVE,
  /**
   * Error when attempting to use a non-key value as a map key
   */
  MJ_NON_KEY,
  /**
   * Error when attempting an invalid operation on a value
   */
  MJ_INVALID_OPERATION,
  /**
   * Syntax error in template source code
   */
  MJ_SYNTAX_ERROR,
  /**
   * Template with the specified name was not found
   */
  MJ_TEMPLATE_NOT_FOUND,
  /**
   * Too many arguments provided to a function, filter, or test
   */
  MJ_TOO_MANY_ARGUMENTS,
  /**
   * Required argument is missing from a function, filter, or test call
   */
  MJ_MISSING_ARGUMENT,
  /**
   * Unknown or undefined filter used in template
   */
  MJ_UNKNOWN_FILTER,
  /**
   * Unknown or undefined test used in template
   */
  MJ_UNKNOWN_TEST,
  /**
   * Unknown or undefined function used in template
   */
  MJ_UNKNOWN_FUNCTION,
  /**
   * Unknown or undefined method called on a value
   */
  MJ_UNKNOWN_METHOD,
  /**
   * Invalid escape sequence in template
   */
  MJ_BAD_ESCAPE,
  /**
   * Undefined variable or expression accessed in template
   */
  MJ_UNDEFINED_ERROR,
  /**
   * Error during value serialization
   */
  MJ_BAD_SERIALIZATION,
  /**
   * Error during value deserialization
   */
  MJ_CANNOT_DESERIALIZE,
  /**
   * Error in template include operation
   */
  MJ_BAD_INCLUDE,
  /**
   * Error in template block evaluation
   */
  MJ_EVAL_BLOCK,
  /**
   * Error when attempting to unpack a value
   */
  MJ_CANNOT_UNPACK,
  /**
   * Error during template output writing
   */
  MJ_WRITE_FAILURE,
  /**
   * Unknown or undefined block used in template
   */
  MJ_UNKNOWN_BLOCK,
} mj_code;

/**
 * \brief Defines how the template environment handles undefined variables and expressions.
 *
 * This enumeration controls the behavior when undefined variables or expressions
 * are encountered during template rendering.
 *
 * @see mj_env_set_undefined_behavior Function to set the undefined behavior policy
 */
typedef enum mj_undefined_behavior {
  /**
   * The default, somewhat lenient undefined behavior.
   * Undefined variables are rendered as empty strings without errors.
   */
  MJ_UNDEFINED_BEHAVIOR_LENIENT,
  /**
   * Complains very quickly about undefined values.
   * Undefined variables cause rendering errors immediately.
   */
  MJ_UNDEFINED_BEHAVIOR_STRICT,
  /**
   * Like Lenient, but also allows chaining of undefined lookups.
   * Allows operations like `{{ undefined.foo.bar }}` without errors.
   */
  MJ_UNDEFINED_BEHAVIOR_CHAINABLE,
} mj_undefined_behavior;

/**
 * \brief Represents a MiniJinja template environment that manages templates
 * and their rendering configuration.
 *
 * @see mj_env_new This function constructs a new environment
 * @see mj_env_free This function frees the heap memory of the environment
 *
 * \note The mj_env actually owns a pointer to a Arc<RwLock<minijinja::Environment>>,
 * which is inside the Rust core code and supports concurrent access.
 *
 * \remark You may use the field `inner` to check whether this is a NULL
 * environment.
 */
typedef struct mj_env {
  /**
   * The pointer to the Arc<RwLock<minijinja::Environment>> in the Rust code.
   * Only touch this on judging whether it is NULL.
   */
  void *inner;
} mj_env;

/**
 * \brief Represents a MiniJinja error with code and message information.
 *
 * This structure encapsulates error information from MiniJinja operations,
 * including both an error code that categorizes the error type and a
 * human-readable error message with full error chain details.
 *
 * @see mj_code The error code enumeration
 * @see mj_error_free This function frees the heap memory of the error
 *
 * \note The mj_error structure owns the error message string, which must
 * be freed using mj_error_free to prevent memory leaks.
 *
 * \remark The message field contains the complete error chain, including
 * the root cause and all intermediate error causes.
 */
typedef struct mj_error {
  /**
   * The error code categorizing the type of error that occurred
   */
  enum mj_code code;
  /**
   * Pointer to a null-terminated C string containing the error message
   */
  const char *message;
} mj_error;

/**
 * \brief Result structure for template rendering functions.
 *
 * This structure contains the result of rendering a template.
 * On success, the result field contains the rendered string and error is NULL.
 * On failure, result is NULL and error contains error information.
 *
 * @see mj_env_render_template Function that returns this result type
 * @see mj_env_render_named_string Function that returns this result type
 *
 * \note The result string should be freed using mj_str_free when no longer needed.
 */
typedef struct mj_result_env_render_template {
  /**
   * Pointer to the rendered string, or NULL on failure
   */
  char *result;
  /**
   * Pointer to error information, or NULL on success
   */
  struct mj_error *error;
} mj_result_env_render_template;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void mj_env_free(struct mj_env *ptr);

/**
 * \brief Creates a new MiniJinja template environment.
 *
 * This function allocates and initializes a new MiniJinja environment
 * that can be used to manage templates and render them with context data.
 *
 * @return mj_result_env_new A result structure containing the newly created
 * environment or error information if creation fails.
 *
 * \note The returned environment should be freed using mj_env_free when
 * no longer needed to prevent memory leaks.
 */
struct mj_env *mj_env_new(void);

/**
 * \brief Adds a template to the environment with the given name and source code.
 *
 * This function compiles and stores a template in the environment so it can
 * be rendered later using mj_env_render_template.
 *
 * @param env Pointer to the environment to add the template to
 * @param name Null-terminated string containing the name of the template
 * @param source Null-terminated string containing the template source code
 *
 * @return mj_result_env_add_template A result structure indicating success
 * or containing error information if compilation fails.
 *
 * \note Both name and source parameters must not be NULL.
 */
struct mj_error *mj_env_add_template(struct mj_env *env, const char *name, const char *source);

/**
 * \brief Removes a template from the environment by name.
 *
 * This function removes a previously added template from the environment.
 * After removal, the template can no longer be rendered.
 *
 * @param env Pointer to the environment to remove the template from
 * @param name Null-terminated string containing the name of the template to remove
 *
 * \note The name parameter must not be NULL.
 */
void mj_env_remove_template(struct mj_env *env, const char *name);

/**
 * \brief Clears all templates from the environment.
 *
 * This function removes all previously added templates from the environment.
 * After clearing, no templates can be rendered until new ones are added.
 *
 * @param env Pointer to the environment to clear templates from
 */
void mj_env_clear_templates(struct mj_env *env);

struct mj_result_env_render_template *mj_env_render(struct mj_env *env,
                                                    const char *name,
                                                    const uint8_t *data,
                                                    uintptr_t len);

/**
 * \brief Renders a template from source code without storing it in the environment.
 *
 * This function renders a template directly from source code using the provided
 * context value, without permanently adding it to the environment.
 *
 * @param env Pointer to the environment to use for rendering
 * @param name Null-terminated string containing the name for the template (used for error reporting)
 * @param source Null-terminated string containing the template source code
 * @param value Pointer to the context value to use for rendering
 *
 * @return mj_result_env_render_template A result structure containing the
 * rendered string or error information if rendering fails.
 *
 * \note All parameters must not be NULL. The returned string should be
 * freed using mj_str_free when no longer needed.
 */
struct mj_result_env_render_template *mj_env_render_named_string(struct mj_env *env,
                                                                 const char *name,
                                                                 const char *source,
                                                                 const uint8_t *data,
                                                                 uintptr_t len);

/**
 * \brief Sets whether to strip leading whitespace from blocks.
 *
 * This function configures the environment to automatically strip leading
 * whitespace from template blocks when enabled.
 *
 * @param env Pointer to the environment to configure
 * @param value Boolean value indicating whether to enable lstrip blocks
 */
void mj_env_set_lstrip_blocks(struct mj_env *env, bool value);

/**
 * \brief Sets whether to strip trailing whitespace from blocks.
 *
 * This function configures the environment to automatically strip trailing
 * whitespace from template blocks when enabled.
 *
 * @param env Pointer to the environment to configure
 * @param value Boolean value indicating whether to enable trim blocks
 */
void mj_env_set_trim_blocks(struct mj_env *env, bool value);

/**
 * \brief Sets whether to keep trailing newlines in template output.
 *
 * This function configures the environment to keep or remove trailing
 * newlines in the rendered template output.
 *
 * @param env Pointer to the environment to configure
 * @param value Boolean value indicating whether to keep trailing newlines
 */
void mj_env_set_keep_trailing_newline(struct mj_env *env, bool value);

/**
 * \brief Sets the maximum recursion depth for template rendering.
 *
 * This function configures the maximum depth of recursive template calls
 * to prevent infinite recursion and stack overflow.
 *
 * @param env Pointer to the environment to configure
 * @param value Maximum recursion depth allowed
 */
void mj_env_set_recursion_limit(struct mj_env *env, uintptr_t value);

/**
 * \brief Sets whether to enable debug mode for template rendering.
 *
 * This function configures the environment to enable or disable debug mode,
 * which affects error reporting and template debugging capabilities.
 *
 * @param env Pointer to the environment to configure
 * @param value Boolean value indicating whether to enable debug mode
 */
void mj_env_set_debug(struct mj_env *env, bool value);

/**
 * \brief Sets the undefined behavior policy for the environment.
 *
 * This function configures how the environment handles undefined variables
 * and expressions in templates.
 *
 * @param env Pointer to the environment to configure
 * @param behavior The undefined behavior policy to set
 */
void mj_env_set_undefined_behavior(struct mj_env *env, enum mj_undefined_behavior behavior);

/**
 * \brief Frees the memory allocated for a MiniJinja error.
 *
 * This function properly deallocates both the error structure and its
 * associated error message string, preventing memory leaks.
 *
 * @param ptr Pointer to the error structure to free
 *
 * \note It is safe to pass NULL to this function.
 * \note Only use this function on error pointers returned by MiniJinja C API functions.
 * \note After calling this function, the pointer becomes invalid and should not be used.
 */
void mj_error_free(struct mj_error *ptr);

void mj_result_env_render_template_free(struct mj_result_env_render_template *result);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  /* _MINIJINJA_H */

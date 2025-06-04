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

typedef enum mj_code {
  MJ_NON_PRIMITIVE,
  MJ_NON_KEY,
  MJ_INVALID_OPERATION,
  MJ_SYNTAX_ERROR,
  MJ_TEMPLATE_NOT_FOUND,
  MJ_TOO_MANY_ARGUMENTS,
  MJ_MISSING_ARGUMENT,
  MJ_UNKNOWN_FILTER,
  MJ_UNKNOWN_TEST,
  MJ_UNKNOWN_FUNCTION,
  MJ_UNKNOWN_METHOD,
  MJ_BAD_ESCAPE,
  MJ_UNDEFINED_ERROR,
  MJ_BAD_SERIALIZATION,
  MJ_CANNOT_DESERIALIZE,
  MJ_BAD_INCLUDE,
  MJ_EVAL_BLOCK,
  MJ_CANNOT_UNPACK,
  MJ_WRITE_FAILURE,
  MJ_UNKNOWN_BLOCK,
} mj_code;

typedef enum mj_undefined_behavior {
  /**
   * The default, somewhat lenient undefined behavior.
   */
  MJ_UNDEFINED_BEHAVIOR_LENIENT,
  /**
   * Complains very quickly about undefined values.
   */
  MJ_UNDEFINED_BEHAVIOR_STRICT,
  /**
   * Like Lenient, but also allows chaining of undefined lookups.
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
 * \note The mj_env actually owns a pointer to a minijinja::Environment,
 * which is inside the Rust core code.
 *
 * \remark You may use the field `inner` to check whether this is a NULL
 * environment.
 */
typedef struct mj_env {
  /**
   * The pointer to the minijinja::Environment in the Rust code.
   * Only touch this on judging whether it is NULL.
   */
  void *inner;
} mj_env;

typedef struct mj_result_env_new {
  struct mj_env *env;
} mj_result_env_new;

typedef struct mj_error {
  enum mj_code code;
  const char *message;
} mj_error;

typedef struct mj_result_env_add_template {
  struct mj_error *error;
} mj_result_env_add_template;

typedef struct mj_result_env_render_template {
  char *result;
  struct mj_error *error;
} mj_result_env_render_template;

typedef struct mj_value {
  void *inner;
} mj_value;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void mj_env_free(const struct mj_env *ptr);

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
struct mj_result_env_new mj_env_new(void);

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
struct mj_result_env_add_template mj_env_add_template(struct mj_env *env,
                                                      const char *name,
                                                      const char *source);

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

/**
 * \brief Renders a template by name with the provided context value.
 *
 * This function retrieves a previously added template by name and renders it
 * using the provided context value.
 *
 * @param env Pointer to the environment containing the template
 * @param name Null-terminated string containing the name of the template to render
 * @param value Pointer to the context value to use for rendering
 *
 * @return mj_result_env_render_template A result structure containing the
 * rendered string or error information if rendering fails.
 *
 * \note The name parameter must not be NULL. The returned string should be
 * freed using mj_str_free when no longer needed.
 */
struct mj_result_env_render_template mj_env_render_template(struct mj_env *env,
                                                            const char *name,
                                                            const struct mj_value *value);

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
struct mj_result_env_render_template mj_env_render_named_string(struct mj_env *env,
                                                                const char *name,
                                                                const char *source,
                                                                const struct mj_value *value);

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
 * \brief Frees a C string returned by MiniJinja functions.
 *
 * This function properly deallocates C strings that were allocated by
 * MiniJinja C API functions, such as rendered template output.
 *
 * @param ptr Pointer to the C string to free
 *
 * \note It is safe to pass NULL to this function.
 * \note Only use this function on strings returned by MiniJinja C API functions.
 */
void mj_str_free(char *ptr);

void mj_error_free(const struct mj_error *ptr);

struct mj_value *mj_value_new(void);

void mj_value_free(struct mj_value *ptr);

void mj_value_set_value(struct mj_value *self, const char *key, const struct mj_value *val);

void mj_value_set_string(struct mj_value *self, const char *key, const char *val);

void mj_value_set_int(struct mj_value *self, const char *key, int64_t val);

void mj_value_set_in32(struct mj_value *self, const char *key, int32_t val);

void mj_value_set_float(struct mj_value *self, const char *key, double val);

void mj_value_set_float32(struct mj_value *self, const char *key, float val);

void mj_value_set_bool(struct mj_value *self, const char *key, bool val);

void mj_value_set_list_value(struct mj_value *self,
                             const char *key,
                             const struct mj_value *const *val,
                             uintptr_t len);

void mj_value_set_list_string(struct mj_value *self,
                              const char *key,
                              const char *const *val,
                              uintptr_t len);

void mj_value_set_list_int(struct mj_value *self,
                           const char *key,
                           const int64_t *val,
                           uintptr_t len);

void mj_value_set_list_int32(struct mj_value *self,
                             const char *key,
                             const int32_t *val,
                             uintptr_t len);

void mj_value_set_list_float(struct mj_value *self,
                             const char *key,
                             const double *val,
                             uintptr_t len);

void mj_value_set_list_float32(struct mj_value *self,
                               const char *key,
                               const float *val,
                               uintptr_t len);

void mj_value_set_list_bool(struct mj_value *self, const char *key, const bool *val, uintptr_t len);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  /* _MINIJINJA_H */

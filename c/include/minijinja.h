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

typedef struct mj_env {
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

struct mj_result_env_new mj_env_new(void);

struct mj_result_env_add_template mj_env_add_template(struct mj_env *env,
                                                      const char *name,
                                                      const char *source);

void mj_env_remove_template(struct mj_env *env, const char *name);

void mj_env_clear_templates(struct mj_env *env);

struct mj_result_env_render_template mj_env_render_template(struct mj_env *env,
                                                            const char *name,
                                                            const struct mj_value *value);

struct mj_result_env_render_template mj_env_render_named_string(struct mj_env *env,
                                                                const char *name,
                                                                const char *source,
                                                                const struct mj_value *value);

void mj_env_set_lstrip_blocks(struct mj_env *env, bool value);

void mj_env_set_trim_blocks(struct mj_env *env, bool value);

void mj_env_set_keep_trailing_newline(struct mj_env *env, bool value);

void mj_env_set_recursion_limit(struct mj_env *env, uintptr_t value);

void mj_env_set_debug(struct mj_env *env, bool value);

void mj_env_set_undefined_behavior(struct mj_env *env, enum mj_undefined_behavior behavior);

void mj_str_free(char *ptr);

void mj_error_free(const struct mj_error *ptr);

struct mj_value *mj_value_new(void);

void mj_value_free(struct mj_value *ptr);

void mj_value_set_string(struct mj_value *self, const char *key, const char *val);

void mj_value_set_int(struct mj_value *self, const char *key, int64_t val);

void mj_value_set_in32(struct mj_value *self, const char *key, int32_t val);

void mj_value_set_float(struct mj_value *self, const char *key, double val);

void mj_value_set_float32(struct mj_value *self, const char *key, float val);

void mj_value_set_bool(struct mj_value *self, const char *key, bool val);

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

void mj_value_set_list_bool(struct mj_value *self, const char *key, const bool *val, uintptr_t len);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  /* _MINIJINJA_H */

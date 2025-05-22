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

typedef struct mj_env {
  void *inner;
} mj_env;

typedef struct mj_result_env_new {
  struct mj_env *env;
} mj_result_env_new;

typedef struct mj_error {
  enum mj_code code;
} mj_error;

typedef struct mj_result_env_add_template {
  struct mj_error *error;
} mj_result_env_add_template;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void mj_env_free(const struct mj_env *ptr);

struct mj_result_env_new mj_env_new(void);

struct mj_result_env_add_template mj_env_add_template(struct mj_env *env,
                                                      const char *name,
                                                      const char *source);

void mj_error_free(const struct mj_error *ptr);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  /* _MINIJINJA_H */

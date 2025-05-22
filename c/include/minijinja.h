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

typedef struct mj_env {
  void *inner;
} mj_env;

typedef struct mj_result_env_new {
  struct mj_env *env;
} mj_result_env_new;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void mj_env_free(const struct mj_env *ptr);

struct mj_result_env_new mj_env_new(void);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  /* _MINIJINJA_H */

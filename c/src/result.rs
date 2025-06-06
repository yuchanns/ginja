use std::ffi::c_char;

use super::*;

/// \brief Result structure for mj_env_new function.
///
/// This structure contains the result of creating a new MiniJinja environment.
/// On success, the env field contains a pointer to the newly created environment.
///
/// @see mj_env_new Function that returns this result type
#[repr(C)]
pub struct mj_result_env_new {
    /// Pointer to the newly created environment, or NULL on failure
    pub env: *mut mj_env,
}

/// \brief Result structure for mj_env_add_template function.
///
/// This structure contains the result of adding a template to the environment.
/// On success, the error field is NULL. On failure, it contains error information.
///
/// @see mj_env_add_template Function that returns this result type
#[repr(C)]
pub struct mj_result_env_add_template {
    /// Pointer to error information, or NULL on success
    pub error: *mut mj_error,
}

/// \brief Result structure for template rendering functions.
///
/// This structure contains the result of rendering a template.
/// On success, the result field contains the rendered string and error is NULL.
/// On failure, result is NULL and error contains error information.
///
/// @see mj_env_render_template Function that returns this result type
/// @see mj_env_render_named_string Function that returns this result type
///
/// \note The result string should be freed using mj_str_free when no longer needed.
#[repr(C)]
pub struct mj_result_env_render_template {
    /// Pointer to the rendered string, or NULL on failure
    pub result: *mut c_char,
    /// Pointer to error information, or NULL on success
    pub error: *mut mj_error,
}

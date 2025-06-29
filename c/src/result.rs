use std::ffi::c_char;

use super::*;

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

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_result_env_render_template_free(
    result: *mut mj_result_env_render_template,
) {
    if result.is_null() {
        return;
    }

    unsafe {
        let res = &mut *result;

        if !res.result.is_null() {
            drop(std::ffi::CString::from_raw(res.result));
        }

        if !res.error.is_null() {
            drop(Box::from_raw(res.error));
        }
        drop(Box::from_raw(result));
    }
}

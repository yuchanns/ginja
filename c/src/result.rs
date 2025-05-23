use std::ffi::c_char;

use super::*;

#[repr(C)]
pub struct mj_result_env_new {
    pub env: *mut mj_env,
}

#[repr(C)]
pub struct mj_result_env_add_template {
    pub error: *mut mj_error,
}

#[repr(C)]
pub struct mj_result_env_render_template {
    pub result: *mut c_char,
    pub error: *mut mj_error,
}

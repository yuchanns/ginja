use super::*;

#[repr(C)]
pub struct mj_result_env_new {
    pub env: *mut mj_env,
}

#[repr(C)]
pub struct mj_result_env_add_template {
    pub error: *mut mj_error,
}

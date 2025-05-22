use super::*;

#[repr(C)]
pub struct mj_result_env_new {
    pub env: *mut mj_env,
}

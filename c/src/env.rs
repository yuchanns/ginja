use std::ffi::c_void;

use minijinja::Environment;

use super::*;

#[repr(C)]
pub struct mj_env {
    pub inner: *mut c_void,
}

impl mj_env {
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_env_free(ptr: *const mj_env) {
        unsafe {
            if ptr.is_null() {
                return;
            }
            drop(Box::from_raw((*ptr).inner as *mut Environment));
        }
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_new() -> mj_result_env_new {
    let env = Environment::new();
    mj_result_env_new {
        env: Box::into_raw(Box::new(mj_env {
            inner: Box::into_raw(Box::new(env)) as *mut c_void,
        })),
    }
}

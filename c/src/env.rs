use std::ffi::{CString, c_char, c_void};

use minijinja::Environment;

use super::*;

#[repr(C)]
pub struct mj_env {
    pub inner: *mut c_void,
}

impl mj_env {
    pub(crate) fn deref_mut(&mut self) -> &mut Environment {
        unsafe { &mut *(self.inner as *mut Environment) }
    }
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

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_add_template(
    env: *mut mj_env,
    name: *const c_char,
    source: *const c_char,
) -> mj_result_env_add_template {
    assert!(!name.is_null());
    assert!(!source.is_null());
    let name = unsafe {
        std::ffi::CStr::from_ptr(name)
            .to_str()
            .expect("malformed name")
    };
    let source = unsafe {
        std::ffi::CStr::from_ptr(source)
            .to_str()
            .expect("malformed template")
    };
    let env = unsafe { &mut *env }.deref_mut();
    match env.add_template(name, source) {
        Ok(_) => mj_result_env_add_template {
            error: std::ptr::null_mut(),
        },
        Err(e) => mj_result_env_add_template {
            error: mj_error::new(e),
        },
    }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_render_template(
    env: *mut mj_env,
    name: *const c_char,
    value: *const mj_value,
) -> mj_result_env_render_template {
    assert!(!name.is_null());
    let name = unsafe {
        std::ffi::CStr::from_ptr(name)
            .to_str()
            .expect("malformed name")
    };
    let env = unsafe { &mut *env }.deref_mut();
    let template = match env.get_template(name) {
        Ok(template) => template,
        Err(e) => {
            return mj_result_env_render_template {
                result: std::ptr::null_mut(),
                error: mj_error::new(e),
            };
        }
    };
    let value = unsafe { &*value }.deref();
    match template.render(value) {
        Ok(rendered) => mj_result_env_render_template {
            result: CString::new(rendered)
                .expect("CString::new failed")
                .into_raw(),
            error: std::ptr::null_mut(),
        },
        Err(e) => mj_result_env_render_template {
            result: std::ptr::null_mut(),
            error: mj_error::new(e),
        },
    }
}

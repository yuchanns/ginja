use std::ffi::{CString, c_char, c_void};

use minijinja::{Environment, UndefinedBehavior};

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
pub unsafe extern "C" fn mj_env_remove_template(env: *mut mj_env, name: *const c_char) {
    assert!(!name.is_null());
    let name = unsafe {
        std::ffi::CStr::from_ptr(name)
            .to_str()
            .expect("malformed name")
    };
    let env = unsafe { &mut *env }.deref_mut();
    env.remove_template(name);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_clear_templates(env: *mut mj_env) {
    let env = unsafe { &mut *env }.deref_mut();
    env.clear_templates();
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

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_render_named_string(
    env: *mut mj_env,
    name: *const c_char,
    source: *const c_char,
    value: *const mj_value,
) -> mj_result_env_render_template {
    assert!(!name.is_null());
    let name = unsafe {
        std::ffi::CStr::from_ptr(name)
            .to_str()
            .expect("malformed name")
    };
    let source = unsafe {
        std::ffi::CStr::from_ptr(source)
            .to_str()
            .expect("malformed source")
    };
    let value = unsafe { &*value }.deref();
    let env = unsafe { &mut *env }.deref_mut();
    match env.render_named_str(name, source, value) {
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

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_lstrip_blocks(env: *mut mj_env, value: bool) {
    let env = unsafe { &mut *env }.deref_mut();
    env.set_lstrip_blocks(value);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_trim_blocks(env: *mut mj_env, value: bool) {
    let env = unsafe { &mut *env }.deref_mut();
    env.set_trim_blocks(value);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_keep_trailing_newline(env: *mut mj_env, value: bool) {
    let env = unsafe { &mut *env }.deref_mut();
    env.set_keep_trailing_newline(value);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_recursion_limit(env: *mut mj_env, value: usize) {
    let env = unsafe { &mut *env }.deref_mut();
    env.set_recursion_limit(value);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_debug(env: *mut mj_env, value: bool) {
    let env = unsafe { &mut *env }.deref_mut();
    env.set_debug(value);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_undefined_behavior(
    env: *mut mj_env,
    behavior: mj_undefined_behavior,
) {
    let behavior = match behavior {
        mj_undefined_behavior::MJ_UNDEFINED_BEHAVIOR_LENIENT => UndefinedBehavior::Lenient,
        mj_undefined_behavior::MJ_UNDEFINED_BEHAVIOR_STRICT => UndefinedBehavior::Strict,
        mj_undefined_behavior::MJ_UNDEFINED_BEHAVIOR_CHAINABLE => UndefinedBehavior::Chainable,
    };
    let env = unsafe { &mut *env }.deref_mut();
    env.set_undefined_behavior(behavior);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_str_free(ptr: *mut c_char) {
    if ptr.is_null() {
        return;
    }
    unsafe {
        drop(CString::from_raw(ptr));
    }
}

use std::ffi::{CString, c_char, c_void};
use std::sync::{Arc, RwLock};

use minijinja::{Environment, UndefinedBehavior, Value};

use super::*;

/// \brief Represents a MiniJinja template environment that manages templates
/// and their rendering configuration.
///
/// @see mj_env_new This function constructs a new environment
/// @see mj_env_free This function frees the heap memory of the environment
///
/// \note The mj_env actually owns a pointer to a Arc<RwLock<minijinja::Environment>>,
/// which is inside the Rust core code and supports concurrent access.
///
/// \remark You may use the field `inner` to check whether this is a NULL
/// environment.
#[repr(C)]
pub struct mj_env {
    /// The pointer to the Arc<RwLock<minijinja::Environment>> in the Rust code.
    /// Only touch this on judging whether it is NULL.
    pub inner: *mut c_void,
}

impl mj_env {
    pub(crate) fn deref(&self) -> &Arc<RwLock<Environment>> {
        unsafe { &*(self.inner as *const Arc<RwLock<Environment>>) }
    }
}

impl mj_env {
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_env_free(ptr: *mut mj_env) {
        unsafe {
            if ptr.is_null() {
                return;
            }
            drop(Box::from_raw((*ptr).inner as *mut Arc<RwLock<Environment>>));
            drop(Box::from_raw(ptr));
        }
    }
}

/// \brief Creates a new MiniJinja template environment.
///
/// This function allocates and initializes a new MiniJinja environment
/// that can be used to manage templates and render them with context data.
///
/// @return mj_result_env_new A result structure containing the newly created
/// environment or error information if creation fails.
///
/// \note The returned environment should be freed using mj_env_free when
/// no longer needed to prevent memory leaks.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_new() -> *mut mj_env {
    let env = Environment::new();
    let env_arc = Arc::new(RwLock::new(env));
    Box::into_raw(Box::new(mj_env {
        inner: Box::into_raw(Box::new(env_arc)) as *mut c_void,
    }))
}

/// \brief Adds a template to the environment with the given name and source code.
///
/// This function compiles and stores a template in the environment so it can
/// be rendered later using mj_env_render_template.
///
/// @param env Pointer to the environment to add the template to
/// @param name Null-terminated string containing the name of the template
/// @param source Null-terminated string containing the template source code
///
/// @return mj_result_env_add_template A result structure indicating success
/// or containing error information if compilation fails.
///
/// \note Both name and source parameters must not be NULL.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_add_template(
    env: *mut mj_env,
    name: *const c_char,
    source: *const c_char,
) -> *mut mj_error {
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
    let env_arc = unsafe { &*env }.deref();
    match env_arc
        .write()
        .unwrap()
        .add_template_owned(name.to_string(), source)
    {
        Ok(_) => std::ptr::null_mut(),
        Err(e) => mj_error::new(e),
    }
}

/// \brief Removes a template from the environment by name.
///
/// This function removes a previously added template from the environment.
/// After removal, the template can no longer be rendered.
///
/// @param env Pointer to the environment to remove the template from
/// @param name Null-terminated string containing the name of the template to remove
///
/// \note The name parameter must not be NULL.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_remove_template(env: *mut mj_env, name: *const c_char) {
    assert!(!name.is_null());
    let name = unsafe {
        std::ffi::CStr::from_ptr(name)
            .to_str()
            .expect("malformed name")
    };
    let env_arc = unsafe { &*env }.deref();
    env_arc.write().unwrap().remove_template(name);
}

/// \brief Clears all templates from the environment.
///
/// This function removes all previously added templates from the environment.
/// After clearing, no templates can be rendered until new ones are added.
///
/// @param env Pointer to the environment to clear templates from
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_clear_templates(env: *mut mj_env) {
    let env_arc = unsafe { &*env }.deref();
    env_arc.write().unwrap().clear_templates();
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_render(
    env: *mut mj_env,
    name: *const c_char,
    data: *const u8,
    len: usize,
) -> *mut mj_result_env_render_template {
    assert!(!name.is_null());
    let bytes = unsafe { std::slice::from_raw_parts(data, len) };
    let name = unsafe {
        std::ffi::CStr::from_ptr(name)
            .to_str()
            .expect("malformed name")
    };
    let env_arc = unsafe { &*env }.deref();

    let env_guard = env_arc.read().unwrap();
    let template = match env_guard.get_template(name) {
        Ok(template) => template,
        Err(e) => {
            return Box::into_raw(Box::new(mj_result_env_render_template {
                result: std::ptr::null_mut(),
                error: mj_error::new(e),
            }));
        }
    };
    let value = match sonic_rs::from_slice::<Value>(bytes) {
        Ok(value) => value,
        Err(e) => {
            return Box::into_raw(Box::new(mj_result_env_render_template {
                result: std::ptr::null_mut(),
                error: Box::into_raw(Box::new(mj_error {
                    code: errors::mj_code::MJ_CANNOT_DESERIALIZE,
                    message: std::ffi::CString::new(e.to_string())
                        .expect("CString::new failed")
                        .into_raw(),
                })),
            }));
        }
    };
    match template.render(&value) {
        Ok(rendered) => Box::into_raw(Box::new(mj_result_env_render_template {
            result: CString::new(rendered)
                .expect("CString::new failed")
                .into_raw(),
            error: std::ptr::null_mut(),
        })),
        Err(e) => Box::into_raw(Box::new(mj_result_env_render_template {
            result: std::ptr::null_mut(),
            error: mj_error::new(e),
        })),
    }
}

/// \brief Renders a template from source code without storing it in the environment.
///
/// This function renders a template directly from source code using the provided
/// context value, without permanently adding it to the environment.
///
/// @param env Pointer to the environment to use for rendering
/// @param name Null-terminated string containing the name for the template (used for error reporting)
/// @param source Null-terminated string containing the template source code
/// @param value Pointer to the context value to use for rendering
///
/// @return mj_result_env_render_template A result structure containing the
/// rendered string or error information if rendering fails.
///
/// \note All parameters must not be NULL. The returned string should be
/// freed using mj_str_free when no longer needed.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_render_named_string(
    env: *mut mj_env,
    name: *const c_char,
    source: *const c_char,
    data: *const u8,
    len: usize,
) -> *mut mj_result_env_render_template {
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
    let bytes = unsafe { std::slice::from_raw_parts(data, len) };
    let env_arc = unsafe { &*env }.deref();
    let env_guard = env_arc.read().unwrap();
    let value = match sonic_rs::from_slice::<Value>(bytes) {
        Ok(value) => value,
        Err(e) => {
            return Box::into_raw(Box::new(mj_result_env_render_template {
                result: std::ptr::null_mut(),
                error: Box::into_raw(Box::new(mj_error {
                    code: errors::mj_code::MJ_CANNOT_DESERIALIZE,
                    message: std::ffi::CString::new(e.to_string())
                        .expect("CString::new failed")
                        .into_raw(),
                })),
            }));
        }
    };
    match env_guard.render_named_str(name, source, &value) {
        Ok(rendered) => Box::into_raw(Box::new(mj_result_env_render_template {
            result: CString::new(rendered)
                .expect("CString::new failed")
                .into_raw(),
            error: std::ptr::null_mut(),
        })),
        Err(e) => Box::into_raw(Box::new(mj_result_env_render_template {
            result: std::ptr::null_mut(),
            error: mj_error::new(e),
        })),
    }
}

/// \brief Sets whether to strip leading whitespace from blocks.
///
/// This function configures the environment to automatically strip leading
/// whitespace from template blocks when enabled.
///
/// @param env Pointer to the environment to configure
/// @param value Boolean value indicating whether to enable lstrip blocks
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_lstrip_blocks(env: *mut mj_env, value: bool) {
    let env_arc = unsafe { &*env }.deref();
    env_arc.write().unwrap().set_lstrip_blocks(value);
}

/// \brief Sets whether to strip trailing whitespace from blocks.
///
/// This function configures the environment to automatically strip trailing
/// whitespace from template blocks when enabled.
///
/// @param env Pointer to the environment to configure
/// @param value Boolean value indicating whether to enable trim blocks
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_trim_blocks(env: *mut mj_env, value: bool) {
    let env_arc = unsafe { &*env }.deref();
    env_arc.write().unwrap().set_trim_blocks(value);
}

/// \brief Sets whether to keep trailing newlines in template output.
///
/// This function configures the environment to keep or remove trailing
/// newlines in the rendered template output.
///
/// @param env Pointer to the environment to configure
/// @param value Boolean value indicating whether to keep trailing newlines
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_keep_trailing_newline(env: *mut mj_env, value: bool) {
    let env_arc = unsafe { &*env }.deref();
    env_arc.write().unwrap().set_keep_trailing_newline(value);
}

/// \brief Sets the maximum recursion depth for template rendering.
///
/// This function configures the maximum depth of recursive template calls
/// to prevent infinite recursion and stack overflow.
///
/// @param env Pointer to the environment to configure
/// @param value Maximum recursion depth allowed
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_recursion_limit(env: *mut mj_env, value: usize) {
    let env_arc = unsafe { &*env }.deref();
    env_arc.write().unwrap().set_recursion_limit(value);
}

/// \brief Sets whether to enable debug mode for template rendering.
///
/// This function configures the environment to enable or disable debug mode,
/// which affects error reporting and template debugging capabilities.
///
/// @param env Pointer to the environment to configure
/// @param value Boolean value indicating whether to enable debug mode
#[unsafe(no_mangle)]
pub unsafe extern "C" fn mj_env_set_debug(env: *mut mj_env, value: bool) {
    let env_arc = unsafe { &*env }.deref();
    env_arc.write().unwrap().set_debug(value);
}

/// \brief Sets the undefined behavior policy for the environment.
///
/// This function configures how the environment handles undefined variables
/// and expressions in templates.
///
/// @param env Pointer to the environment to configure
/// @param behavior The undefined behavior policy to set
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
    let env_arc = unsafe { &*env }.deref();
    env_arc.write().unwrap().set_undefined_behavior(behavior);
}

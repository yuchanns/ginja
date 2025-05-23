use std::{
    collections::HashMap,
    ffi::{c_char, c_void},
};

use minijinja::Value;

#[repr(C)]
pub struct mj_value {
    inner: *mut c_void,
}

impl mj_value {
    pub(crate) fn deref(&self) -> &HashMap<String, Value> {
        unsafe { &*(self.inner as *mut HashMap<_, _>) }
    }

    pub(crate) fn deref_mut(&mut self) -> &mut HashMap<String, Value> {
        unsafe { &mut *(self.inner as *mut HashMap<_, _>) }
    }
}

impl mj_value {
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_new() -> *mut Self {
        let map: HashMap<String, Value> = HashMap::new();
        let value = Self {
            inner: Box::into_raw(Box::new(map)) as _,
        };
        Box::into_raw(Box::new(value))
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_free(ptr: *mut mj_value) {
        unsafe {
            if ptr.is_null() {
                return;
            }
            drop(Box::from_raw((*ptr).inner as *mut HashMap<String, Value>));
        }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_string(
        &mut self,
        key: *const c_char,
        val: *const c_char,
    ) {
        assert!(!key.is_null());
        assert!(!val.is_null());
        let key = unsafe {
            std::ffi::CStr::from_ptr(key)
                .to_str()
                .expect("malformed key")
        };
        let val = unsafe {
            std::ffi::CStr::from_ptr(val)
                .to_str()
                .expect("malformed value")
        };
        self.deref_mut().insert(key.into(), val.into());
    }
}

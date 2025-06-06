use std::{
    collections::HashMap,
    ffi::{c_char, c_void},
};

use minijinja::Value;

#[repr(C)]
pub struct mj_value {
    pub(crate) inner: *mut c_void,
}

impl mj_value {
    pub(crate) fn deref(&self) -> &HashMap<String, Value> {
        unsafe { &*(self.inner as *mut HashMap<_, _>) }
    }

    pub(crate) fn deref_mut(&mut self) -> &mut HashMap<String, Value> {
        unsafe { &mut *(self.inner as *mut HashMap<_, _>) }
    }

    unsafe fn set<T>(&mut self, key: *const c_char, val: T)
    where
        T: Clone + Into<Value>,
    {
        assert!(!key.is_null());
        let key = unsafe {
            std::ffi::CStr::from_ptr(key)
                .to_str()
                .expect("malformed key")
        };
        self.deref_mut().insert(key.into(), val.into());
    }

    unsafe fn set_list<T>(&mut self, key: *const c_char, val: *const T, len: usize)
    where
        T: Clone + Into<Value>,
    {
        assert!(!val.is_null());
        let list: Vec<Value> = (0..len)
            .map(|i| {
                let item = unsafe { (*val.add(i)).clone() };
                item.into()
            })
            .collect();
        unsafe { self.set(key, list) }
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
            drop(Box::from_raw(ptr));
        }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_value(
        &mut self,
        key: *const c_char,
        val: *const mj_value,
    ) {
        assert!(!val.is_null());
        let val = unsafe { &*val }.deref().clone();
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_string(
        &mut self,
        key: *const c_char,
        val: *const c_char,
    ) {
        assert!(!val.is_null());
        let val = unsafe {
            std::ffi::CStr::from_ptr(val)
                .to_str()
                .expect("malformed value")
        };
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_int(&mut self, key: *const c_char, val: i64) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_int32(&mut self, key: *const c_char, val: i32) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_int16(&mut self, key: *const c_char, val: i16) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_int8(&mut self, key: *const c_char, val: i8) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_uint(&mut self, key: *const c_char, val: u64) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_uint32(&mut self, key: *const c_char, val: u32) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_uint16(&mut self, key: *const c_char, val: u16) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_uint8(&mut self, key: *const c_char, val: u8) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_float(&mut self, key: *const c_char, val: f64) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_float32(&mut self, key: *const c_char, val: f32) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_bool(&mut self, key: *const c_char, val: bool) {
        unsafe { self.set(key, val) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_value(
        &mut self,
        key: *const c_char,
        val: *const *const mj_value,
        len: usize,
    ) {
        assert!(!val.is_null());
        let list: Vec<Value> = (0..len)
            .map(|i| {
                let item = unsafe { &*(*val.add(i)) }.deref().clone();
                item.into()
            })
            .collect();
        unsafe { self.set(key, list) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_string(
        &mut self,
        key: *const c_char,
        val: *const *const c_char,
        len: usize,
    ) {
        assert!(!val.is_null());
        let list: Vec<String> = (0..len)
            .map(|i| {
                let item = unsafe { std::ffi::CStr::from_ptr(*val.add(i)) }
                    .to_str()
                    .expect("malformed value");
                item.into()
            })
            .collect();
        unsafe { self.set(key, list) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_int(
        &mut self,
        key: *const c_char,
        val: *const i64,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_int32(
        &mut self,
        key: *const c_char,
        val: *const i32,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_int16(
        &mut self,
        key: *const c_char,
        val: *const i16,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_int8(
        &mut self,
        key: *const c_char,
        val: *const i8,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_uint(
        &mut self,
        key: *const c_char,
        val: *const u64,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_uint32(
        &mut self,
        key: *const c_char,
        val: *const u32,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_uint16(
        &mut self,
        key: *const c_char,
        val: *const u16,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_uint8(
        &mut self,
        key: *const c_char,
        val: *const u8,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_float(
        &mut self,
        key: *const c_char,
        val: *const f64,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_float32(
        &mut self,
        key: *const c_char,
        val: *const f32,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_bool(
        &mut self,
        key: *const c_char,
        val: *const bool,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }
}

#[repr(C)]
pub enum mj_undefined_behavior {
    /// The default, somewhat lenient undefined behavior.
    MJ_UNDEFINED_BEHAVIOR_LENIENT,
    /// Complains very quickly about undefined values.
    MJ_UNDEFINED_BEHAVIOR_STRICT,
    /// Like Lenient, but also allows chaining of undefined lookups.
    MJ_UNDEFINED_BEHAVIOR_CHAINABLE,
}

use std::{
    collections::HashMap,
    ffi::{c_char, c_void},
};

use minijinja::Value;

/// Internal enum to represent different value container types
#[derive(Clone)]
pub(crate) enum ValueContainer {
    Map(HashMap<String, Value>),
    List(Vec<Value>),
}

/// \brief Represents a MiniJinja value that can hold various data types
/// including strings, numbers, booleans, objects, and arrays.
///
/// This structure is used to pass context data to template rendering functions.
/// It internally manages either a HashMap of key-value pairs or a Vec of values
/// where keys are strings and values can be of various types supported by MiniJinja.
///
/// @see mj_value_new Creates a new empty value
/// @see mj_value_free Frees the memory allocated for the value
///
/// \note The mj_value actually owns a pointer to a ValueContainer,
/// which is inside the Rust core code.
///
/// \remark You may use the field `inner` to check whether this is a NULL
/// value, but should not modify it directly.
#[repr(C)]
pub struct mj_value {
    /// The pointer to the ValueContainer in the Rust code.
    /// Only touch this for checking whether it is NULL.
    pub(crate) inner: *mut c_void,
}

impl From<ValueContainer> for Value {
    fn from(container: ValueContainer) -> Self {
        match container {
            ValueContainer::Map(map) => Value::from(map),
            ValueContainer::List(list) => Value::from(list),
        }
    }
}

impl mj_value {
    pub(crate) fn deref(&self) -> &ValueContainer {
        unsafe { &*(self.inner as *mut ValueContainer) }
    }

    pub(crate) fn deref_mut(&mut self) -> &mut ValueContainer {
        unsafe { &mut *(self.inner as *mut ValueContainer) }
    }

    pub(crate) fn deref_as_map(&self) -> Option<&HashMap<String, Value>> {
        match self.deref() {
            ValueContainer::Map(map) => Some(map),
            ValueContainer::List(_) => None,
        }
    }

    pub(crate) fn deref_as_map_mut(&mut self) -> Option<&mut HashMap<String, Value>> {
        match self.deref_mut() {
            ValueContainer::Map(map) => Some(map),
            ValueContainer::List(_) => None,
        }
    }

    pub(crate) fn deref_as_list_mut(&mut self) -> Option<&mut Vec<Value>> {
        match self.deref_mut() {
            ValueContainer::List(list) => Some(list),
            ValueContainer::Map(_) => None,
        }
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
        self.deref_as_map_mut()
            .unwrap()
            .insert(key.into(), val.into());
    }

    unsafe fn append<T>(&mut self, val: T)
    where
        T: Clone + Into<Value>,
    {
        let list = self.deref_as_list_mut().unwrap();
        list.push(val.into());
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
    /// \brief Creates a new empty MiniJinja value.
    ///
    /// This function allocates and initializes a new MiniJinja value that can be used
    /// to store context data for template rendering. The value starts as an empty
    /// HashMap and can be populated using the various mj_value_set_* functions.
    ///
    /// @return Pointer to the newly created mj_value structure
    ///
    /// \note The returned value should be freed using mj_value_free when
    /// no longer needed to prevent memory leaks.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_new() -> *mut Self {
        let map: HashMap<String, Value> = HashMap::new();
        let value = Self {
            inner: Box::into_raw(Box::new(ValueContainer::Map(map))) as _,
        };
        Box::into_raw(Box::new(value))
    }

    /// \brief Creates a new empty MiniJinja value that can hold a list.
    ///
    /// This function allocates and initializes a new MiniJinja value that is
    /// specifically designed to hold a list of values. The value starts as an empty
    /// Vec<Value> and can be populated using the various mj_value_set_list_* functions.
    ///
    /// @return Pointer to the newly created mj_value structure
    ///
    /// \note The returned value should be freed using mj_value_free when
    /// no longer needed to prevent memory leaks.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_new_list() -> *mut Self {
        let list: Vec<Value> = Vec::new();
        let value = Self {
            inner: Box::into_raw(Box::new(ValueContainer::List(list))) as _,
        };
        Box::into_raw(Box::new(value))
    }

    /// \brief Frees the memory allocated for a MiniJinja value.
    ///
    /// This function properly deallocates the memory used by a MiniJinja value
    /// that was created with mj_value_new. After calling this function,
    /// the pointer should not be used anymore.
    ///
    /// @param ptr Pointer to the mj_value structure to free
    ///
    /// \note It is safe to pass NULL to this function.
    /// \note Only use this function on values created by mj_value_new.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_free(ptr: *mut mj_value) {
        unsafe {
            if ptr.is_null() {
                return;
            }
            drop(Box::from_raw((*ptr).inner as *mut ValueContainer));
            drop(Box::from_raw(ptr));
        }
    }

    /// \brief Sets a mj_value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is another mj_value structure. This allows for nested
    /// data structures.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to the mj_value to set as the value
    ///
    /// \note Both key and val parameters must not be NULL.
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

    /// \brief Sets a string value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a string.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Null-terminated string containing the value to set
    ///
    /// \note Both key and val parameters must not be NULL.
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

    /// \brief Sets a 64-bit signed integer value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a 64-bit signed integer.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The integer value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_int(&mut self, key: *const c_char, val: i64) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets a 32-bit signed integer value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a 32-bit signed integer.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The 32-bit integer value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_int32(&mut self, key: *const c_char, val: i32) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets a 16-bit signed integer value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a 16-bit signed integer.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The 16-bit integer value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_int16(&mut self, key: *const c_char, val: i16) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets an 8-bit signed integer value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an 8-bit signed integer.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The 8-bit integer value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_int8(&mut self, key: *const c_char, val: i8) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets a 64-bit unsigned integer value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a 64-bit unsigned integer.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The 64-bit unsigned integer value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_uint(&mut self, key: *const c_char, val: u64) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets a 32-bit unsigned integer value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a 32-bit unsigned integer.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The 32-bit unsigned integer value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_uint32(&mut self, key: *const c_char, val: u32) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets a 16-bit unsigned integer value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a 16-bit unsigned integer.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The 16-bit unsigned integer value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_uint16(&mut self, key: *const c_char, val: u16) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets an 8-bit unsigned integer value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an 8-bit unsigned integer.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The 8-bit unsigned integer value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_uint8(&mut self, key: *const c_char, val: u8) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets a 64-bit floating-point value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a 64-bit floating-point number.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The floating-point value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_float(&mut self, key: *const c_char, val: f64) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets a 32-bit floating-point value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a 32-bit floating-point number.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The 32-bit floating-point value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_float32(&mut self, key: *const c_char, val: f32) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets a boolean value as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is a boolean (true or false).
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val The boolean value to set
    ///
    /// \note The key parameter must not be NULL.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_bool(&mut self, key: *const c_char, val: bool) {
        unsafe { self.set(key, val) }
    }

    /// \brief Sets an array of mj_value pointers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of mj_value structures. This allows for complex
    /// nested data structures and arrays of objects.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of mj_value pointers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len mj_value pointers.
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

    /// \brief Sets an array of string pointers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of null-terminated strings.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of null-terminated string pointers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len string pointers.
    /// \note All strings in the array must be valid null-terminated strings.
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

    /// \brief Sets an array of 64-bit signed integers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 64-bit signed integers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 64-bit signed integers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len integers.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_int(
        &mut self,
        key: *const c_char,
        val: *const i64,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of 32-bit signed integers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 32-bit signed integers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 32-bit signed integers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len integers.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_int32(
        &mut self,
        key: *const c_char,
        val: *const i32,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of 16-bit signed integers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 16-bit signed integers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 16-bit signed integers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len integers.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_int16(
        &mut self,
        key: *const c_char,
        val: *const i16,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of 8-bit signed integers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 8-bit signed integers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 8-bit signed integers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len integers.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_int8(
        &mut self,
        key: *const c_char,
        val: *const i8,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of 64-bit unsigned integers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 64-bit unsigned integers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 64-bit unsigned integers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len unsigned integers.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_uint(
        &mut self,
        key: *const c_char,
        val: *const u64,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of 32-bit unsigned integers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 32-bit unsigned integers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 32-bit unsigned integers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len unsigned integers.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_uint32(
        &mut self,
        key: *const c_char,
        val: *const u32,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of 16-bit unsigned integers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 16-bit unsigned integers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 16-bit unsigned integers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len unsigned integers.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_uint16(
        &mut self,
        key: *const c_char,
        val: *const u16,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of 8-bit unsigned integers as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 8-bit unsigned integers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 8-bit unsigned integers
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len unsigned integers.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_uint8(
        &mut self,
        key: *const c_char,
        val: *const u8,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of 64-bit floating-point values as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 64-bit floating-point numbers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 64-bit floating-point values
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len floating-point values.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_float(
        &mut self,
        key: *const c_char,
        val: *const f64,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of 32-bit floating-point values as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of 32-bit floating-point numbers.
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of 32-bit floating-point values
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len floating-point values.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_float32(
        &mut self,
        key: *const c_char,
        val: *const f32,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Sets an array of boolean values as a field in the value map.
    ///
    /// This function adds or updates a key-value pair in the value map where
    /// the value is an array of boolean values (true/false).
    ///
    /// @param key Null-terminated string containing the key name
    /// @param val Pointer to an array of boolean values
    /// @param len Number of elements in the array
    ///
    /// \note Both key and val parameters must not be NULL.
    /// \note The val parameter should point to an array of len boolean values.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_set_list_bool(
        &mut self,
        key: *const c_char,
        val: *const bool,
        len: usize,
    ) {
        unsafe { self.set_list(key, val, len) }
    }

    /// \brief Appends a value to a list in the mj_value.
    ///
    /// This function appends a single mj_value to the end of a list contained
    /// within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val Pointer to the mj_value to append
    ///
    /// \note The val parameter must not be NULL.
    /// \remark This function is unsafe because it dereferences the pointer
    /// and assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_value(&mut self, val: *const mj_value) {
        assert!(!val.is_null());
        let val = unsafe { &*val }.deref().clone();
        self.deref_as_list_mut().unwrap().push(val.into());
    }

    /// \brief Appends a string value to a list in the mj_value.
    ///
    /// This function appends a single string to the end of a list contained
    /// within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val Null-terminated string to append
    ///
    /// \note The val parameter must not be NULL.
    /// \remark This function is unsafe because it dereferences the pointer
    /// and assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_string(&mut self, val: *const c_char) {
        assert!(!val.is_null());
        let val = unsafe {
            std::ffi::CStr::from_ptr(val)
                .to_str()
                .expect("malformed value")
        };
        unsafe { self.append(val) }
    }

    /// \brief Appends a 64-bit signed integer value to a list in the mj_value.
    ///
    /// This function appends a single 64-bit signed integer to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The integer value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_int(&mut self, val: i64) {
        unsafe { self.append(val) }
    }

    /// \brief Appends a 32-bit signed integer value to a list in the mj_value.
    ///
    /// This function appends a single 32-bit signed integer to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The 32-bit integer value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_int32(&mut self, val: i32) {
        unsafe { self.append(val) }
    }

    /// \brief Appends a 16-bit signed integer value to a list in the mj_value.
    ///
    /// This function appends a single 16-bit signed integer to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The 16-bit integer value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_int16(&mut self, val: i16) {
        unsafe { self.append(val) }
    }

    /// \brief Appends an 8-bit signed integer value to a list in the mj_value.
    ///
    /// This function appends a single 8-bit signed integer to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The 8-bit integer value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_int8(&mut self, val: i8) {
        unsafe { self.append(val) }
    }

    /// \brief Appends a 64-bit unsigned integer value to a list in the mj_value.
    ///
    /// This function appends a single 64-bit unsigned integer to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The 64-bit unsigned integer value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_uint(&mut self, val: u64) {
        unsafe { self.append(val) }
    }

    /// \brief Appends a 32-bit unsigned integer value to a list in the mj_value.
    ///
    /// This function appends a single 32-bit unsigned integer to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The 32-bit unsigned integer value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_uint32(&mut self, val: u32) {
        unsafe { self.append(val) }
    }

    /// \brief Appends a 16-bit unsigned integer value to a list in the mj_value.
    ///
    /// This function appends a single 16-bit unsigned integer to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The 16-bit unsigned integer value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_uint16(&mut self, val: u16) {
        unsafe { self.append(val) }
    }

    /// \brief Appends an 8-bit unsigned integer value to a list in the mj_value.
    ///
    /// This function appends a single 8-bit unsigned integer to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The 8-bit unsigned integer value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_uint8(&mut self, val: u8) {
        unsafe { self.append(val) }
    }

    /// \brief Appends a 64-bit floating-point value to a list in the mj_value.
    ///
    /// This function appends a single 64-bit floating-point number to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The floating-point value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_float(&mut self, val: f64) {
        unsafe { self.append(val) }
    }

    /// \brief Appends a 32-bit floating-point value to a list in the mj_value.
    ///
    /// This function appends a single 32-bit floating-point number to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The 32-bit floating-point value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_float32(&mut self, val: f32) {
        unsafe { self.append(val) }
    }

    /// \brief Appends a boolean value to a list in the mj_value.
    ///
    /// This function appends a single boolean value to the end of a list
    /// contained within the mj_value. If the mj_value does not contain a list,
    /// it will panic.
    ///
    /// @param val The boolean value to append
    ///
    /// \remark This function is unsafe because it assumes that the mj_value contains a list.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_value_append_bool(&mut self, val: bool) {
        unsafe { self.append(val) }
    }
}

/// \brief Defines how the template environment handles undefined variables and expressions.
///
/// This enumeration controls the behavior when undefined variables or expressions
/// are encountered during template rendering.
///
/// @see mj_env_set_undefined_behavior Function to set the undefined behavior policy
#[repr(C)]
pub enum mj_undefined_behavior {
    /// The default, somewhat lenient undefined behavior.
    /// Undefined variables are rendered as empty strings without errors.
    MJ_UNDEFINED_BEHAVIOR_LENIENT,
    /// Complains very quickly about undefined values.
    /// Undefined variables cause rendering errors immediately.
    MJ_UNDEFINED_BEHAVIOR_STRICT,
    /// Like Lenient, but also allows chaining of undefined lookups.
    /// Allows operations like `{{ undefined.foo.bar }}` without errors.
    MJ_UNDEFINED_BEHAVIOR_CHAINABLE,
}

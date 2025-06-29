use std::ffi::{CString, c_char};

use minijinja::{Error, ErrorKind};

/// \brief Represents error codes for different types of MiniJinja errors.
///
/// This enumeration defines all possible error conditions that can occur
/// during template compilation, rendering, and value operations in MiniJinja.
/// Each error code corresponds to a specific error kind from the core MiniJinja library.
///
/// @see mj_error This structure contains both the error code and message
///
/// \note These error codes are C-compatible and can be used for error handling
/// in C/C++ applications.
#[repr(C)]
pub enum mj_code {
    /// Error when attempting to use a non-primitive value where a primitive is expected
    MJ_NON_PRIMITIVE,
    /// Error when attempting to use a non-key value as a map key
    MJ_NON_KEY,
    /// Error when attempting an invalid operation on a value
    MJ_INVALID_OPERATION,
    /// Syntax error in template source code
    MJ_SYNTAX_ERROR,
    /// Template with the specified name was not found
    MJ_TEMPLATE_NOT_FOUND,
    /// Too many arguments provided to a function, filter, or test
    MJ_TOO_MANY_ARGUMENTS,
    /// Required argument is missing from a function, filter, or test call
    MJ_MISSING_ARGUMENT,
    /// Unknown or undefined filter used in template
    MJ_UNKNOWN_FILTER,
    /// Unknown or undefined test used in template
    MJ_UNKNOWN_TEST,
    /// Unknown or undefined function used in template
    MJ_UNKNOWN_FUNCTION,
    /// Unknown or undefined method called on a value
    MJ_UNKNOWN_METHOD,
    /// Invalid escape sequence in template
    MJ_BAD_ESCAPE,
    /// Undefined variable or expression accessed in template
    MJ_UNDEFINED_ERROR,
    /// Error during value serialization
    MJ_BAD_SERIALIZATION,
    /// Error during value deserialization
    MJ_CANNOT_DESERIALIZE,
    /// Error in template include operation
    MJ_BAD_INCLUDE,
    /// Error in template block evaluation
    MJ_EVAL_BLOCK,
    /// Error when attempting to unpack a value
    MJ_CANNOT_UNPACK,
    /// Error during template output writing
    MJ_WRITE_FAILURE,
    /// Unknown or undefined block used in template
    MJ_UNKNOWN_BLOCK,
}

impl From<ErrorKind> for mj_code {
    fn from(kind: ErrorKind) -> Self {
        match kind {
            ErrorKind::NonPrimitive => mj_code::MJ_NON_PRIMITIVE,
            ErrorKind::NonKey => mj_code::MJ_NON_KEY,
            ErrorKind::InvalidOperation => mj_code::MJ_INVALID_OPERATION,
            ErrorKind::SyntaxError => mj_code::MJ_SYNTAX_ERROR,
            ErrorKind::TemplateNotFound => mj_code::MJ_TEMPLATE_NOT_FOUND,
            ErrorKind::TooManyArguments => mj_code::MJ_TOO_MANY_ARGUMENTS,
            ErrorKind::MissingArgument => mj_code::MJ_MISSING_ARGUMENT,
            ErrorKind::UnknownFilter => mj_code::MJ_UNKNOWN_FILTER,
            ErrorKind::UnknownTest => mj_code::MJ_UNKNOWN_TEST,
            ErrorKind::UnknownFunction => mj_code::MJ_UNKNOWN_FUNCTION,
            ErrorKind::UnknownMethod => mj_code::MJ_UNKNOWN_METHOD,
            ErrorKind::BadEscape => mj_code::MJ_BAD_ESCAPE,
            ErrorKind::UndefinedError => mj_code::MJ_UNDEFINED_ERROR,
            ErrorKind::BadSerialization => mj_code::MJ_BAD_SERIALIZATION,
            ErrorKind::CannotDeserialize => mj_code::MJ_CANNOT_DESERIALIZE,
            ErrorKind::BadInclude => mj_code::MJ_BAD_INCLUDE,
            ErrorKind::EvalBlock => mj_code::MJ_EVAL_BLOCK,
            ErrorKind::CannotUnpack => mj_code::MJ_CANNOT_UNPACK,
            ErrorKind::WriteFailure => mj_code::MJ_WRITE_FAILURE,
            ErrorKind::UnknownBlock => mj_code::MJ_UNKNOWN_BLOCK,
            _ => unimplemented!(
                "The newly added ErrorKind in core crate is not handled in C bindings"
            ),
        }
    }
}

/// \brief Represents a MiniJinja error with code and message information.
///
/// This structure encapsulates error information from MiniJinja operations,
/// including both an error code that categorizes the error type and a
/// human-readable error message with full error chain details.
///
/// @see mj_code The error code enumeration
/// @see mj_error_free This function frees the heap memory of the error
///
/// \note The mj_error structure owns the error message string, which must
/// be freed using mj_error_free to prevent memory leaks.
///
/// \remark The message field contains the complete error chain, including
/// the root cause and all intermediate error causes.
#[repr(C)]
pub struct mj_error {
    /// The error code categorizing the type of error that occurred
    pub(crate) code: mj_code,
    /// Pointer to a null-terminated C string containing the error message
    pub(crate) message: *const c_char,
}

impl mj_error {
    pub fn new(error: Error) -> *mut Self {
        let code = mj_code::from(error.kind());
        let mut err = &error as &dyn std::error::Error;
        let mut message = err.to_string();
        while let Some(cause) = err.source() {
            message.push_str("\nCaused by: ");
            message.push_str(cause.to_string().as_str());
            err = cause;
        }
        Box::into_raw(Box::new(mj_error {
            code,
            message: CString::new(message)
                .expect("CString::new failed")
                .into_raw(),
        }))
    }

    /// \brief Frees the memory allocated for a MiniJinja error.
    ///
    /// This function properly deallocates both the error structure and its
    /// associated error message string, preventing memory leaks.
    ///
    /// @param ptr Pointer to the error structure to free
    ///
    /// \note It is safe to pass NULL to this function.
    /// \note Only use this function on error pointers returned by MiniJinja C API functions.
    /// \note After calling this function, the pointer becomes invalid and should not be used.
    #[unsafe(no_mangle)]
    pub unsafe extern "C" fn mj_error_free(ptr: *mut mj_error) {
        if ptr.is_null() {
            return;
        }
        unsafe {
            drop(Box::from_raw((*ptr).message as *mut c_char));
            drop(Box::from_raw(ptr));
        }
    }
}

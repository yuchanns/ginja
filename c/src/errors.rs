use std::ffi::{CString, c_char};

use minijinja::{Error, ErrorKind};

#[repr(C)]
pub enum mj_code {
    MJ_NON_PRIMITIVE,
    MJ_NON_KEY,
    MJ_INVALID_OPERATION,
    MJ_SYNTAX_ERROR,
    MJ_TEMPLATE_NOT_FOUND,
    MJ_TOO_MANY_ARGUMENTS,
    MJ_MISSING_ARGUMENT,
    MJ_UNKNOWN_FILTER,
    MJ_UNKNOWN_TEST,
    MJ_UNKNOWN_FUNCTION,
    MJ_UNKNOWN_METHOD,
    MJ_BAD_ESCAPE,
    MJ_UNDEFINED_ERROR,
    MJ_BAD_SERIALIZATION,
    MJ_CANNOT_DESERIALIZE,
    MJ_BAD_INCLUDE,
    MJ_EVAL_BLOCK,
    MJ_CANNOT_UNPACK,
    MJ_WRITE_FAILURE,
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

#[repr(C)]
pub struct mj_error {
    code: mj_code,
    message: *const c_char,
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

// This crate is a C binding for the minijinja project.
// So it's type node can't meet camel case.
#![allow(non_camel_case_types)]
// This crate is a C binding for the minijinja project.
// Nearly all the functions exposed to C FFI are unsafe.
#![allow(clippy::missing_safety_doc)]

mod env;
mod errors;
mod result;
mod types;

pub use result::mj_result_env_render_template;

pub use env::mj_env;
pub use errors::mj_error;

pub use types::mj_undefined_behavior;

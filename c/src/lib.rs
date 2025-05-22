// This crate is a C binding for the minijinja project.
// So it's type node can't meet camel case.
#![allow(non_camel_case_types)]
// This crate is a C binding for the minijinja project.
// Nearly all the functions exposed to C FFI are unsafe.
#![allow(clippy::missing_safety_doc)]

mod result;

pub use result::mj_result_env_new;

pub mod env;

pub use env::mj_env;

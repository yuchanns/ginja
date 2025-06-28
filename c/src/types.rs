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

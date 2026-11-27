// Generated from doc/user/module_declaration.lark.
constexpr char ModuleListSeparator = ',';
constexpr char ModuleAttributesMarker = ':';
constexpr char ModuleAssignment = '=';
constexpr char ModuleParametersBegin = '(';
constexpr char ModuleParametersEnd = ')';

enum class ModuleSuffix { None, Parameters, Alias };

constexpr ModuleSuffix moduleSuffix(char marker)
{
    switch (marker) {
    case ModuleParametersBegin: return ModuleSuffix::Parameters;
    case ModuleAssignment: return ModuleSuffix::Alias;
    default: return ModuleSuffix::None;
    }
}

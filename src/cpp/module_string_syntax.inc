// Generated from doc/user/module_string.lark by test/check_module_string_grammar.py.
    { // Production
        {ModuleSyntax::Bare, ModuleSyntax::WithArguments, ModuleSyntax::Bare},
        {ModuleSyntax::Bare, ModuleSyntax::WithArguments, ModuleSyntax::Variable},
        {ModuleSyntax::Bare, ModuleSyntax::WithArguments, ModuleSyntax::Surface},
    },
    { // Pattern
        {ModuleSyntax::Bare, ModuleSyntax::WithArguments, ModuleSyntax::Bare},
        {ModuleSyntax::Bare, ModuleSyntax::WithArguments, ModuleSyntax::Capture},
        {ModuleSyntax::Bare, ModuleSyntax::WithArguments, ModuleSyntax::Surface},
    },

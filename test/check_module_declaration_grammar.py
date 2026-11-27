"""Check the generated module declaration syntax and the tracked LPy corpus."""

import argparse
import json
from pathlib import Path
import re
import subprocess
import sys

from lark import Lark, UnexpectedInput


ROOT = Path(__file__).resolve().parents[1]
GRAMMAR = ROOT / "doc/user/module_declaration.lark"
TABLE = ROOT / "src/cpp/module_declaration_syntax.inc"
DECLARATION = re.compile(r"^[ \t]*module(?=[ \t(]|$)(.*)$")
PUNCTUATION = {
    "COMMA": "ModuleListSeparator",
    "COLON": "ModuleAttributesMarker",
    "EQUAL": "ModuleAssignment",
    "OPEN_PAREN": "ModuleParametersBegin",
    "CLOSE_PAREN": "ModuleParametersEnd",
}


def generate_syntax(grammar):
    lines = ["// Generated from doc/user/module_declaration.lark."]
    terminals = {terminal.name: terminal.pattern.value for terminal in grammar.terminals}
    for terminal, name in PUNCTUATION.items():
        literal = terminals[terminal]
        if len(literal) != 1:
            raise ValueError(f"{terminal} must be a single character")
        value = json.dumps(literal)[1:-1]
        if value == "'":
            value = r"\'"
        lines.append(f"constexpr char {name} = '{value}';")
    suffixes = [rule.expansion[0].name for rule in grammar.rules if rule.origin.name == "suffix"]
    starts = {
        rule.origin.name: PUNCTUATION[rule.expansion[0].name]
        for rule in grammar.rules if rule.origin.name in suffixes
    }
    lines.extend([
        "",
        "enum class ModuleSuffix { None, " + ", ".join(name.capitalize() for name in suffixes) + " };",
        "",
        "constexpr ModuleSuffix moduleSuffix(char marker)",
        "{",
        "    switch (marker) {",
    ])
    lines.extend(f"    case {starts[name]}: return ModuleSuffix::{name.capitalize()};" for name in suffixes)
    lines.extend(["    default: return ModuleSuffix::None;", "    }", "}"])
    return "\n".join(lines) + "\n"


def check_corpus(grammar):
    sources = subprocess.check_output(
        ["git", "ls-files", "-z", "*.lpy"], cwd=ROOT
    ).decode().split("\0")[:-1]
    accepted = rejected = 0
    for name in sources:
        source = (ROOT / name).read_text(encoding="utf-8-sig", errors="surrogateescape")
        for lineno, line in enumerate(source.splitlines(), 1):
            match = DECLARATION.match(line)
            if match is None:
                continue
            try:
                grammar.parse(match[1])
                accepted += 1
            except UnexpectedInput as error:
                print(f"{name}:{lineno}: {error}", file=sys.stderr)
                rejected += 1
    print(f"{accepted} module declaration lines accepted, {rejected} rejected in {len(sources)} files")
    return int(rejected != 0 or accepted == 0)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write", action="store_true", help="Regenerate the C++ syntax table.")
    options = parser.parse_args()
    grammar = Lark.open(GRAMMAR, parser="earley", keep_all_tokens=True)
    syntax = generate_syntax(grammar)
    if options.write:
        TABLE.write_text(syntax, encoding="utf-8")
    elif TABLE.read_text(encoding="utf-8") != syntax:
        parser.exit(1, "Module declaration syntax is stale. Run this script with --write.\n")
    sys.exit(check_corpus(grammar))


if __name__ == "__main__":
    main()

"""Check the C++ directive table generated from the Lsystem directive grammar."""

import argparse
import json
from pathlib import Path

from lark import Lark


ROOT = Path(__file__).resolve().parents[1]
GRAMMAR = ROOT / "doc/user/lsystem_directives.lark"
TABLE = ROOT / "src/cpp/lsystem_directives.inc"


def cpp_name(name):
    return "".join(word.capitalize() for word in name.split("_"))


def generate_table(grammar):
    """Map each directive literal to the syntax of its payload."""
    literals = {terminal.name: terminal.pattern.value for terminal in grammar.terminals}
    lines = ["// Generated from doc/user/lsystem_directives.lark by test/check_directive_grammar.py."]
    for rule in grammar.rules:
        if rule.origin.name != "directive":
            continue
        keyword, payload = rule.expansion
        lines.append(
            f"LPY_DIRECTIVE({cpp_name(rule.alias)}, {json.dumps(literals[keyword.name])}, "
            f"{cpp_name(payload.name)})"
        )
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write", action="store_true", help="Regenerate the C++ directive table.")
    options = parser.parse_args()
    grammar = Lark.open(GRAMMAR, parser="earley", keep_all_tokens=True)
    generated = generate_table(grammar)
    if options.write:
        TABLE.write_text(generated, encoding="utf-8")
    elif TABLE.read_text(encoding="utf-8") != generated:
        parser.exit(1, "Directive table is stale. Run this script with --write.\n")
    print(f"{sum(line.startswith('LPY_DIRECTIVE') for line in generated.splitlines())} directive entries verified")


if __name__ == "__main__":
    main()

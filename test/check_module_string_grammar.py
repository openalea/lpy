"""Generate and verify the C++ syntax table for LPy module strings."""

import argparse
from pathlib import Path

from lark import Lark, Token


ROOT = Path(__file__).resolve().parents[1]
GRAMMAR = ROOT / "doc/user/module_string.lark"
TABLE = ROOT / "src/cpp/module_string_syntax.inc"
STARTS = ["production_string", "pattern_string"]


def generate_table(grammar):
    kinds = {"MODULE": "Module", "GET_MODULE": "GetModule", "SURFACE": "Surface"}
    suffixes = {
        (): "End",
        ("arguments",): "Arguments",
        ("VARIABLE",): "Other",
        ("captured_module",): "Other",
        ("SURFACE_LABEL",): "Other",
        ("SURFACE_LABEL", "arguments"): "Other",
    }
    entries = {}
    for rule in grammar.rules:
        if rule.origin.name not in {"production_module", "pattern_module"}:
            continue
        kind, *suffix = (symbol.name for symbol in rule.expansion)
        mode = "Production" if rule.origin.name == "production_module" else "Pattern"
        key = (mode, kinds[kind], suffixes[tuple(suffix)])
        action = "".join(word.capitalize() for word in rule.alias.split("_"))
        if entries.setdefault(key, action) != action:
            raise ValueError(f"Ambiguous module string action for {key}")

    lines = ["// Generated from doc/user/module_string.lark by test/check_module_string_grammar.py."]
    # A bare ordinary module can also be followed by the next module.
    for mode in ("Production", "Pattern"):
        entries.setdefault((mode, "Module", "Other"), entries[mode, "Module", "End"])
        lines.append(f"    {{ // {mode}")
        for kind in kinds.values():
            actions = (entries[mode, kind, suffix] for suffix in ("End", "Arguments", "Other"))
            lines.append("        {" + ", ".join(f"ModuleSyntax::{action}" for action in actions) + "},")
        lines.append("    },")
    return "\n".join(lines) + "\n"


def check_token_streams(grammar):
    for mode, tokens, actions in [
        ("production_string", ["MODULE", "ARGUMENTS", "GET_MODULE", "VARIABLE"], ["with_arguments", "variable"]),
        ("pattern_string", ["GET_MODULE", "CAPTURED_MODULE", "MODULE"], ["capture", "bare"]),
        ("pattern_string", ["SURFACE", "SURFACE_LABEL", "ARGUMENTS"], ["surface"]),
        ("production_string", ["SURFACE", "ARGUMENTS", "MODULE"], ["with_arguments", "bare"]),
        ("pattern_string", [], []),
    ]:
        parser = grammar.parse_interactive(start=mode)
        for token in tokens:
            parser.feed_token(Token(token, token))
        actual = [str(module.data) for module in parser.feed_eof().children]
        if actual != actions:
            raise ValueError(f"{mode}: {tokens} produced {actual}, expected {actions}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write", action="store_true", help="Regenerate the C++ syntax table.")
    options = parser.parse_args()
    grammar = Lark.open(GRAMMAR, parser="lalr", start=STARTS)
    check_token_streams(grammar)
    table = generate_table(grammar)
    if options.write:
        TABLE.write_text(table, encoding="utf-8")
    elif TABLE.read_text(encoding="utf-8") != table:
        parser.exit(1, "Module string syntax is stale. Run this script with --write.\n")
    print("Module string syntax table and token streams verified")


if __name__ == "__main__":
    main()

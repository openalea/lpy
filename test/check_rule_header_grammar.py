"""Check the declarative rule-header grammar against tracked LPy sources."""

import argparse
from collections import Counter
from pathlib import Path
import re
import subprocess
import sys

from lark import Lark, UnexpectedInput


ROOT = Path(__file__).resolve().parents[1]
SECTION = re.compile(r"(production|decomposition|homomorphism|interpretation)\b")
END_SECTION = re.compile(r"(endlsystem|def)\b")
DIRECTIVE = re.compile(r"(endgroup|group|consider|ignore|derivation length|maximum depth)\b")


def rule_lines(source):
    """Yield candidate header lines without evaluating the model's Python."""
    in_rules = False
    for lineno, line in enumerate(source.splitlines(), 1):
        if line.startswith("###### INITIALISATION ######"):
            break
        if not line or line[0].isspace() or line.startswith("#"):
            continue
        if SECTION.match(line):
            in_rules = True
        elif END_SECTION.match(line) or line.strip() in {"'''", '"""'}:
            in_rules = False
        elif in_rules and not DIRECTIVE.match(line):
            yield lineno, line


HEADER_FIELDS = {
    "left_context": "leftContext",
    "new_left_context": "newLeftContext",
    "predecessor": "predecessor",
    "new_right_context": "newRightContext",
    "right_context": "rightContext",
}
PRODUCTION_TABLE = ROOT / "src/cpp/rule_header_productions.inc"


def generate_production_table(grammar):
    """Expand the context productions into separator sequences and field bindings."""
    literals = {terminal.name: terminal.pattern.value for terminal in grammar.terminals}
    fields = {
        rule.origin.name: rule.expansion
        for rule in grammar.rules
        if rule.origin.name in HEADER_FIELDS
    }
    lines = ["// Generated from doc/user/rule_header.lark by test/check_rule_header_grammar.py."]
    productions = {
        rule.expansion[0].name for rule in grammar.rules if rule.origin.name == "rule_header"
    }
    for rule in grammar.rules:
        if rule.origin.name not in productions:
            continue
        separators = []
        bindings = []
        for field in rule.expansion:
            for symbol in fields[field.name]:
                if symbol.is_term:
                    separators.append(f'"{literals[symbol.name]}"')
                elif symbol.name == "pattern":
                    bindings.append(f"&RuleHeader::{HEADER_FIELDS[field.name]}")
                else:
                    raise ValueError(f"Unexpected rule-header symbol: {symbol.name}")
        lines.append("    {{" + ", ".join(separators) + "}, {" + ", ".join(bindings) + "}},")
    return "\n".join(lines) + "\n"


def check_grammar(grammar):
    sources = subprocess.check_output(
        ["git", "ls-files", "-z", "*.lpy"], cwd=ROOT
    ).decode().split("\0")[:-1]
    forms = Counter()
    terminators = Counter()
    failures = 0
    files_with_headers = set()
    for name in sources:
        source = (ROOT / name).read_text(encoding="utf-8-sig", errors="surrogateescape")
        for lineno, line in rule_lines(source):
            try:
                parsed = grammar.parse(line)
            except UnexpectedInput as error:
                print(f"{name}:{lineno}:{error.column}: {line}", file=sys.stderr)
                failures += 1
                continue
            form, terminator = parsed.children[0].children
            files_with_headers.add(name)
            forms[str(form.data)] += 1
            terminators[str(terminator.data)] += 1

    print(f"{sum(forms.values())} headers accepted in {len(files_with_headers)} files "
          f"({len(sources)} tracked LPy files scanned)")
    print("Context forms:", dict(sorted(forms.items())))
    print("Terminators:", dict(sorted(terminators.items())))
    print(f"{failures} candidates rejected")
    return int(failures != 0 or not forms)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write", action="store_true", help="Regenerate the C++ production table.")
    options = parser.parse_args()
    grammar = Lark.open(ROOT / "doc/user/rule_header.lark", parser="earley", keep_all_tokens=True)
    generated = generate_production_table(grammar)
    if options.write:
        PRODUCTION_TABLE.write_text(generated, encoding="utf-8")
    elif PRODUCTION_TABLE.read_text(encoding="utf-8") != generated:
        sys.exit("Rule-header production table is stale. Run this script with --write.")
    sys.exit(check_grammar(grammar))

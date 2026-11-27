"""Check the declarative rule-header grammar against tracked LPy sources."""

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


def check_grammar():
    grammar = Lark.open(ROOT / "doc/user/rule_header.lark", parser="earley")
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
    sys.exit(check_grammar())

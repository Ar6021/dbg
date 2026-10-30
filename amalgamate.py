"""Header amalgamator that preserves preprocessor semantics.
Edit the constants below and run: python amalgamate.py
"""
import re
import sys
from pathlib import Path

# ---------------- configuration ----------------
MAIN_HEADER  = "inc/dbg/dbg.hpp"
OUTPUT       = "dist/dbg.hpp"
INCLUDE_DIRS = ["inc"]
KEEP         = []   # local include names to leave untouched, e.g. ["generated/config.hpp"]
# -----------------------------------------------

INC       = re.compile(r'^\s*#\s*include\s*"([^"]+)"')
PRAGMA    = re.compile(r'^\s*#\s*pragma\s+once\b')
OPEN      = re.compile(r'^\s*#\s*(if|ifdef|ifndef)\b')
CLOSE     = re.compile(r'^\s*#\s*endif\b')
GUARD     = re.compile(r'^\s*#\s*ifndef\s+(\w+)\s*\n\s*#\s*define\s+\1\b', re.M)
PRAGMA_ML = re.compile(r'^\s*#\s*pragma\s+once\b', re.M)

dirs = [Path(d).resolve() for d in INCLUDE_DIRS]
stack, emitted, guards, out = [], set(), {}, []


def resolve(name, cur_dir):
    for base in [cur_dir, *dirs]:
        p = base / name
        if p.is_file():
            return p.resolve()
    return None


def guard_for(path):
    if path not in guards:
        rel = path.name
        for d in dirs:
            try:
                rel = str(path.relative_to(d))
                break
            except ValueError:
                pass
        g = "AMALG_" + re.sub(r'\W', '_', rel).upper()
        while g in guards.values():
            g += "_"
        guards[path] = g
    return guards[path]


def update_comment_state(line, in_c):
    """Track whether we are inside a /* */ comment after this line."""
    i = 0
    while i < len(line):
        if in_c:
            j = line.find('*/', i)
            if j < 0:
                return True
            in_c, i = False, j + 2
        else:
            j, k = line.find('/*', i), line.find('//', i)
            if j < 0 or (k != -1 and k < j):
                return False
            in_c, i = True, j + 2
    return in_c


def expand(path, unconditional):
    if path in stack:  # circular include
        out.append(f'/* amalgamate: circular include of {path.name} skipped */\n')
        return

    text = path.read_text(encoding='utf-8', errors='replace')
    once = bool(PRAGMA_ML.search(text) or GUARD.search(text))
    if once and path in emitted:
        return

    stack.append(path)
    g = guard_for(path)
    if once:
        out.append(f'#ifndef {g}\n#define {g}\n')
    out.append(f'/* ---- begin {path.name} ---- */\n')

    depth, in_c = 0, False
    for line in text.splitlines(keepends=True):
        if not line.endswith('\n'):
            line += '\n'
        if not in_c:
            if PRAGMA.match(line):
                continue
            if OPEN.match(line):
                depth += 1
            elif CLOSE.match(line):
                depth -= 1
            m = INC.match(line)
            if m and m.group(1) not in KEEP:
                target = resolve(m.group(1), path.parent)
                if target:
                    expand(target, unconditional and depth == 0)
                    continue
                print(f'warning: {path}: cannot resolve "{m.group(1)}", left as-is',
                      file=sys.stderr)
        out.append(line)
        in_c = update_comment_state(line, in_c)

    out.append(f'/* ---- end {path.name} ---- */\n')
    if once:
        out.append('#endif\n')
        if unconditional:
            emitted.add(path)
    stack.pop()


if __name__ == '__main__':
    expand(Path(MAIN_HEADER).resolve(), True)
    out_path = Path(OUTPUT)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(''.join(out), encoding='utf-8')
    print(f'wrote {out_path} ({len(guards)} headers inlined)')
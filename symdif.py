#!/usr/bin/env python3

import subprocess
import os
import sys

archive_files = ["webserv.a"]
build_dir = "./build/"
binary_file = "./webserv"

TTY = sys.stdout.isatty()
COL_CYAN = "\x1b[36m"
COL_GOLD = "\x1b[93m"
COL_GRAY = "\x1b[90m"
COL_GREEN = "\x1b[32m"
COL_RED = "\x1b[31m"
COL_WHITE = "\x1b[37m"


def print_col_cond(msg: str, col: str, cond: bool = True):
    if not cond:
        return
    if TTY:
        print(f"{col}{msg}\x1b[0m")
    else:
        print(msg)


dump_archive = subprocess.run(
    ["/usr/bin/env", "readelf", "--symbols", "--wide"]
    + [f"{build_dir}/{name}" for name in archive_files],
    text=True,
    capture_output=True,
)
dump_binary = subprocess.run(
    ["/usr/bin/env", "readelf", "--symbols", "--wide"] + [binary_file],
    text=True,
    capture_output=True,
)

if dump_archive.returncode != 0 or dump_binary.returncode != 0:
    print("Error when getting the symbols out of the objects files or binary (or both)")
    print(f"{dump_archive.stderr}")
    print(f"{dump_binary.stderr}")
    exit(1)

symbols_archive = set()
symbols_binary = set()

for line in dump_archive.stdout.split("\n"):
    words = line.split()
    if len(words) >= 8 and words[3] == "FUNC":
        idx = words[7].find("@")
        if idx != -1:
            words[7] = words[7][:idx]
        symbols_archive.add(words[7])

for line in dump_binary.stdout.split("\n"):
    words = line.split()
    if len(words) >= 8 and words[3] == "FUNC":
        idx = words[7].find("@")
        if idx != -1:
            words[7] = words[7][:idx]
        symbols_binary.add(words[7])

# diff = list(symbols_archive - symbols_binary)
diff = list(symbols_binary - symbols_archive)
diff.sort()

funcs = set()
try:
    with open("./authorized_functions") as f:
        s = f.read()
        funcs = set(s.split("\n"))
except FileNotFoundError:
    pass

# add known built in functions
built_in = {
    "deregister_tm_clones",
    "frame_dummy",
    "memcmp",
    "memcpy",
    "memmove",
    "memset",
    "register_tm_clones",
    "strlen",
}

skip_none = False
if len(sys.argv) >= 2 and sys.argv[1].lower() == "all":
    skip_none = True

for sym in diff:
    if sym in funcs:
        print_col_cond(sym, COL_GREEN, skip_none)
    elif sym.startswith("_Z"):
        print_col_cond(sym, COL_CYAN, skip_none)
    elif sym.startswith("_") or sym in built_in:
        print_col_cond(sym, COL_GRAY, skip_none)
    else:
        print_col_cond(sym, COL_RED)

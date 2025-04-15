#!/usr/bin/env python3

import subprocess
import os
import sys
import argparse
import shlex
from pathlib import Path

output_colors = {
    "allowed": "\x1b[32m",  # green
    "builtin": "\x1b[90m",  # gray
    "cpp": "\x1b[36m",  # cyan
    "deny": "\x1b[41m",  # red background
    "lib": "\x1b[33m",  # yellow
    "mlx": "\x1b[35m",  # purple
    "other": "\x1b[31m",  # red
}

output_symbols = {
    "allowed": [],
    "builtin": [],
    "cpp": [],
    "deny": [],
    "lib": [],
    "mlx": [],
    "other": [],
}

order = ["deny", "other", "builtin", "allowed", "lib", "mlx", "cpp"]

parser = argparse.ArgumentParser(
    prog="symdif",
    description="print the difference in symbols from some objects and binaries",
    epilog="Made by a cavemen (Maix a.k.a maiboyer) for 42",
)

parser.add_argument(
    "--class",
    "-C",
    dest="classes",
    help="list of classes to print",
    action="extend",
    default=list(),
    choices=list(output_symbols.keys()),
    nargs="+",
)

parser.add_argument(
    "--all", "-A", help="print all possible classes", default=False, action="store_true"
)

parser.add_argument(
    "--demangle",
    "-D",
    help="try to demangle symbols",
    default=False,
    action="store_true",
)


parser.add_argument(
    "--object",
    "-o",
    help="list of objects (.a, .o)",
    default=[],
    required=True,
    action="extend",
    nargs="+",
)

parser.add_argument(
    "--binary",
    "-b",
    help="list of binaries",
    default=[],
    required=True,
    action="extend",
    nargs="+",
)

parser.add_argument(
    "--library",
    "-l",
    help="add a library to use",
    default=[],
    action="extend",
    nargs="+",
)

parser.add_argument(
    "--library-path",
    "-L",
    help="add a library PATH",
    default=[],
    action="extend",
    nargs="+",
)

parser.add_argument(
    "--allow",
    "-a",
    help="add functions to the allowed list",
    default=[],
    action="extend",
    nargs="+",
)

parser.add_argument(
    "--deny",
    "-d",
    help="add functions to the denied list",
    default=[],
    action="extend",
    nargs="+",
)

parser.add_argument(
    "--makefile",
    action="store_true",
    default=False,
    help="Prints a rule `symdif` that should work in your Makefile",
)


args = parser.parse_args()
if args.makefile:
    print(
        """
# change $(OBJ) with the name of all objects
# change $(TARGET) with the name of the executables
# you can modify this to change the default "classes" of symbols showed
SYMDIFF_CLASSES = other builtin cpp allowed
# this can be overriten to add flags (-D to demangle, -A to unconditionnally add all classes)
SYMDIFF_FLAGS = 
symdiff:
    @$(MAKE) --no-print-directory fclean
    @$(MAKE) --no-print-directory all "CXXFLAGS=-g3 -Wall -Wextra -MMD -std=c++98" "CFLAGS=-g3 -Wall -Wextra -MMD"
    @# you can add the -D flag to transform cpp '_Zstuff' into readable symbols in the symdiff rule
    @./symdiff.py -C $(SYMDIFF_CLASSES) -o $(OBJ) -b $(TARGET) $(SYMDIFF_FLAGS)
"""
    )
    exit(0)


def get_libs_args(args: list[str]):
    final_args = ["clang", "-###"] + args + ["/dev/null"]
    pipe = subprocess.Popen(
        final_args,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    stdout, _ = pipe.communicate()
    raw_output = stdout.split("\n")
    while len(raw_output) != 0 and len(raw_output[-1]) == 0:
        raw_output.pop()
    output = {"path": list(), "lib": set()}
    cmds = shlex.split(raw_output[-1])
    for cmd in cmds:
        if cmd.startswith("-L"):
            output["path"].append(Path(cmd[2:]))
        if cmd.startswith("-l"):
            output["lib"].add(cmd[2:])
    # added by default !
    output["lib"] -= {"c"}  # "gcc", "gcc_s",
    return output


def file_type(path):
    try:
        out = subprocess.check_output(["file", path], text=True)
        return out.lower()
    except subprocess.CalledProcessError:
        return ""


def is_library(path):
    info = file_type(path)
    return (
        "elf" in info and ("shared" in info or "relocatable" in info)
    ) or "archive" in info


def get_libs_files(lib_data) -> list[str]:
    out = []
    for lib in lib_data["lib"]:
        for p in lib_data["path"]:
            p: Path
            break_ = False
            lib_path_so, lib_path_a = (
                p.joinpath("lib" + lib + ".so"),
                p.joinpath("lib" + lib + ".a"),
            )
            if lib_path_so.is_file() and is_library(str(lib_path_so)):
                out.append(lib_path_so)
                break_ = True
            if lib_path_a.is_file() and is_library(str(lib_path_a)):
                out.append(lib_path_a)
                break_ = True
            if break_:
                break
    return out


lib_args = get_libs_args(
    [f"-L{path}" for path in args.library_path] + [f"-l{name}" for name in args.library]
)
libs = get_libs_files(lib_args)

if len(args.classes) == 0:
    args.classes = {"other"}
args.classes = set(args.classes)
if args.all:
    args.classes = set(output_symbols.keys())

TTY = sys.stdout.isatty()

dump_archive = subprocess.run(
    ["/usr/bin/env", "readelf", "--symbols", "--wide"] + args.object,
    text=True,
    capture_output=True,
)

dump_archive_unused = subprocess.run(
    ["/usr/bin/env", "nm", "-u", "-j"] + args.object,
    text=True,
    capture_output=True,
)

dump_binary = subprocess.run(
    ["/usr/bin/env", "readelf", "--symbols", "--wide"] + args.binary,
    text=True,
    capture_output=True,
)

dump_libraries = subprocess.run(
    ["/usr/bin/env", "nm", "-j"] + libs,
    text=True,
    capture_output=True,
)

if (
    dump_archive.returncode != 0
    or dump_archive_unused.returncode != 0
    or dump_binary.returncode != 0
    or dump_libraries.returncode != 0
):
    print("Error when getting the symbols out of the objects files or binary (or both)")
    print(f"{dump_archive.stderr}")
    print(f"{dump_archive_unused.stderr}")
    print(f"{dump_binary.stderr}")
    print(f"{dump_libraries.stderr}")
    exit(1)

symbols_archive = set()
symbols_archive_unused = set()
symbols_binary = set()
symbols_libraries = set()

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

symbols_archive_unused = set(dump_archive_unused.stdout.split("\n"))
symbols_libraries = set(dump_libraries.stdout.split("\n"))
symbols_libraries -= symbols_archive
symbols_libraries -= symbols_archive_unused


# this would say which symbol are present in the archive, but not in the binary
# diff = list(symbols_archive - symbols_binary)

# this gives us the functions that aren't defined in the .a, but are present in the binary
diff = list(symbols_binary - symbols_archive)
diff.sort()


# set your allowed function here :)
# currently these are webserv functions :)
funcs = {
    "open",
    "close",
    "read",
    "write",
    "printf",
    "malloc",
    "free",
    "perror",
    "strerror",
    "exit",
    "gettimeofday",
}
funcs.update(args.allow)

# set your allowed function here :)
# currently these are webserv functions :)
denied_funcs = set()
denied_funcs.update(args.deny)

mlx = {
    "mlx_X_error",
    "mlx_clear_window",
    "mlx_col_name",
    "mlx_destroy_display",
    "mlx_destroy_image",
    "mlx_destroy_window",
    "mlx_do_key_autorepeatoff",
    "mlx_do_key_autorepeaton",
    "mlx_do_sync",
    "mlx_expose_hook",
    "mlx_flush_event",
    "mlx_get_color_value",
    "mlx_get_data_addr",
    "mlx_get_screen_size",
    "mlx_hook",
    "mlx_init",
    "mlx_key_hook",
    "mlx_loop",
    "mlx_loop_end",
    "mlx_loop_hook",
    "mlx_mouse_get_pos",
    "mlx_mouse_hide",
    "mlx_mouse_hook",
    "mlx_mouse_move",
    "mlx_mouse_show",
    "mlx_new_image",
    "mlx_new_image2",
    "mlx_new_window",
    "mlx_pixel_put",
    "mlx_put_image_to_window",
    "mlx_set_font",
    "mlx_string_put",
    "mlx_xpm_file_to_image",
    "mlx_xpm_to_image",
}

# add known built in functions
built_in = {
    "bcmp",  # used to compare byte sequence
    "deregister_tm_clones",  # thread locals
    "frame_dummy",  # yes ?
    "memcmp",  # compiler likes to insert those
    "memcpy",  # compiler likes to insert those
    "memmove",  # compiler likes to insert those
    "memset",  # compiler likes to insert those
    "register_tm_clones",  # tread locals I think
    "strlen",  # used by printing stuff
    # internal function used by compiler :)
    "_Unwind_Resume",
    "__assert_fail",
    "__cxa_allocate_exception",
    "__cxa_atexit",
    "__cxa_begin_catch",
    "__cxa_call_unexpected",
    "__cxa_end_catch",
    "__cxa_finalize",
    "__cxa_free_exception",
    "__cxa_guard_abort",
    "__cxa_guard_acquire",
    "__cxa_guard_release",
    "__cxa_pure_virtual",
    "__cxa_rethrow",
    "__cxa_throw",
    "__do_global_dtors_aux",
    "__errno_location",
    "__gxx_personality_v0",
    "__libc_start_main",
    "_fini",
    "_init",
    "_start",
    # seems like clang likes to use these function on newer version ?
    "__isoc23_strtol",
    "__isoc23_strtoull",
    "__open_2",
    "__read_chk",
    "__stack_chk_fail",
    "__vdprintf_chk",
}

for sym in diff:
    if sym in mlx:
        output_symbols["mlx"].append(sym)
    elif sym in symbols_libraries:
        output_symbols["lib"].append(sym)
    elif sym in denied_funcs:
        output_symbols["deny"].append(sym)
    elif sym in funcs:
        output_symbols["allowed"].append(sym)
    elif sym.startswith("_Z"):
        output_symbols["cpp"].append(sym)
    elif sym in built_in:
        output_symbols["builtin"].append(sym)
    else:
        output_symbols["other"].append(sym)


def demangle(names):
    args = ["/usr/bin/env", "c++filt"]
    args.extend(names)
    pipe = subprocess.Popen(
        args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True
    )
    stdout, _ = pipe.communicate()
    demangled = stdout.split("\n")

    # Each line ends with a newline, so the final entry of the split output
    # will always be ''.
    assert len(demangled) == len(names) + 1
    return demangled[:-1]


RESET = "\x1b[0m"

if args.demangle:
    demangled = demangle(output_symbols["cpp"])
    demangled.sort()
    output_symbols["cpp"] = demangled
for class_ in filter(lambda c: c in args.classes, order[::-1]):
    output_symbols[class_].sort()
    for sym in output_symbols[class_]:
        print(f"{output_colors[class_] if TTY else ''}{sym}{RESET if TTY else ''}")

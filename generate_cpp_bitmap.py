#!/usr/bin/env python

# Simple script to convert binary files to CPP byte arrays.

import os
import sys
from pathlib import Path

CONTAINER_TEMPLATE = """#ifndef %(def_name)s
#define %(def_name)s
#ifdef ESP32
#  define E_PROGMEM PROGMEM
#else
#  define E_PROGMEM
#endif
%(declarations)s
#endif
"""

DECLARATION_TEMPLATE = """
const unsigned char %(name)s[] E_PROGMEM = {
%(content)s
};
"""

def file_declaration(def_name, path: Path):
    name = def_name.lower() + "_" + path.with_suffix("").name.lower().replace(".", "_")

    with open(path, "rb", buffering=0) as f:
        content = f.read()

        return DECLARATION_TEMPLATE % {
            "name": name,
            "content": ", ".join([hex(b) for b in content]),
        }

def main(args):
    def_name, files = args[1], args[2:]

    print(CONTAINER_TEMPLATE % {
        "def_name": "__%s_H" % def_name.upper(),
        "declarations": "\n".join([file_declaration(def_name, Path(f)) for f in files]),
    })

if __name__ == "__main__":
    sys.exit(main(sys.argv))


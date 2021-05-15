#!/usr/bin/env python

# Simple script to convert binary files to CPP byte arrays.

import sys

TEMPLATE = """#ifndef %(def_name)s
#define %(def_name)s
#ifdef ESP32
#  define E_PROGMEM PROGMEM
#else
#  define E_PROGMEM
#endif
const unsigned char %(name)s[] E_PROGMEM = {
%(content)s
};
#endif
"""

def main(args):
    var_name, fname = args[1], args[2]
    with open(fname, "rb", buffering=0) as f:
        content = f.read()
        all_bytes = [
            hex(b) for b in content
        ]

        print(TEMPLATE % {
            "def_name": "__%s_H" % var_name.upper(),
            "name": var_name,
            "content": ", ".join(all_bytes),
        })

if __name__ == "__main__":
    sys.exit(main(sys.argv))


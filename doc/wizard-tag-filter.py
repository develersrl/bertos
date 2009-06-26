#!/usr/bin/env python
from __future__ import with_statement
import re, sys

if __name__ == "__main__":
    expr = re.compile(r"\$WIZ\$.*$")
    with file(sys.argv[1]) as f:
        for line in f:
            if re.search(expr, line) != None:
                new_line = re.sub(expr, "", line)
                sys.stdout.write(new_line)
            else:
                sys.stdout.write(line)

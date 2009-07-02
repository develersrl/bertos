#!/usr/bin/env python
from __future__ import with_statement
import re, sys

def remove_expr(regex, line):
	new_line = re.sub(regex, "", line)
	sys.stdout.write(new_line)

if __name__ == "__main__":
    wiz = re.compile(r"\$WIZ\$.*$")
    notest = re.compile(r'notest:.*$')
    with file(sys.argv[1]) as f:
        for line in f:
            if re.search(wiz, line) != None:
                remove_expr(wiz, line)
            elif re.search(notest, line) != None:
                remove_expr(notest, line)
            else:
                sys.stdout.write(line)

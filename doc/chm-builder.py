"""
To be used on windows platform only to generate BeRTOS .chm documentation.
Requires doxygen.exe and hhc.exe to be in the system PATH.
"""
from __future__ import with_statement
import os, sys

def move(old_path, new_path):
    if os.path.exists(new_path):
        os.unlink(new_path)
    os.rename(old_path, new_path)


if sys.platform != 'win32':
    print "This program can be run only on a Windows machine"
    sys.exit(-1)

if len(sys.argv) < 2:
    print "Usage: " + sys.argv[0] + " [custom_doxyfile]"
    sys.exit(-1)

DOC_PATH = 'doc\\offline-reference\\html\\'
toc_modifier = r"doc\chm-toc-modifier.py"
toc_file = DOC_PATH + r"index.hhc"
bertos_toc = r"bertos-toc.hhc"


if os.system("doxygen " + sys.argv[1]):
    print "doxygen error"
if os.system(toc_modifier + " " + toc_file + " " + bertos_toc):
    print "toc-modifier error"
move(bertos_toc, toc_file)

# compile CHM
os.system(r"hhc " + DOC_PATH + "index.hhp")

chm_target = r"bertos\bertos-doc.chm"
move(DOC_PATH + r"bertos-doc.chm", chm_target)



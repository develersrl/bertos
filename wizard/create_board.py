#/usr/bin/evn python
from __future__ import with_statement
import sys, os, shutil

SPEC_TEMPLATE = "\
name = 'Board name'\n\
description = '''\n\
<!-- Put board documentation here -->\n\
''' \
"

EXAMPLE = "\
name='Examples'\n\
ord=1\n\
description='Full working example projects.'\n\
"

TEMPLATE = "\
name = 'Templates'\n\
ord = 0\n\
description = 'Use these as a starting point for your BeRTOS project.'\n\
"

BENCHMARK = "\
name='Benchmarks'\n\
ord=2\n\
description='Projects to measure different aspects of BeRTOS performance.'\n\
"

def makeBoardDir(dir_path, spec_tpl):
    #spec_tpl is a template for .spec file
    os.mkdir(dir_path)
    with open(dir_path + '.spec', 'w') as f:
        f.write(spec_tpl)


if len(sys.argv) < 4:
    print "Usage: %s <board_dir> <board_image> <prj_source_dir>" % sys.argv[0]
    exit(0)

board_dir = sys.argv[1] + os.sep
board_img = sys.argv[2]
source_dir = sys.argv[3]

# create board directory
if not os.path.exists(board_dir):
    os.mkdir(board_dir)
else:
    print "Board directory exists"
    exit(1)

with open(board_dir + ".spec", 'w') as f:
    f.write(SPEC_TEMPLATE)

# copy board image
shutil.copy(board_img, board_dir + ".image.png")

# create hw/ directory
shutil.copytree(source_dir + "hw", board_dir + 'hw')

#create examples, templates and benchmark directories together with .spec files
makeBoardDir(board_dir + "templates" + os.sep, TEMPLATE)
makeBoardDir(board_dir + 'examples' + os.sep, EXAMPLE)
makeBoardDir(board_dir + 'benchmark' + os.sep, BENCHMARK)

print 'Done. Remember to fill in the description of your board in %s' % (board_dir + '.spec')


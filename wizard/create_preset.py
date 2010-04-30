import pickle
import sys
import os
import shutil
import pprint
import glob
import re

def remove(path):
    if os.path.isdir(path):
        shutil.rmtree(path, ignore_errors=True)
    else:
        try:
            os.remove(path)
        except OSError:
            pass

def findPath(start, target):
    pa = start
    while pa != "/":
        pa = os.path.abspath(pa + "/..")
        if os.path.exists(pa + "/" + target):
            return os.path.relpath(pa, start)

if len(sys.argv) < 3:
    print "Usage: %s <project_dir> <preset_dir>" % sys.argv[0]
    exit(0)

prj_dir = sys.argv[1]
preset_dir = sys.argv[2]

if not os.path.exists(prj_dir + "/project.bertos"):
    print "%s does not seem a Wizard created project." % prj_dir
    exit(1)

p = open(prj_dir + "/project.bertos")
s = pickle.load(p)
if s["WIZARD_VERSION"] < 3:
    print "Project version too old."
    exit(1)
pname = s["PROJECT_NAME"] 
preset_dir += pname

hw_path = findPath(preset_dir, "hw")
if not hw_path:
    print "hw/ path not found in parent directories of %s" % preset_dir
    exit(1)

bertos_path = findPath(preset_dir, "bertos")
if not bertos_path:
    print "bertos/ path not found in parent directories of %s" % preset_dir
    exit(1)

shutil.copytree(prj_dir, preset_dir)
remove(preset_dir + "/bertos")
remove(preset_dir + "/images")
remove(preset_dir + "/obj")
remove(preset_dir + "/Makefile")
remove(preset_dir + "/buildrev.h")
remove(preset_dir + "/VERSION")
remove(preset_dir + "/" + pname + ".project")
remove(preset_dir + "/" + pname + ".workspace")

l = glob.glob(preset_dir + "/" + pname + "/*")
for f in l:
    shutil.move(f, preset_dir)

remove(preset_dir + "/" + pname)
remove(preset_dir + "/hw")

assert(os.path.exists(preset_dir + "/" + hw_path     + "/hw"))
assert(os.path.exists(preset_dir + "/" + bertos_path + "/bertos"))

s["BERTOS_PATH"] = bertos_path
s["PRESET"] = True
s["PROJECT_HW_PATH"] = hw_path
s["PROJECT_SRC_PATH"] = "."
s["PRESET"] = True
pprint.pprint(s)
p = open(preset_dir + "/project.bertos", "w")
pickle.dump(s, p)

p = open(preset_dir + "/.spec", "w")
p.write("name = '%s preset'" % pname)
p.close()

bertos_path = os.path.abspath(preset_dir + "/" + bertos_path)
hw_path = os.path.abspath(preset_dir + "/" + hw_path)

src_path = os.path.relpath(preset_dir, bertos_path)
hw_path  = os.path.relpath(hw_path, bertos_path)

mk = open(preset_dir + "/" + pname + ".mk").read()
mk = re.sub(r"(%s_SRC_PATH\s*=\s*).*" % pname, r"\1%s" % src_path, mk)
mk = re.sub(r"(%s_HW_PATH\s*=\s*).*" % pname, r"\1%s" % hw_path, mk)

#remove absolute path from toolchain
mk = re.sub(r'(%s_PREFIX\s*=\s*").*?([^/]*")' % pname, r'\1\2', mk)
open(preset_dir + "/" + pname + ".mk", "w").write(mk)

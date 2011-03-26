from __future__ import with_statement
import sys, re

if len(sys.argv) < 1:
    print "Usage: " + sys.argv[0] + " [hhc index file] [output file]"
    sys.exit(-1)

outfile = "out.hhc"
if len(sys.argv) == 3:
    outfile = sys.argv[2]

data_structures = []

section_start = re.compile(r"<LI>.*? value=\"((File List)|(Data Structures)|(Main Page)|(Directory Hierarchy))\".*$")

data_section = False
file_section = False

with open(sys.argv[1], "r") as f:
    with open(outfile, "w+") as out:
        for line in f:
            if re.search(section_start, line):
                if line.find("Data Structures") != -1:
                    data_section = True
                    file_section = False
                elif line.find("File List") != -1:
                    data_section = False
                    file_section = True
                else:
                    data_section = False
                    file_section = False

            if data_section:
                data_structures.append(line)
            elif file_section:
                pass
            elif re.search(r"^<\/UL>$", line):
                for i in data_structures:
                    out.write(i)
                out.write(line)
            else:
                out.write(line)



import os

file_name = "gversions.h"

base_data = \
"""#ifndef _GVERSIONS_H_
#define _GVERSIONS_H_

/**
 * File generated automaticly by generate_version.py.
 * Values of COMMIT_ID and COMMIT_DATE are readed from git repository.
 * Value BUILD is incremented by script when current commit will
 * be newer than stored in file.
 * Values MAJOR and MINOR will not be changed by script, those need
 * to be changed manually.
 * Values COMMIT_ID_BYTES and COMMIT_DATE_BYTES points to string lengths.
 */

"""
end_data = "\n#endif"

cwd = os.getcwd()
if(cwd.endswith("Debug")):
	file_name = "../"+file_name

data = {
	"COMMIT_ID": '"-------"',
	"COMMIT_DATE": '"0000-00-00 00:00:00"',
	"MAJOR": "0",
	"MINOR": "0",
	"BUILD": "0"
	}

def parse(line):
	line = line.strip()
	if line.startswith("#define "):
		s = line.split(" ")
		if len(s)>=3:
			data[s[1]] = " ".join(s[2:])

def gen_def(key):
	return "#define "+key+" "+data[key]+"\n"

try:
	with open(file_name, 'r') as ifile:
		for line in ifile:
			parse(line)
except:
	print("No previous file.")

cid_cmd = "git rev-parse --short HEAD"
date_cmd = "git show --no-patch --no-notes --pretty='%cd' --date=format:'%Y-%m-%d %H:%M:%S'"

cid = '"'+os.popen(cid_cmd).read().rstrip()+'"'
print("current commit id: {}".format(cid))
date = '"'+os.popen(date_cmd).read().rstrip()+'"'
print("current commit date: {}".format(date))

print("saved commit date: "+data["COMMIT_DATE"])
if date>data["COMMIT_DATE"]:
	print("current id is newer than stored one, updating data.")
	data["COMMIT_ID"] = cid
	data["COMMIT_DATE"] = date
	data["BUILD"] = str(int(data["BUILD"])+1)
else:
	print("current id equal with stored one, no need to update.")
print(data)



ofile = open(file_name, "w")
ofile.write(base_data)

ofile.write(gen_def("MAJOR"))
ofile.write(gen_def("MINOR"))
ofile.write(gen_def("BUILD"))
ofile.write(gen_def("COMMIT_ID"))
ofile.write(gen_def("COMMIT_DATE"))

ofile.write("#define COMMIT_ID_BYTES "+str(len(data["COMMIT_ID"]))+"\n")
ofile.write("#define COMMIT_DATE_BYTES "+str(len(data["COMMIT_DATE"]))+"\n")

ofile.write(end_data)
ofile.close()




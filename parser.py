import json

counter = 0
with open("data", "r") as f:
	lines = f.readlines()
	f1 = open("out", "w")

	for line in lines:
		f1.write("%s\t" % line.strip())
		counter += 1
		if counter == 4:
			counter=0
			f1.write("\n")		

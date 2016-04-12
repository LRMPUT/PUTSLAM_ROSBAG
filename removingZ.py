from sys import argv

with open(argv[1]) as trajFile:
	trajFileContent = trajFile.readlines()
	for l in trajFileContent:
		lineSep = l.split(' ');
		print(str(lineSep[0]) + " " + str(lineSep[1]) + " " + str(lineSep[3]) + "0 0 0 0 0 1");
		

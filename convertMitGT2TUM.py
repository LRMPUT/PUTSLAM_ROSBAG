# The script must be called with 2 files: the laser ground truth file from MiT and timestamps.txt (the resulting file from convesion)
#
# Exemplary usage: python2 convertMitGT2TUM.py 2012-04-06-11-15-29_part1_floor2.gt.laser.poses.txt timestamps.txt
from sys import argv

# Specifies the maximum difference in time that is allowed between ground truth pose and robot pose
# The values is specified in ns, thus the default values is equal to 15 ms
maximumTimeDifference = 15000;

# We need 2 files
if len(argv) == 3:
	
	# We open the ground truth and timestamp file
	with open(argv[1]) as gtFile, open(argv[2]) as timestampFile:
    		
		# Readding the content of the files
		gtFileContent = gtFile.readlines()
		timestampContent = timestampFile.readlines()

		# Reading gt timestamps, X and Y positions
		gtTimestamps = [];
		gtX = [];
		gtY = [];
		for line in gtFileContent:
			line = line.split(',');
			gtTimestamps.append(int(line[0]))
			gtX.append(float(line[1]))
			gtY.append(float(line[2]))
		

		# Going through image timestamps and finding the best index in gt file (if possible!)
		indexGt = 0;
		poseIndex = 0;
		for tLine in timestampContent:

			# Converting timestamp from s to ns
			tValue = int(float(tLine) * 1000000);
			
			# We need to find the place where timestamps match
			while indexGt < len(gtTimestamps) and gtTimestamps[indexGt] < tValue:
				indexGt=indexGt+1;
			
			# End of gt file -> ending the fun
			if indexGt == len(gtTimestamps):
				break;
			
			# Deciding if the best timestamp is the one that is slightly smaller or greater 
			# (we choose the one with smaller abs diff)
			if abs(gtTimestamps[indexGt-1] - tValue) < abs(gtTimestamps[indexGt] - tValue):
				indexGt = indexGt - 1;
			
			# We check if the best match is not worse than 15 ms.
			# If it is, we cannot use this gt information 
			if abs(tValue - gtTimestamps[indexGt]) > maximumTimeDifference :
				poseIndex = poseIndex + 1;
				continue;
			
			# Printing the gt position in TUM RGBD compatible format - we assume Z = 0 and no orientation change
			print(str(poseIndex) + " " + str(gtX[indexGt]) + " " + str(gtY[indexGt]) + " 0" +" 0 0 0 1");
			poseIndex = poseIndex + 1;

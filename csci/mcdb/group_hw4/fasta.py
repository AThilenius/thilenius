#####################################################################
# FUNCTION: 
#       read_fasta_file(in_file)
#
# PURPOSE:
#       Reads in and parses a .fasta file.
#
# USAGE:
#       This function Parses a fasta (given by the file path) and 
#       returns a list of tuples in the form (sequence_name, sequence_data)
#
#
#####################################################################

#Note: this read-in code was the original function, but was redone (see Alec's version below)

'''
def readInput(inFile):
	f = open(inFile, 'r')
	sequenceNames = []
	sequences = []
	sequenceSoFar = []
	output = []
	for line in f.xreadlines():
		line = line.strip('\n')
		if line != '' and line[0] == '>':
			# Separate each sequence header by spaces and store the name
			# (string before the first space) in the names list
			name = line[1:].split()
			sequenceNames.append(name[0])
			# Store the sequence string corresponding to the previous
			# sequence in sequences. Ensure the same chromosome will
			# have its name and sequence stored in the same index of
			# their respective lists.
			if sequenceSoFar != []:
				sequences.append(''.join(sequenceSoFar))
				sequenceSoFar = []
		else:
			# Collect all the lines of a sequence in sequenceSoFar
			sequenceSoFar.append(line)
	# Add the last sequence string into sequences
	sequences.append(''.join(sequenceSoFar))
	# Make a list of lists with the names and sequences
	# Correct for capitalization inconsistencies
	for i in range(len(sequences)):
		output.append([sequenceNames[i],sequences[i].lower()])
	return output
'''

import sys, re


def read_fasta_file(in_file):
  fasta = open(in_file).read()
  # Read the entire file, match regex for names and chromosome data.
  matches = re.findall(r'^>([^\n]+\n*[^>]+)', fasta, re.MULTILINE)
  # Seperate the names from unprocessed sequance data (into a tuple)
  matches = [(m.split('\n')[0], ''.join(m.split('\n')[1:])) for m in matches]
  # Process sequance data (remove \n and \s)
  return [(name, re.sub(r'[\n\s]+', '', data)) for (name, data) in matches]

#def test_file_read_fasta_file():
  #Path to tiny fasta is: ../tiny.fasta
  #store = read_fasta_file('../tiny.fasta')
  #print sequenceNames
  # for item in store:
  # print item


# test_file_read_fasta_file()

#####################################################################
# FUNCTION: 
#       Parse(args_array)
# PURPOSE:
#       Parses out command line args into a dictionary.
#
# USAGE:
#       -Takes in an array of args (the same as sys.argv) 
#       -Returns a dictionary whith keys (arg name),values (arg's)
#
#       For example, the input array:
#       ['-f', 'tiny.fasta', '-s', 'SocringMatrix.py']
#       And will return the dictionary:
#       { f: 'tiny.fasta', s: 'ScoringMatrix.py' }
#
#####################################################################
import sys, getopt

def parse(args_array):
  i = 1
  entires = {}
  while True:
    if i >= len(args_array):
      break
    item = args_array[i]
    # If the item starts with a -
    if len(item) > 0 and  item[0] == '-':
      # If the next arg doesn't start with a -, use it as the value
      if i + 1 < len(args_array) and args_array[i + 1][0] != '-':
        entires[item.lstrip('-')] = args_array[i + 1]
        i += 2
      else:
        # Othersize treat it as a flag and just set it to True
        entires[item.lstrip('-')] = True
        i += 1
    else:
      # Ignore it
      i += 1
  return entires

"""
def test_parse():
  print parse(['-f', 'tiny.fasta', '-s', 'SocringMatrix.py'])
"""
# test_parse()
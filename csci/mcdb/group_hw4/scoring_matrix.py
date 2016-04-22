#####################################################################
# FUNCTION: 
#       parse_scoring_matrix_to_arrays :
# PURPOSE: 
#     Takes in a file path to a scoring matrix 
#     returns a dictionary of dictionaries of scores. 
#       
# USAGE: 
#      For example, the matrix:
#       A	 3	-2	-1	-2
#       C	-2	 3	-2	-1
#       G	-1	-2	 3	-2
#       T	-2	-1	-2	 3
#      And returns a dictionary of dictionaries, like this:
#       {
#         {  3, -2, -1, -2 },
#         { -2,  3, -1, -2 },
#        { -1, -2,  3, -2 }, 
#        { -2, -1, -2,  3 }
#       }
#####################################################################

def parse_scoring_matrix_to_arrays(file_path):
  name = ['A', 'C', 'G', 'T']
  with open(file_path) as content:
    # Read the file in and parse it as a list of lists, split by row then tab
    v = [line.strip().split('\t')[1:] for line in content]
    # Convert the strings into actual ints
    v = [[int(val) for val in row] for row in v]
    # Convert the list of lists to a dictionary of dictionaries for easy lookup.
    # This allows things like: loopup['A']['G'] to get the score.
    return {name[r]: { name[c]: v[r][c] for c in xrange(4)} for r in xrange(4)}

def test_parse_scoring_matrix():
  print parse_scoring_matrix_to_arrays('ScoringMatrix.txt')

#test_parse_scoring_matrix()
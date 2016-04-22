#####################################################################
# FUNCTION: 
#       compute_distance(seq_row, seq_col, scores, gap):
#
# PURPOSE:
#       The purpose of this function is to generate the distances between 
#       sequences so that they can be made into a distance matrix and 
#       utilized in the UPGMA.
#
# USAGE:
#     -Takes two strings, scoring matrix, and a gap score 
#     -Creates a matrix of of the correct size   
#     -Decraments the first row and column by the gap penalty starting at zero    
#     -Increment through matrix excluding the first row and column
#       -Pull the score from the scoring matrix
#       -Find values for the diagonal, above, and left and ammend with gap/score
#       -Find maximum of values above
# 
# RETURN:
#     -Returns the bottom right value of the matrix (Hamming distance)
#
#####################################################################
# Professor Knox:
  # There is no need to back trace back through the matrix
  # You fill the first row/col with gap*row or gap*col (with negative gap 
  # penalties that gives you increasingly negative numbers). 
  # This decrements the first row and column by the gap penalty.
  # Then the score in the bottom right is the hamming distance.
#####################################################################
def compute_distance(seq_row, seq_col, scores, gap):
  """Computes the global alignment matrix for the given sequences using the
  given scoring matrix and gap penalty."""
  # Convention: all accesses will be row, col (row major)
  # Pad each sequence to make the math easy
  seq_row = ' ' + seq_row
  seq_col = ' ' + seq_col
  # Create a matrix and popilate the first row and colum
  mat = [[(c + r) * gap for r in xrange(len(seq_col))]
       for c in xrange(len(seq_row))]
  #Increment through matrix excluding the first row and column
  for row in range(1, len(seq_row)):
    for col in range(1, len(seq_col)):
      # Get the score for chaging nucleotides
      change_score = scores[seq_col[col]][seq_row[row]]
      # Set the values for the diagonal, above, and left for a particular cell.
      # Use the scoring matrix for diagonal.
      diag = mat[row - 1][col - 1] + change_score
      above = mat[row - 1][col] + gap
      left = mat[row][col - 1] + gap
      # Select the maximum choice. Note that if the values are the same the max
      # function will pick the value that appears first in the list.
      mat[row][col] = max(diag, above, left)
  
  return mat[len(seq_row) - 1][len(seq_col) - 1]
  
"""
def test_global_matrix():
  import scoring_matrix
  scores = scoring_matrix.parse_scoring_matrix_to_arrays('ScoringMatrix.txt')
  print compute_distance('ACG', 'ACC', scores, -2)
"""
# test_global_matrix()
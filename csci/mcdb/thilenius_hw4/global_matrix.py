#####################################################################
# FUNCTION:
#             global_matrix(seq_row, seq_col, scoreDicti, gap)
#
# PURPOSE:
#             return the global maximum for the best alignmet score.
#
# USAGE:
#             This function takes two strings, scoring matrix, and a gap score and
#             will. It will then call a function to create a distance matrix with
#             the score and the two sequences as arguements.
#
#
#####################################################################


# def global_matrix(seq_row, seq_col, scores, gap):
    # """Computes the global alignment matrix for the given sequences using the
    # given scoring matrix and gap penalty."""
    # # Convention: all accesses will be row, col (row major)
    # # Pad each sequence to make the math easy
    # seq_row = ' ' + seq_row
    # seq_col = ' ' + seq_col
    # # Create a zero matrix
    # mat = [[0 for r in xrange(len(seq_col))] for c in xrange(len(seq_row))]
    # # Populate 0th row of the matrix
    # for row in xrange(len(seq_row)):
        # mat[row][0] = (row, (row - 1, 0))
    # # Populate 0th column of the matrix
    # for col in xrange(len(seq_col)):
        # mat[0][col] = (col, (0, col - 1))
    # # Set a marker for the upper left corner cell
    # mat[0][0] = (0, None)
    # # Compute each cell score, keeping breadcrumbs (as the second element of
    # # each tuple) as go along
    # for row in range(1, len(seq_row)):
        # for col in range(1, len(seq_col)):
            # change_score = scores[seq_col[col]][seq_row[row]]
            # # Set the values for the diagonal, above, and left for a particular
            # # cell. Use the scoring matrix for diagonal.
            # diag = (mat[row - 1][col - 1][0] + change_score, (row - 1, col - 1))
            # above = (mat[row - 1][col][0] + gap, (row - 1, col))
            # left = (mat[row][col - 1][0] + gap, (row, col - 1))
            # # Select the maximum choice. Note that if the values are the same
            # # the max function will pick the value that appears first in the
            # # list.
            # mat[row][col] = max(diag, above, left, lambda t: t[0])
    # # Back trace
    # (cell_val, cell_offset) = mat[len(seq_row) - 1, len(seq_col - 1)]
    # score = 0
    # while True:
        # if cell_offset is None:
            # return score
        # score += 
        # (new_row, new_col) = cell_offset
        # (cell_val, cell_offset) = mat[new_row][new_col]

def global_matrix(seq_row, seq_col, scores, gap):
    """Computes the global alignment matrix for the given sequences using the
    given scoring matrix and gap penalty."""
    # Convention: all accesses will be row, col (row major)
    # Pad each sequence to make the math easy
    seq_row = ' ' + seq_row
    seq_col = ' ' + seq_col
    # Create a zero matrix
    mat = [[(c + r) * gap for r in xrange(len(seq_col))]
           for c in xrange(len(seq_row))]
    for row in range(1, len(seq_row)):
        for col in range(1, len(seq_col)):
            change_score = scores[seq_col[col]][seq_row[row]]
            # Set the values for the diagonal, above, and left for a particular
            # cell. Use the scoring matrix for diagonal.
            diag = mat[row - 1][col - 1] + change_score
            above = mat[row - 1][col] + gap
            left = mat[row][col - 1] + gap
            # Select the maximum choice. Note that if the values are the same
            # the max function will pick the value that appears first in the
            # list.
            mat[row][col] = max(diag, above, left)
    # Professor Knox:
    # We don't need to back trace, figure that out after like 2 hours >:(
    # You fill the first row/col with gap*row or gap*col (with negative gap pens
    # that gives you increasingly negative numbers). Then the score in the
    # bottom right is the hamming distance.
    return mat[len(seq_row) - 1][len(seq_col) - 1]

def test_global_matrix():
    scores = {'A': {'A': 3, 'C': -2, 'T': -2, 'G': -1}, 'C': {'A': -2, 'C': 3,
            'T': -1, 'G': -2}, 'T': {'A': -2, 'C': -1, 'T': 3,
               'G': -2}, 'G': {'A': -1, 'C': -2, 'T': -2, 'G': 3}}
    print global_matrix('ACG', 'ACC', scores, 0)

test_global_matrix()

#####################################################################
# FUNCTION: 
#       compute_distance_list(scores, seqs, gap_pen)
#
# PURPOSE:
#       This function will create a distance matrix from 
#       the maximum alignment score from globl_matrix.py this  will 
#       allow for a phylogenetic tree to be generated
#       through the upgma algorithm.
#
# USAGE:
#     Takes in the scoring matrix, as a list of lists, and a 
#     list of sequences as a list of tuples, along with the gap penalty
#####################################################################

import sets, itertools
from global_matrix import compute_distance

def compute_distance_list(scores, seqs, gap_pen):
  """
  Computes the actual distance 'matrix'. It's actually a list of tuples as that
  is easier to work with.
  Args:
    scores A dictionary of dictionaries that represent the scoring matrix
    seq The list of sequences where each item is a tuple of (name, data)
    gap_pen An integer type penalty added to each gab of the global alignment
  Returns:
    A list of tuples. Each tuple is of the form
    ((left_seq_name, right_seq_name), distance_between_them). Only uniqie
    sequences are returned (meaning only AB, not BA) as well as not duplicates,
    aka, no AA or BB.
  """
  
  distance_list = list(itertools.combinations(seqs, 2))
  distance_list = [((ln, rn), compute_distance(ld, rd, scores, gap_pen))
                   for ((ln, ld), (rn, rd)) in distance_list]
  return distance_list

def print_distance_list_as_matrix(scores, seq, gap_pen):
  """
  Prints out a distance matrix (by computing everything, including duplicated).
  This is a bit ugly, but it's for printing, so I don't care very much.
  Args:
    scores A dictionary of dictionaries that represent the scoring matrix
    seq The list of sequences where each item is a tuple of (name, data)
    gap_pen An integer type penalty added to each gab of the global alignment
  """
  items = []
  for s1 in seq:
    for s2 in seq:
      items.append(((s1[0], s2[0]), compute_distance(s1[1], s2[1], scores, gap_pen)))

  # Convert the list into a lookup dictionary
  # that takes the S1 out of the tuple, diregarding what is in the second half.
  lookup = { t[0]: t[1] for t in items}
  for (s1, second_half_of_tuple) in seq:
    print s1 + '\t' + str([lookup[(s1, s2)] for (s2, second_half_of_tuple) in seq])

import fasta, scoring_matrix

def test_distance_matrix():
  # Load the test FASTA file
  sequences = fasta.read_fasta_file('./tiny.fasta')
  scores = scoring_matrix.parse_scoring_matrix_to_arrays('ScoringMatrix.txt')
  distance_list = compute_distance_list(scores, sequences, -1)
  print_distance_list_as_matrix(scores, sequences, -1)

# test_distance_matrix()

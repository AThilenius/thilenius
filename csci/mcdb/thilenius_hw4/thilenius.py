"""
================================================================================
Alec Thilenius
For usage: python thilenius.py --helpshort
================================================================================
"""

import gflags
import itertools
import os.path
import re
import sys

FLAGS = gflags.FLAGS
gflags.DEFINE_integer('gap', None, 'Gap penalty', short_name='g')
gflags.DEFINE_string('filename', None, 'Input fasta file path', short_name='f')
gflags.DEFINE_string('output', None, 'Optional output file', short_name='o')
gflags.DEFINE_string('scoring', None, 'Input scoring file', short_name='s')


def read_fasta_file():
    """Reads a .FASTA file into name-sequence chromosome tuples."""
    fasta = open(FLAGS.filename).read()
    # Read the entire file, match regex for names and chromosome data.
    matches = re.findall(r'^>([^\n]+\n*[^>]+)', fasta, re.MULTILINE)
    # Seperate the names from unprocessed sequance data (into a tuple)
    matches = [(m.split('\n')[0], ''.join(m.split('\n')[1:])) for m in matches]
    # Process sequance data (remove \n and \s)
    return [(name, re.sub(r'[\n\s]+', '', data)) for (name, data) in matches]


def read_scoring_matrix():
    """Parses a scoring matrix into a dictionary or dictionarys for easy lookup.
    You can use this lookup like this: lookup['A']['T']."""
    score_data = open(FLAGS.scoring).read().split('\n')
    # Pull the nucleotide names (order of them) out
    names = [line.split()[0] for line in score_data if line]
    # Pull the values out of each row
    data = [line.split()[1:] for line in score_data if line]
    # Convert the strings into actual ints
    data = [[int(val) for val in row] for row in data]
    # Convert the list of lists to a dictionary of dictionaries for easy #
    # lookup.  This allows things like: loopup['A']['G'] to get the score.
    return {names[r]: {names[c]: data[r][c] for c in xrange(4)}
            for r in xrange(4)}


def distance(seq_row, seq_col, scores):
    """Computes the global alignment matrix for the given sequences using the
    given scoring matrix and gap penalty."""
    # Add a space to both seqances to make the math easier
    seq_row = ' ' + seq_row
    seq_col = ' ' + seq_col
    gap = FLAGS.gap
    # Create a matrix where the first row and first col are filled with
    # increasing gap penalties (that way we !!DON'T NEED TO DO A TRACE BACK!!).
    # Figured that out after like 2 hours >:(
    mat = [[(c + r) * gap for r in xrange(len(seq_col))]
           for c in xrange(len(seq_row))]
    for row in range(1, len(seq_row)):
        for col in range(1, len(seq_col)):
            change_score = scores[seq_col[col]][seq_row[row]]
            # Set the values for the diagonal, above, and left for a particular
            # cell. Use the scoring matrix for diagonal (above)
            mat[row][col] = max(mat[row - 1][col - 1] + change_score,
                                mat[row - 1][col] + gap,
                                mat[row][col - 1] + gap)
    return mat[len(seq_row) - 1][len(seq_col) - 1]


def select(search_list, seq_name):
    """A helper that selects all forward and backward matches of a
    combination and returns them as well as the modified original list."""
    forward = [((left, right), val) for ((left, right), val)
               in search_list if left == seq_name]
    backward = [((right, left), val) for ((left, right), val)
                in search_list if right == seq_name]
    new_search_list = [((left, right), val) for ((left, right), val)
                       in search_list if left != seq_name and right != seq_name]
    return (forward + backward, new_search_list)


def upgma(combination_list):
    """Runs the UPGMA algorithm on the given computation list recursively."""
    print 'Combination List: ', combination_list
    if len(combination_list) == 1:
        ((left, right), value) = combination_list[0]
        return '(%s, %s):%g;' % (left, right, value/2.0)
        # (_, val) = combination_list[0]
        # return (combination_list[0][0], val / 2.0)
    # Find the min in the list
    ((left, right), value) = min(combination_list, key=lambda (_, value): value)
    # Remove min from the list
    combination_list = [
        item for item in combination_list if item != ((left, right), value)]
    # Select the first item and sequence
    (sel_left, combination_list) = select(combination_list, left)
    (sel_right, combination_list) = select(combination_list, right)
    # Sort the list so we can select pairs of 2
    selected = sorted(sel_left + sel_right, key=lambda ((l, right), v): right)
    print 'Selected: ', selected
    # Grab each set of 2
    for i in xrange(0, len(selected), 2):
        ((_, fr), fv) = selected[i]
        ((_, _), sv) = selected[i + 1]
        print 'Averaging: ', selected[i], ' and ', selected[i + 1]
        combination_list.append((('(%s, %s):%g' % (left, right, value/2.0),
            fr), (fv + sv) / 2.0))
    return upgma(combination_list)


def main():
    """The entry point after args have been processed by gflags."""
    sequance_tuples = read_fasta_file()
    scoring_matrix = read_scoring_matrix()
    # Print out the distance matrix. Yep, this is ugly, but it's a print so I
    # don't really care.
    print 'Scoring Matrix:'
    print '\t', '\t'.join([name for (name, _) in sequance_tuples])
    for (name, d_row) in sequance_tuples:
        print name, '\t', '\t'.join([str(distance(d_row, d_col, scoring_matrix))
                                     for (_, d_col) in sequance_tuples])
    # Compute the distance tuple list.
    distance_list = list(itertools.combinations(sequance_tuples, 2))
    distance_list = [((ln, rn), distance(ld, rd, scoring_matrix))
                     for ((ln, ld), (rn, rd)) in distance_list]
    grouping_tree = upgma(distance_list)


if __name__ == '__main__':
    FLAGS(sys.argv)
    # I (and many other programs) think it's totally valid to just fail and
    # print a stack trace when a file given on as a command arg doesn't exist.
    # But I'll play ball and print messages if they don't, or if the command
    # args are wrong :)
    if not FLAGS.filename or not FLAGS.scoring or not FLAGS.gap:
        print ('Usage: %s -f <fasta_file> -s <scoring_file> -g <gap_pen>' %
               sys.argv[0])
    else:
        if not os.path.isfile(FLAGS.filename):
            print 'File %s does not exist!' % FLAGS.filename
        if not os.path.isfile(FLAGS.scoring):
            print 'File %s does not exist!' % FLAGS.scoring
        if FLAGS.gap >= 0:
            print '\033[93mPositive gat PENALTY? You sure?\033[0m'
        # If we are asked to send data to an output file, just redirect stdout
        sys.stdout = open(FLAGS.output, 'w') if FLAGS.output else sys.stdout
        main()

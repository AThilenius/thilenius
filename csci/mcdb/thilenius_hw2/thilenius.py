"""
================================================================================
Alec Thilenius
For usage: python thilenius.py --helpshort

My 53 line description of what my 29 line program does :p

Quick Overview:
    This script reads in a file in FASTA format and returns the 5 most common
    k-mers in all the sequences where k is given by the parameter l.
    See 'python thilenius.py --helpshort' for usage details.

Purpose, References, and Usage:
    The purpose of this program is to find the most common kmers in sequences of
    data, loaded from a FASTA file. The kmer length and the FASTA file are given
    as arguments to the program. References are in-line with code. Please see
    'python thilenius.py --helpshort' for usage.

Readability Note:
    This code very strictly adheres to Google's Style guide, including the use
    of whitespace. This is strictly governed by the JSCS formatter in Google
    style mode. Please allow for differences in opinion of what 'readability'
    counts as, this code reflects Google's opinion.

Outline - Efficiency:
    Complexity is bounded between O(N) and O(Nlog(S)) where N is the number of
    kmers in the file, and S is the number of sequences. Best case, the
    sequences don't have tons of duplicates, then then performance is O(N)
    following the same algorithum below. If, however, each sequence was the
    exact same then 5 * S 'top results' will be returned, resulting in 5 * S
    pulls from a heap (a log2N operation).

Outline - Implementation:
    Best case O(N) efficiency is implemented as follows:
    - Read the entire file, and break it into a list of sequences: O(N)
    - Break each sequance into all kmers
    - Throw those kmers in a set to remove duplicates
    - Add each unique kmer to a hash-based lookup table storing (kmer, count)
        pairs: O(N)
    - Invert the dictionary to a bag of (count, kmer) O(N)
    - Heapify the inverted dictionary in-place O(N)
    - Remove top 5 values: O(1) for small kmers.
      - This is where it can turn into O(Nlog(S)) if all sequances are the same,
        as N kmers will be eligable for 'top 5' and for each of them, all S
        sequences will need to be pulled from the heap

Reusability:
    This script is far, far too simple and short to worry about making the code
    reusable.

14 blank lines, 59 comment lines, 29 lines of code
================================================================================
"""

import collections, getopt, gflags, heapq, os.path, re, sets, sys

FLAGS = gflags.FLAGS
gflags.DEFINE_integer('length', None, 'The length of the k-mar', short_name='l')
gflags.DEFINE_string('filename', None, 'Input fasta file path', short_name='f')
gflags.DEFINE_string('output', None, 'Optional output file', short_name='o')


def main(argv):
    # Read the entire file
    file = open(FLAGS.filename).read()
    # The lookup table storing counts
    counts = collections.defaultdict(int)
    # Read the entire file, match regex for chromosome data only
    matches = re.findall('^>[^\n]+\n([^>]+)', file, re.MULTILINE)
    # Replace \n and spaces with nothing
    matches = [re.sub('[\n\s]+', '', m) for m in matches]
    # For each match
    for match in matches:
        l = FLAGS.length
        # Generate all k-mers with a sliding window, iterate them
        # Inspired from http://goo.gl/WFMR2m
        kmers = [match[i:i + l] for i in xrange(len(match) - l + 1)]
        # Create a set from the kmers
        unique_kmers = sets.Set(kmers)
        # Increment each in the total count
        for kmer in unique_kmers:
            counts[kmer] += 1
    # Invert the dictionary to a list of (-value, key) tuples
    heap = [(-v, k) for (k, v) in counts.iteritems()]
    # Heapify the inverted dictionary
    heapq.heapify(heap)
    # Print the top 5 kmers (top 5 by unique count) and their count
    print '5 most common %i-mer from the %i sequences in fasta file [%s]:' % (
        FLAGS.length, len(matches), FLAGS.filename)
    printed = 0
    for i in xrange(5):
        # Print all values that are the same, as long as the heap isn't empty
        while len(heap) > 0:
            (count, kmer) = heapq.heappop(heap)
            print "%s %i" % (kmer, -count)
            printed += 1
            if len(heap) > 0 and heap[0][0] != count: break
        if printed >= 5: return


if __name__ == '__main__':
    args = FLAGS(sys.argv)
    # If we are asked to send data to an output file, just redirect stdout
    if (FLAGS.output): sys.stdout = open(FLAGS.output, 'w')
    main(args)

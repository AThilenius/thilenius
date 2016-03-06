"""
================================================================================
For usage: python thilenius.py --helpshort

Processes an input .FASTA file, searches the given k-mer in the given chromosome
and outputs those findings to std out in .GFF file format. Also computes the GC
for the given chromosome and prints it to std out.

This application is written to be simple on the code side of things, and is not
optimized in any way. Memory usage is O(N) by file size; Files are not streamed.

This program is released without restriction, licence or warranty.

Written By: Alec Thilenius
14 blank lines, 33 comment lines, 35 lines of code
================================================================================
"""

import sys, gflags, getopt, re, os.path

FLAGS = gflags.FLAGS
gflags.DEFINE_string('filename', None, 'Input fasta file path', short_name='f')
gflags.DEFINE_string('chromo_name', None, 'Chromosome name', short_name='c')
gflags.DEFINE_string('k_mer', None, 'K-Mer name', short_name='k')

def read_fasta_file(file_path):
    """Reads an processes a .FASTA file into name-sequence chromosome tuples.

    Args:
        file_path: The file path to the input .FASTA file for processing.

    Returns:
        An array of tuples where each tuple is in the form
        (name_of_chromosome, raw_chromosome_data).
    """
    with open(file_path) as f:
        # Split into sequence blocks, skipping blank lines
        for sequence_block in re.split('^\>', f.read(), 0, re.MULTILINE):
            if not sequence_block: continue
            sequence_identifier = sequence_block.split('\n', 1)[0]
            sequence_raw = sequence_block.split('\n', 1)[1]
            sequence = re.sub('[\s\\n]', '', sequence_raw)
            yield (sequence_identifier.split(' ', 1)[0], sequence)

def search_sequence(sequence, k_mer):
    """Searches a given sequence for the given k_mer and it's reverse
    complement.

    Args:
        sequence: A string containing the DNA sequence.
        k_mer: A string containing the k_mer to search for.

    Returns:
        A union of matches of the k_mer and matches of the reverse complement
        k_mer. This is returned as an array of tuples in the form
        (strand_direction, start_position)
    """
    inv = {'a':'g', 't':'c', 'g':'a', 'c':'t'}
    # Compute reverse complement iff the character is in the inv map. Convert it
    # to a string (with join)
    reverse_compliment = ''.join(
        [inv[mer] if mer in inv else mer for mer in k_mer[::-1]])
    forward_matches = re.finditer(k_mer, sequence, re.IGNORECASE)
    reverse_matches = re.finditer(reverse_compliment, sequence, re.IGNORECASE)
    return [('+', m.start()) for m in forward_matches] + \
        [('-', m.start()) for m in reverse_matches]

def main(argv):
    gc_count = total_count = 0
    for name, sequence in read_fasta_file(FLAGS.filename):
        if name.lower() != FLAGS.chromo_name.lower(): continue
        for strand, start in search_sequence(sequence, FLAGS.k_mer.lower()):
            print '%s\t%s\t%s\t%i\t%i\t%d\t%s\t%s\t%s' % ( FLAGS.chromo_name,
                'thilenius', 'Gene', start + 1, start + 1 + len(FLAGS.k_mer),
                100.0, strand, 0, '.')
        gc_count += len(re.findall('[GgCc]', sequence))
        total_count += len(sequence)
    print 'GC content: ', 0 if total_count == 0 else (100.0 * gc_count /
        total_count), '%'

if __name__ == '__main__':
    main(FLAGS(sys.argv))

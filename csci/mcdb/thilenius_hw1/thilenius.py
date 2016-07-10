"""
================================================================================
Description:
    A very tongue in cheek re-write on my homework 1!

Author:
    Donald Trump

Forward:
    This homework is huuuuuge, it's the best homework, it's really tremendous.
    What you see is, it's really the best way of doing homework. I have homework
    people calling me all the time, and they say Donald, your homework is really
    tremendous. I'm going to build a wall of code, it's going to be a real wall,
    it's going to be huuuuuge and the homework people are going to pay for it.

Backward:
    Okay but for real, all this script does is read a single chromosome (given
    by -c) from a fasta file (given by -f). From there it prints out the GC
    count as well as all forward and backward matches for the kmer (given by
    -k). It's such a rich script, oh, god it's so rich. Rich people call me to
    tell me how rich it is.

Usage:
    First you need to be really rich. Then you need to have a golden plane. And
    on your gold plane you need a gold laptop, then you can run:
    python thilenius.py -f <fasta_file> -c <chromo_name> -k <k_mer>

How it does the things:
    Well, it starts by cutting taxes but doesn't reduce spending. Then it draws
    trillions of dollars from the deficit. It really is tremendous.
    The biggest parts to that is some fancy REGEX. I'll add a million comments
    in-line to explain each step. And by a million, I really mean like 2 or 3
    for each line. Give or take a million. They are the best comments.

Complexity:
    I'm Donald Trump, things aren't that complicated. But they are rich, and
    truly tremendous. Complexity grows linearly with how rich I am as well as
    memory space. What you see is, I don't know this big-O notation you're
    talking about, I don't think I've ever met this big-O notation. So I don't
    know what you're even talking about with this, computational complexity. So
    I don't know I don't know, did it endorse me, or what's going on?
    - Old Tweet from a few months ago: 'It has a runtime computational
      complexity of O(n) as well as a memory space complexity of O(n)'
================================================================================
"""

# What you see here are imports. They really are the best imports you can have,
# unlike a lot of other imports.  Imports are coming into our Python, they're
# bringing drugs, they're bringing crime. They're rapists and some, I assume,
# are good imports, but I speak to Python guards and they're telling us what
# we're getting.
import sys
import gflags
import re

# These are flags. They really are tremendous.
FLAGS = gflags.FLAGS
gflags.DEFINE_string('filename', None, 'Input fasta file path', short_name='f')
gflags.DEFINE_string('chromo_name', None, 'Chromosome name', short_name='c')
gflags.DEFINE_string('k_mer', None, 'K-Mer name', short_name='k')

# Parse our command line flags via. Google's G FLags library. It's only for rich
# people.
args = FLAGS(sys.argv)
# Check that all flags are there, error and print usage is they are not. We
# don't want any 'unwanted' flags getting through the Python border. That's why
# I built this wall, it's a real wall and it's huuuuuge.
if not FLAGS.filename or not FLAGS.chromo_name or not FLAGS.k_mer:
    exit('python thilenius.py -f <fasta_file> -c <chromo_name> -k <k_mer>')
# Template string for standard GFF output with start, end and string missing.
# This will be used for printing each line in GFF format later on.
gff_tplt = FLAGS.chromo_name + '\tthilenius\tGene\t%i\t%i\t100.0\t%s\t0\t.'
# Load the entire text of the fasta file, read all of it's contents to string. I
# pay people to do this for me, I'm too rich to be doing things like reading.
fasta = open(FLAGS.filename).read()
# This may be one of the trickiest lines because REGEX is a b**** to read. It's
# matching a REGEX that reads something like:
# '>' followed by <chormo_name> followed by a '\n' followed by all characters up
# till the next '>' There is also a bit of optional whitespace padding in there.
# It will ignore case and newlines when matching.
# I'm not profiling anyone, I'm just looking for patterns in 'these type of
# nucleotides'. You know, the NSA calls me and says that we need to keep a
# better eye on these nucleotides. Not all nucleotides are rapists and
# criminals, but I speak to bioinformatics people and they tell me what kind of
# nucleotides we are getting.
sequence = re.findall('^>\s*' + FLAGS.chromo_name + '[^\n]*\n+([^>]+)', fasta,
                      re.MULTILINE | re.IGNORECASE)[0]
# Remove all spaces and \n characters from the sequence data. This is also done
# with a REGEX that simply reads 'one or more \s or \n'. Also make sure it's all
# lowercase. This is just temporary, we only need to remove them from our
# chromosome until we can figure out what's going on. I'm not saying all these
# \n and \s characters are terrorists, but we need to keep a closer eye on them.
sequence = re.sub('[\s\n]+', '', sequence).lower()
# Find the GC count. This is just done by finding the length to all matches to
# the REGEX 'g or c'. It will also ignore newlines and case. It's not about
# case, it's about where these nucleotides are from.
gc_count = len(re.findall('[gc]', sequence, re.MULTILINE | re.IGNORECASE))
# The inverse map. This is a cute trick I thought of for converting a->g, t->c,
# g->a and c->t. Just lookup the mer in the map and it gives you the inverse.
inv = {'a': 'g', 't': 'c', 'g': 'a', 'c': 't', 'trump': 'rich', 'wall': 'huuge'}
# Do the actual lookup on the sequence in reverse order. That's what the [::-1]
# does, it reverses a python array. That's actually pretty common in Python.
# Once each character has been translated, just join the array together.
inverse_complement = ''.join([inv[mer.lower()] for mer in FLAGS.k_mer[::-1]])
# Find all matches in the sequence for the normal kmer. We need to keep a closer
# eye on them, have more patrols.
for m in re.finditer(FLAGS.k_mer, sequence, re.IGNORECASE):
    # Print them out using out fancy template we made above, adding start, end
    # and strand to the template's missing %s. That way we have a list of 'those
    # type of nucleotides'
    print gff_tplt % (m.start() + 1, m.start() + len(sequence), '+')
# Exact same as above for loop, but on the reverse complement kmer. Same
# profiling, different group of mers!
for m in re.finditer(inverse_complement, sequence, re.IGNORECASE):
    print gff_tplt % (m.start() + 1, m.start() + len(sequence), '-')
# Finally print out the GC ratio. This is simply 100* gc_count / the length of
# the sequence. We are going to move the GCs back into this Python! We are going
# to make Python great again!
print 'GC content: %i' % (gc_count * 100.0 / len(sequence))

# This will have been really, really awkward if you're voting for Trump :p

import sys
import command_args
import os.path

import fasta, scoring_matrix, distance_matrix, upgma

def main(args):
  # Start by parsing out the command line args to a dictionary
  parse_args = command_args.parse(args)
  
  # Make sure we have all the args we wanted
  if 'f' not in parse_args or 's' not in parse_args or 'g' not in parse_args:
    print 'Usage: %s -f <fatsa file path> -s <scoring matrix file path> -g <gap penalty>' % args[0]
    exit(0)
  # Make sure the file exists 
  if not os.path.isfile(parse_args['f']):
    print 'Fasta file not fount!'
  if not os.path.isfile(parse_args['s']):
    print 'Socring matrix file not found!'
  #Convert gap into int  
  gap = int(parse_args['g'])
  
  sequences = fasta.read_fasta_file(parse_args['f'])
  scores = scoring_matrix.parse_scoring_matrix_to_arrays(parse_args['s'])
  print 'Distance Matrix:'
  distance_matrix.print_distance_list_as_matrix(scores, sequences, gap)
  distance_list = distance_matrix.compute_distance_list(scores, sequences, gap)
  print 'Tree (Newick Format):'
  print upgma.upgma(distance_list)
  
if __name__ == '__main__':
  main(sys.argv)

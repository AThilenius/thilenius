// Copyright 2016 Alec Thilenius
// All rights reserved.

#include "fasta_file.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace thilenius {
namespace csci {
namespace mcdb {
namespace thilenius_hw3 {

void Fasta::StreamFileInSeqPairs(
    const std::string& file_path,
    const std::function<void(NucleotideSequencePtr seq)>& seq_handler) {
  // Stream the file, line by line
  std::ifstream infile(file_path);
  if (!infile) {
    std::cerr << "Failed to load FASTA file!" << std::endl;
    exit(0);
  }
  NucleotideSequencePtr seq(nullptr);
  for (std::string line; std::getline(infile, line);) {
    if (line[0] == '>') {
      // Start of a sequence, fire off the event handler for the old sequence
      if (seq) {
        seq_handler(seq);
      }
      seq = NucleotideSequencePtr(new NucleotideSequence());
      seq->name = line.substr(1, line.length() - 1);
    } else {
      if (line.length() > 0) {
        seq->nucleotides = seq->nucleotides + line;
      }
    }
  }
  // Fire off the last sequence as well
  if (seq) {
    seq_handler(seq);
  }
}

void Fasta::StreamFileInSeqPairs(
    const std::string& file_path,
    const std::function<void(NucleotideSequencePtr seq_one,
                             NucleotideSequencePtr seq_two)>&
        seq_pair_handler) {
  NucleotideSequencePtr buffer;
  Fasta::StreamFileInSeqPairs(
      file_path, [&buffer, seq_pair_handler](NucleotideSequencePtr seq) {
        if (buffer) {
          seq_pair_handler(buffer, seq);
          buffer = nullptr;
        } else {
          buffer = seq;
        }
      });
}

}  // namespace thilenius_hw3
}  // namespace mcdb
}  // namespace csci
}  // namespace thilenius

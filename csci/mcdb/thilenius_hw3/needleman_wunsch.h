// Copyright 2016 Alec Thilenius
// All rights reserved.

#ifndef CSCI_MCDB_THILENIUS_HW3_NEEDLEMAN_WUNSCH_H_
#define CSCI_MCDB_THILENIUS_HW3_NEEDLEMAN_WUNSCH_H_

#include <functional>
#include <string>

#include "fasta_file.h"

namespace thilenius {
namespace csci {
namespace mcdb {
namespace thilenius_hw3 {

// Represents a single sequence alignment
struct SequenceAlignment {
  std::string top_sequence;
  std::string bottom_sequence;
  int score;
};

// Prints out a SequenceAlignment
std::ostream& operator<<(std::ostream& out, const SequenceAlignment& align);

// A multithreaded Needleman Wunsch solver for global nucleotide sequences.
class NeedlemanWunsch {
 public:
  static SequenceAlignment Solve(NucleotideSequencePtr top_seq,
                                 NucleotideSequencePtr left_seq);
};

}  // namespace thilenius_hw3
}  // namespace mcdb
}  // namespace csci
}  // namespace thilenius

#endif  // CSCI_MCDB_THILENIUS_HW3_NEEDLEMAN_WUNSCH_H_

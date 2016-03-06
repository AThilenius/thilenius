// Copyright 2016 Alec Thilenius
// All rights reserved.

#ifndef CSCI_MCDB_THILENIUS_HW3_FASTA_FILE_H_
#define CSCI_MCDB_THILENIUS_HW3_FASTA_FILE_H_

#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace thilenius {
namespace csci {
namespace mcdb {
namespace thilenius_hw3 {

// Represents a single named nucleotide sequence
struct NucleotideSequence {
  // The name of the Sequence
  std::string name;
  // The full string of nucleotides, not including newlines
  std::string nucleotides;
};

typedef std::shared_ptr<NucleotideSequence> NucleotideSequencePtr;

// All things to do with the FASTA file format. For now, this is nothing more
// than just reading + parsing a fasta file. The data is streamed as it becomes
// available, however it is streamed via the calling thread.
class Fasta {
 public:
  // Streams a FASTA file, calling seq_handler as soon each seq becomes
  // available.
  static void StreamFileInSeqPairs(
      const std::string& file_path,
      const std::function<void(NucleotideSequencePtr seq)>& seq_handler);

  // Streams a FASTA file, calling seq_pair_handler as soon as a pair or
  // sequences are avalable.
  static void StreamFileInSeqPairs(
      const std::string& file_path,
      const std::function<void(NucleotideSequencePtr seq_one,
                               NucleotideSequencePtr seq_two)>&
          seq_pair_handler);
};

}  // namespace thilenius_hw3
}  // namespace mcdb
}  // namespace csci
}  // namespace thilenius

#endif  // CSCI_MCDB_THILENIUS_HW3_FASTA_FILE_H_

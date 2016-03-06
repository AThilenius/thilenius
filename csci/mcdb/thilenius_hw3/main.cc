// Copyright 2015 Alec Thilenius
// All rights reserved.
//
// Usage:
//   ./thilenius_hw3 -f <file path>
//
// Comments/Readabuility:
//    I wrote this in C++ to be fast. With that said, it's not the prettiest (or
//    anywhere close to the even 'pretty') C++. Things like the
//    NeedlemanWunsch::Solve function are very ugly, but very fast.
//
//  Compiling:
//    cmake .
//    make
//
//  Efficiency:
//    The Needleman-Wunsch algorithm is O(N^2), that hasn't been changed.
//    Complexity will grow at that rate. Memory space is also O(N^2) as I didn't
//    add any type of tiling.
//
//    But, I was very careful with how I used memory and the ALU so it's very
//    fast. Loops only do addition and subtraction and the 2D array is flat for
//    better cache coherence.
//
//  Outline:
//    The fasta file is loaded and for every 2 sequences it reads in, a callback
//    is invoked and they are processed using NeedlemanWunsch. I created this
//    callback to allow for thread pooling if I wasn't feeling lazy with async
//    file I/O. I feel lazy so I didn't add that.
//
//  Reusabuility:
//    The FastaFile class is reusable, although I think it's a total hunk of
//    crap and wouldn't reuse it...

#include <chrono>
#include <iostream>

#include "fasta_file.h"
#include "needleman_wunsch.h"

using ::thilenius::csci::mcdb::thilenius_hw3::Fasta;
using ::thilenius::csci::mcdb::thilenius_hw3::NeedlemanWunsch;
using ::thilenius::csci::mcdb::thilenius_hw3::NucleotideSequence;
using ::thilenius::csci::mcdb::thilenius_hw3::NucleotideSequencePtr;

// Current time in milliseconds from unix epoch
uint64_t EpochMilis() {
  return std::chrono::system_clock::now().time_since_epoch() /
         std::chrono::milliseconds(1);
}

int main(int argc, char** argv) {
  uint64_t start_time = EpochMilis();
  // Parse command line flags (in a really shitty way)
  std::string usage =
      "Usage: " + std::string(argv[0]) + " --fasta_file <file path>";
  if (argc < 3) {
    std::cerr << usage << std::endl;
    exit(0);
  }
  uint64_t solve_time = 0;
  // Load the FASTA file, and for ever sequence pair, run it through the
  // NeedlemanWunsch solver.
  Fasta::StreamFileInSeqPairs(
      argv[2], [&solve_time](NucleotideSequencePtr seq_one,
                             NucleotideSequencePtr seq_two) {
        uint64_t start_time = EpochMilis();
        // Solve the pair and pring it out
        std::cout << NeedlemanWunsch::Solve(seq_one, seq_two);
        solve_time += (EpochMilis() - start_time);
      });
  uint64_t wall_time = EpochMilis() - start_time;
  // Print out stats (to std::err, you can still output normal results to file)
  std::cerr << std::endl;
  std::cerr << "Runtime Stats:" << std::endl;
  std::cerr << "   Wall Time (Ms): " << wall_time << std::endl;
  std::cerr << " Solver Time (Ms): " << solve_time << std::endl;
  std::cerr << "File IO Time (Ms): " << wall_time - solve_time << std::endl;
}

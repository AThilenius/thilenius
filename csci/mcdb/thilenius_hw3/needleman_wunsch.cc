// Copyright 2016 Alec Thilenius
// All rights reserved.

#include <algorithm>
#include <iostream>

#include "needleman_wunsch.h"

namespace thilenius {
namespace csci {
namespace mcdb {
namespace thilenius_hw3 {
namespace {

struct TableEntry {
  enum class Direction { UNKNOWN, UP, LEFT, DIAG };
  int score;
  Direction direction;
};

}  // namespace

std::ostream& operator<<(std::ostream& out, const SequenceAlignment& align) {
  out << align.top_sequence << std::endl;
  out << align.bottom_sequence << std::endl;
  for (int i = 0; i < align.top_sequence.length(); i++) {
    out << "=";
  }
  out << " (" << align.score << ")" << std::endl;
  return out;
}

// This function is not designed to be easy to read, it's designed to be very
// fast. I sacrificed a lot of readabuility in the name of speed here.
SequenceAlignment NeedlemanWunsch::Solve(NucleotideSequencePtr top_seq,
                                         NucleotideSequencePtr left_seq) {
  int table_width = top_seq->nucleotides.length() + 1;
  int table_height = left_seq->nucleotides.length() + 1;
  // Create the initial 2D array. This is a flattened 2D array for tighter cache
  // coherence. I will also be storing row offsets in each 2D loop to prevent
  // needing to do multiplication, a relatively expensive operation
  TableEntry* arr = new TableEntry[table_width * table_height];
  // Fill in initial values across the top
  for (int i = 0; i < table_width; i++) {
    arr[i].score = i;
    arr[i].direction = TableEntry::Direction::LEFT;
  }
  // Fill in the initial values across the left
  for (int row = 0, row_offset = 0; row < table_height;
       row++, row_offset += table_width) {
    arr[row_offset].score = row;
    arr[row_offset].direction = TableEntry::Direction::UP;
  }
  // Now compute out the rest of the table, again, storing row offsets
  for (int row = 1, row_offset = table_width; row < table_height;
       row++, row_offset += table_width) {
    for (int col = 1; col < table_width; col++) {
      // The compiler can pull out the 'last_row_offset' from these
      TableEntry* current = &arr[row_offset + col];
      TableEntry* left = &arr[row_offset + col - 1];
      TableEntry* up = &arr[row_offset - table_width + col];
      TableEntry* diag = &arr[row_offset - table_width + col - 1];
      int diag_score =
          diag->score +
          (left_seq->nucleotides[row - 1] == top_seq->nucleotides[col - 1] ? 0
                                                                           : 1);
      int left_score = left->score + 1;
      int up_score = up->score + 1;
      // Let the compiler turn this into a ternary if it wants. This is ugly
      // enough as it
      if (left_score < diag_score && left_score < up_score) {
        // Left smallest
        current->score = left_score;
        current->direction = TableEntry::Direction::LEFT;
      } else if (up_score < diag_score && up_score < left_score) {
        // Up smallest
        current->score = up_score;
        current->direction = TableEntry::Direction::UP;
      } else {
        // Diagonal smallest or equal
        current->score = diag_score;
        current->direction = TableEntry::Direction::DIAG;
      }
    }
  }
  // Now trace back through the matrix, building in the backwards result strings
  // as we go.
  std::string top_buffer;
  std::string left_buffer;
  int top_offset = top_seq->nucleotides.length() - 1;
  int left_offset = left_seq->nucleotides.length() - 1;
  int row_offset = (table_height - 1) * table_width;
  int col = table_width - 1;
  while (true) {
    if (row_offset <= 0 || col <= 0) {
      break;
    }
    TableEntry* cursor = &arr[row_offset + col];
    switch (cursor->direction) {
      case TableEntry::Direction::UNKNOWN:
        std::cerr << "Oh snap. Something went really, really wrong";
        exit(1);
      case TableEntry::Direction::UP:
        // Gap the top
        top_buffer.push_back('-');
        left_buffer.push_back(left_seq->nucleotides[left_offset--]);
        row_offset -= table_width;
        break;
      case TableEntry::Direction::LEFT:
        // Gap the left
        top_buffer.push_back(top_seq->nucleotides[top_offset--]);
        left_buffer.push_back('-');
        col--;
        break;
      case TableEntry::Direction::DIAG:
        // Don't gap
        top_buffer.push_back(top_seq->nucleotides[top_offset--]);
        left_buffer.push_back(left_seq->nucleotides[left_offset--]);
        row_offset -= table_width;
        col--;
        break;
    }
  }
  // Reverse both strings
  std::reverse(top_buffer.begin(), top_buffer.end());
  std::reverse(left_buffer.begin(), left_buffer.end());
  // We be done, return them
  return SequenceAlignment{std::move(top_buffer), std::move(left_buffer),
                           arr[(table_width * table_height) - 1].score};
}

}  // namespace thilenius_hw3
}  // namespace mcdb
}  // namespace csci
}  // namespace thilenius

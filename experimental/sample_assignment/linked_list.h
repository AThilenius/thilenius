// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXPERIMENTAL_SAMPLE_ASSIGNMENT_H_
#define EXPERIMENTAL_SAMPLE_ASSIGNMENT_H_

#include <deque>

class LinkedList {
public:
  LinkedList();
  virtual ~LinkedList();

  virtual void Push(int value);
  virtual int Pop();

private:
  std::deque<int> deque_;
};

#endif // EXPERIMENTAL_SAMPLE_ASSIGNMENT_H_

// Copyright 2015 Alec Thilenius
// All rights reserved.
#pragma once

#include <vector>
#include <unordered_map>

// ==  UTCondition  ===========================================================
class UTCondition {
 public:
  UTCondition(bool didPass, std::string name, std::string asertMessage);
  void Print();

 public:
  bool DidPass;
  std::string Name;
  std::string AssertMessage;
};

// ==  UTTestConfiguration  ====================================================
class UTTestConfiguration {
 public:
  UTTestConfiguration();

 public:
  int MinMemory;
  int MaxMemory;
  bool LeakCheck;
};

// ==  MemoryAllocation  =======================================================
class MemoryAllocation {
 public:
  MemoryAllocation(void* address, size_t size, std::string file, size_t line);

 public:
  void* Address;
  size_t Size;
  std::string File;
  size_t Line;
};

// ==  UTTest  =================================================================
class UTTest {
 public:
  UTTest();
  bool DidPass();
  int PointsEarned();
  void RegisterAllocation(MemoryAllocation allocation);
  void RegisterFree(void* ptr);
  void Print();

 public:
  std::string Name;
  int points_possible;
  int points_denominator;
  std::string FatalMessage;
  UTTestConfiguration Configuration;
  std::vector<UTCondition> Conditions;
  std::unordered_map<void*, MemoryAllocation> OutstandingAllocations;
  std::unordered_map<void*, MemoryAllocation> OutstandingFrees;
  std::vector<MemoryAllocation> ClosedAllocations;
};

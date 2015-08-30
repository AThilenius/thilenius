//
//  UTTest.cpp
//  UTFramework
//
//  Created by Alec Thilenius on 4/20/15.
//  Copyright (c) 2015 Thilenius. All rights reserved.
//
#include "test.h"

#include <math.h>
#include <sstream>
#include <string>

#include "console_colors.h"
#include "json.h"

// ==  UTCondition  ===========================================================
UTCondition::UTCondition(bool passed, std::string name,
                         std::string asert_message)
    : did_pass(passed), name(name), assert_message(asert_message) {}

void UTCondition::Print(std::ostream& stream) {
  if (did_pass) {
    stream << Blue << "|   |   " << Green << "-" << name << std::endl;
  } else {
    stream << Blue << "|   |   " << Red << "-" << name << std::endl;
    stream << Blue << "|   |   |   " << Yellow << assert_message
              << std::endl;
  }
}

std::string UTCondition::WriteJson() {
  ::nlohmann::json json = {
    {"did_pass", did_pass},
    {"name", name},
    {"assert_message", assert_message}
  };
  return json.dump();
}

// ==  UTTestConfiguration  ====================================================
UTTestConfiguration::UTTestConfiguration()
    : min_memory(-1), max_memory(-1), leak_check(true) {}

// ==  MemoryAllocation  =======================================================
MemoryAllocation::MemoryAllocation(void* address, size_t size, std::string file,
                                   size_t line)
    : address(address), size(size), file(file), line(line) {
  std::vector<std::string> tokens;
  std::istringstream stringStream(file);
  std::string token;
  while (std::getline(stringStream, token, '\\')) {
    tokens.push_back(token);
  }
  if (tokens.size() > 1) {
    file = tokens[tokens.size() - 1];
  } else {
    // Split on /
    std::istringstream string_stream_forward(file);
    tokens.clear();
    while (std::getline(string_stream_forward, token, '/')) {
      tokens.push_back(token);
    }
    if (tokens.size() > 1) {
      file = tokens[tokens.size() - 1];
    }
  }
}

// ==  UTTest  =================================================================
UTTest::UTTest()
    : memory_leaked(false),
      name("unknown"),
      points_possible(0),
      points_denominator(0) {}

bool UTTest::DidPass() {
  bool didPass = true;
  for (UTCondition condition : conditions) {
    if (!condition.did_pass) didPass = false;
  }
  size_t closedsize = 0;
  for (MemoryAllocation allocation : closed_allocations) {
    closedsize += allocation.size;
  }
  size_t ftcsize = 0;
  for (auto kvp : outstanding_allocations) {
    ftcsize += kvp.second.size;
  }
  size_t ftosize = 0;
  for (auto kvp : outstanding_frees) {
    ftosize += kvp.second.size;
  }
  // Min check
  if (configuration.min_memory != -1 && configuration.min_memory > closedsize) {
    std::stringstream stringStream;
    stringStream << "Not Enough Heap Memory Allocated!\n";
    stringStream << " - Minimum Required Heap Allocations: "
                 << configuration.min_memory << " Bytes\n";
    stringStream << " - Total Heap Allocations: " << closedsize << " Bytes";
    fatal_message = stringStream.str();
    memory_leaked = true;
    return false;
  }
  // Max check
  if (configuration.max_memory != -1 && configuration.max_memory < closedsize) {
    std::stringstream stringStream;
    stringStream << "Too Much Heap Memory Allocated!\n";
    stringStream << " - Maximum Allowed Heap Allocations: "
                 << configuration.max_memory << " Bytes\n";
    stringStream << " - Total Heap Allocations: " << closedsize << " Bytes";
    fatal_message = stringStream.str();
    memory_leaked = 0;
    return false;
  }
  // First check for memory leaks
  if (configuration.leak_check && ftcsize > 0) {
    std::stringstream stringStream;
    stringStream
        << "Memory Leak! new was called without a corresponding delete:\n";
    for (auto kvp : outstanding_allocations) {
      stringStream << "  - " << kvp.second.size
                   << " Bytes from: " << kvp.second.file
                   << ". line: " << kvp.second.line << "\n";
    }
    stringStream << ftcsize << " Bytes leaked total";
    fatal_message = stringStream.str();
    memory_leaked = 0;
    return false;
  }
  return didPass;
}

int UTTest::PointsEarned() {
  if (memory_leaked) {
    return false;
  }
  int passed_conditions = 0;
  for (const auto& condition : conditions) {
    if (condition.did_pass) {
      passed_conditions += 1;
    }
  }
  if (conditions.size() == 0) {
    return points_possible;
  }
  return round(static_cast<float>(points_possible) *
               static_cast<float>(passed_conditions) /
               static_cast<float>(conditions.size()));
}

void UTTest::RegisterAllocation(MemoryAllocation allocation) {
  outstanding_allocations.insert({allocation.address, allocation});
}

void UTTest::RegisterFree(void* ptr) {
  auto iter = outstanding_allocations.find(ptr);
  if (iter != outstanding_allocations.end()) {
    // Close correctly
    closed_allocations.push_back(iter->second);
  } else {
    // delete called without new being called
    outstanding_frees.insert({iter->first, iter->second});
  }
  outstanding_allocations.erase(iter);
}

void UTTest::Print(std::ostream& stream) {
  stream << Blue << "|   Test: " << name << std::endl;
  // Print out all but the last conditions
  for (UTCondition condition : conditions) {
    condition.Print(stream);
  }
  if (fatal_message == "") {
    if (DidPass()) {
      stream << Blue << "|   " << Green << "Passed!" << std::endl;
    } else {
      stream << Blue << "|   " << Red << "Failed!" << std::endl;
    }
  } else {
    std::istringstream iss(fatal_message);
    for (std::string line; std::getline(iss, line);) {
      stream << Blue << "|   " << Red << line << std::endl;
    }
  }
  stream << Blue << "|" << std::endl;
}

std::string UTTest::WriteJson() {
  // No try-catch because this will be called around a try-catch
  ::nlohmann::json json = {
    {"did_pass", DidPass()},
    {"name", name},
    {"points_earned", PointsEarned()},
    {"points_possible", points_possible},
    {"points_denominator", points_denominator},
    {"tests", ::nlohmann::json::array()}
  };
  for (auto& condition : conditions) {
    json["tests"].push_back(::nlohmann::json::parse(condition.WriteJson()));
  }
  return json.dump();
}

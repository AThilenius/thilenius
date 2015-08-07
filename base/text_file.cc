// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "base/text_file.h"

#include "base/file.h"
#include "base/log.h"

namespace thilenius {
namespace base {

// static
TextFile TextFile::LoadFromFileOrFail(const std::string& path) {
  TextFile file;
  file.file_contents_ = File::ReadContentsOrDie(path);
  return std::move(file);
}

}  // namespace base
}  // namespace thilenius

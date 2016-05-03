// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2005-2011 Google, Inc.
// Author: rws@google.com (Richard Sproat)
//
// Simple implementation of StrCat, needed in various places.  This version
// allows from 2 to 5 combinations of strings and ints.

#ifndef THRAX_COMPAT_STRUTILS_H_
#define THRAX_COMPAT_STRUTILS_H_

#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <fstream>
#include <string>
#include <vector>

namespace thrax {

using std::string;
using std::fstream;
using std::ios;
using std::vector;

struct StringOrInt {
  StringOrInt(const string& s) : str_(s) {}
  StringOrInt(const char* s) : str_(string(s)) {}
  StringOrInt(int i) {
    char buf[1024];
    sprintf(buf, "%d", i);
    str_ = string(buf);
  }
  string str_;
};

extern string StrCat(const StringOrInt &s1, const StringOrInt &s2);

extern string StrCat(const StringOrInt &s1,
                     const StringOrInt &s2,
                     const StringOrInt &s3);

extern string StrCat(const StringOrInt &s1,
                     const StringOrInt &s2,
                     const StringOrInt &s3,
                     const StringOrInt &s4);

extern string StrCat(const StringOrInt &s1,
                     const StringOrInt &s2,
                     const StringOrInt &s3,
                     const StringOrInt &s4,
                     const StringOrInt &s5);

extern string StringPrintf(const char* format, ...);

extern void SplitStringAllowEmpty(const string& full, const char* delim,
                                  vector<string>* result);

extern vector<string> Split(const string& full, const char* delim);

extern string JoinPath(const string& dirname, const string& basename);

extern const char* Suffix(const char* filename);

extern const string Suffix(const string& filename);

extern string StripBasename(const char* filename);

extern string StripBasename(const string& filename);

extern bool Readable(const string& f);

extern void ReadFileToStringOrDie(const string& file, string* store);

extern bool RecursivelyCreateDir(const string& path);

class File {
 public:
  File() : stream_(NULL) {}
  explicit File(fstream* stream) : stream_(stream) {}

  ~File() { delete stream_; }
  void SetStream(fstream* stream) {
    stream_ = stream;
  }
  fstream* stream() { return stream_; }

  void Close() {
    stream_->close();
    delete stream_;
  }

 private:
  fstream* stream_;
};

// 2^14 --- should be enough for 1 line for the intended use

#define MAXLINE 16384

class InputBuffer {
 public:
  explicit InputBuffer(File* fp) : fp_(fp) { }
  ~InputBuffer() { delete fp_; }
  bool ReadLine(string* line) {
    line->clear();
    fp_->stream()->getline(buf_, MAXLINE);
    if (!fp_->stream()->gcount()) {
      delete fp_;
      fp_ = NULL;
      return false;
    }
    line->append(buf_);
    return true;
  }

 private:
  File* fp_;
  char buf_[MAXLINE];
};

File* Open(const string& filename, const string& mode);

File* OpenOrDie(const string& filename, const string& mode);

}  // namespace thrax

#endif  // THRAX_COMPAT_STRUTILS_H_

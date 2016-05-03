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
// Author: wojciech@google.com (Wojciech Skut)
//         ttai@google.com (Terry Tai)
//
// Lexer class for grammars.

#ifndef NLP_GRM_LANGUAGE_LEXER_H_
#define NLP_GRM_LANGUAGE_LEXER_H_

#include <string.h>
#include <algorithm>
#include <iostream>
#include <set>
#include <stack>
#include <string>

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/compat/utils.h>

DECLARE_int32(v);

namespace thrax {

class Lexer {
 public:
  // Different token classes recognized by this lexer.
  enum TokenClass {
    EOS,                   // end of string
    QUOTED_STRING,         // quoted sequence of characters
    DOUBLE_QUOTED_STRING,  // double-quoted sequence of characters
    DESCRIPTOR,            // name of a function/transducer/...
    INTEGER,               // integer
    FLOAT,                 // float
    CONNECTOR,             // any special symbol
    ANGLE_STRING,          // string enclosed in angle brackets
    KEYWORD,               // reserved keywords
  };

  static const set<string> kKeywords;

  // Preprocessing - must be called before the grammar is processed via repeated
  // calls to YYLex().
  void ScanString(const string &str) {
    grammar_.push(GrammarFile("", str));
  }

  // Parses one token and returns its type. Token data is accessible via
  // 'YYString()'.
  TokenClass YYLex();

  // Access to the most recently read string.
  const string &YYString() const;

  // Access to the beginning and one past end positions of the most recently
  // read string. The difference between the two may not equal the length of
  // YYString() for string literals with escape sequences.
  int YYBeginPos() const;
  int YYEndPos() const;

  // Current line number in the grammar being processed.  If the grammar has
  // been popped empty, which can happen in GetChar() if we run off the end of
  // the file while in the middle of processing, return -1.
  int line_number() const {
    if (grammar_.empty()) return -1;
    return curr_file()->line_number;
  }

  // Context string (for error messages and debugging).
  // If grammar is popped empty, return empty string.
  string GetCurrentContext() const {
    if (grammar_.empty()) return "";
    int end = curr_file()->pos;
    int start = curr_file()->content.rfind('\n', end - 1);
    if (start == string::npos)
      start = 0;
    else
      ++start;  // Skip over the actual newline.
    return curr_file()->content.substr(start, end - start);
  }

  // Path to current grammar file.
  const string &current_grammar_path() const {
    return curr_file()->filename;
  }

 private:
  // Information about the current token.
  struct Token {
    string token_string;
    TokenClass token_class;
    int begin_pos, end_pos;

    void Reset() {
      token_string.clear();
      token_class = EOS;
      begin_pos = -1;
      end_pos = -1;
    }
  };

  struct GrammarFile {
    GrammarFile(const string &fn, const string &cont)
        : filename(fn), content(cont), pos(0), line_number(1) {}
    GrammarFile()
        : pos(0), line_number(1) {}

    string filename;
    string content;
    int pos;
    int line_number;
  };

  Token curr_token_;            // Current token data.
  stack<GrammarFile> grammar_;

  GrammarFile *curr_file() { return &grammar_.top(); }
  const GrammarFile *curr_file() const { return &grammar_.top(); }

  // The following symbols are considered connectors (unless escaped).
  // '-' and '_' are also connectors but are handled separately.
  bool is_connector(char c) const { return strchr("()=:;[]{}|*+@,.?$/", c); }

  int GetChar() {
    int c = 0;

    while (!grammar_.empty() && curr_file()->pos >= curr_file()->content.size())
      grammar_.pop();

    if (!grammar_.empty() && curr_file()->pos < curr_file()->content.size()) {
      c = curr_file()->content[curr_file()->pos++];
      if (c == '\n') ++curr_file()->line_number;
    }
    return c;
  }

  // Moves processing position one character back.
  void UnGetChar() {
    GrammarFile *const current_file = curr_file();
    if (0 <= current_file->pos &&
        current_file->pos < current_file->content.size() &&
        current_file->content[--current_file->pos] == '\n') {
      --current_file->line_number;
    }
  }

  // Zero based position of the next character in a file. Returns -1 when the
  // grammar has been popped empty.
  int GetPos() const {
    if (grammar_.empty()) {
      return -1;
    }
    return curr_file()->pos;
  }
};

}  // namespace thrax

#endif  // NLP_GRM_LANGUAGE_LEXER_H_

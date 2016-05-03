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

#include <thrax/lexer.h>

#include <ctype.h>
#include <iostream>
#include <set>

namespace thrax {

static set<string> InitStaticKeywords() {
  set<string> keywords;
  keywords.insert("as");
  keywords.insert("export");
  keywords.insert("func");
  keywords.insert("import");
  keywords.insert("return");
  keywords.insert("byte");
  keywords.insert("utf8");
  return keywords;
}
const set<string> Lexer::kKeywords = InitStaticKeywords();

Lexer::TokenClass Lexer::YYLex() {
  int begin_pos = GetPos();
  int c = GetChar();

  bool found_token = false;
  curr_token_.Reset();

  while (!grammar_.empty() && !found_token && c != 0) {
    if (isspace(c)) {  // skip space
      while (isspace(c) && c != 0) {
        begin_pos = GetPos();
        c = GetChar();
      }
    } else if (c == '#') {  // skip comment
      while (c != '\n' && c != 0) {
        begin_pos = GetPos();
        c = GetChar();
      }
      if (c == '\n') {
        begin_pos = GetPos();
        c = GetChar();
      }
    } else if (c == '"' || c == '\'') {  // quoted string
      char terminator = c;
      curr_token_.token_class = c == '"' ? DOUBLE_QUOTED_STRING : QUOTED_STRING;

      while (true) {
        char curr_c = GetChar();
        if (curr_c == '\0') {
          LOG(FATAL) << "Found EOF without terminating string: "
                     << curr_token_.token_string;
        }
        if (curr_c == '\\') {
          curr_c = GetChar();
          CHECK_NE(curr_c, '\0');
          if (curr_c != terminator)
            curr_token_.token_string += '\\';
        } else if (curr_c == terminator) {
          break;
        }

        curr_token_.token_string += curr_c;
      }

      found_token = true;
    } else if (is_connector(c)) {  // connector
      curr_token_.token_string += c;
      curr_token_.token_class = CONNECTOR;
      found_token = true;
    } else if (isdigit(c) || c == '.' || c == '-') {  // integer
      if (c == '-') {
        curr_token_.token_string += '-';
        c = GetChar();
        if (!isdigit(c) && c != '.') {
          curr_token_.token_class = CONNECTOR;
          found_token = true;
          UnGetChar();
          continue;
        }
      }

      found_token = true;
      curr_token_.token_class = INTEGER;
      while (isdigit(c)) {
        curr_token_.token_string += c;
        c = GetChar();
      }
      if (c == '.') {  // float
        curr_token_.token_string += c;
        curr_token_.token_class = FLOAT;
        c = GetChar();
        int num_frac_digits = 0;
        while (isdigit(c)) {
          curr_token_.token_string += c;
          c = GetChar();
          ++num_frac_digits;
        }
        CHECK_GT(num_frac_digits, 0);
      }
      if (c != 0) UnGetChar();
    } else if (isalpha(c) || c == '_') {
      found_token = true;
      curr_token_.token_class = CONNECTOR;
      while (isalpha(c) || isdigit(c) || c == '_' || c == '.') {
        if (c != '_' && c != '.')
          curr_token_.token_class = DESCRIPTOR;
        curr_token_.token_string += c;
        c = GetChar();
      }
      if (c != 0) UnGetChar();

      if (kKeywords.find(curr_token_.token_string) != kKeywords.end())
        curr_token_.token_class = KEYWORD;
    } else if (c == '<') {
      curr_token_.token_class = ANGLE_STRING;
      while ((c = GetChar()) != '>') {  // multichar label ([LABEL])
        curr_token_.token_string += c;
      }
      found_token = true;
    } else {
      LOG(FATAL) << "Cannot parse character " << static_cast<char>(c) << " ("
                 << c << ")\n";
    }
  }
  curr_token_.begin_pos = begin_pos;
  curr_token_.end_pos = GetPos();
  return curr_token_.token_class;
}

const string &Lexer::YYString() const {
  return curr_token_.token_string;
}

int Lexer::YYBeginPos() const { return curr_token_.begin_pos; }

int Lexer::YYEndPos() const { return curr_token_.end_pos; }

}  // namespace thrax

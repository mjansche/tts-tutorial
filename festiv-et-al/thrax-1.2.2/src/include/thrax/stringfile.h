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
// Reads in a file of strings to be compiled into an fst
// uses the speech/fst/prefix_tree functionality.

#ifndef THRAX_STRINGFILE_H_
#define THRAX_STRINGFILE_H_

#include <iostream>
#include <string>
#include <vector>
using std::vector;

#include <thrax/compat/utils.h>
#include <fst/fstlib.h>
#include <fst/string.h>
#include <thrax/fst-node.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/symbols.h>
#include <thrax/algo/prefix_tree.h>
#include <thrax/compat/stlfunctions.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.
DECLARE_string(indir);  // From util/flags.cc.

namespace thrax {
namespace function {

template <typename Arc>
class StringFile : public Function<Arc> {
 public:
  typedef fst::VectorFst<Arc> MutableTransducer;
  typedef fst::PrefixTree<Arc> PrefixTree;
  typedef typename Arc::Label Label;

  StringFile() : Function<Arc>() {}
  virtual ~StringFile() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    if (args.size() < 1 || args.size() > 3) {
      std::cout << "StringFile: Expected 1-3 arguments but got " << args.size()
                << std::endl;
      return NULL;
    }
    if (!args[0]->is<string>()) {
      std::cout << "StringFile: Expected string (file) for argument 1"
                << std::endl;
      return NULL;
    }
    int imode = fst::StringCompiler<Arc>::BYTE;
    const fst::SymbolTable* isymbols = NULL;
    if (args.size() == 1) {
      // If the StringFile call doesn't specify a parse mode, but if
      // FLAGS_save_symbols is set, we should set the symbol table to byte
      // mode.
      if (FLAGS_save_symbols) isymbols = GetByteSymbolTable();
    } else if (args.size() > 1) {
      if (args[1]->is<string>()) {
        if (*args[1]->get<string>() == "utf8") {
          imode = fst::StringCompiler<Arc>::UTF8;
          if (FLAGS_save_symbols) isymbols = GetUtf8SymbolTable();
        } else {
          imode = fst::StringCompiler<Arc>::BYTE;
          if (FLAGS_save_symbols) isymbols = GetByteSymbolTable();
        }
      } else if (args[1]->is<fst::SymbolTable>()) {
        isymbols = args[1]->get<fst::SymbolTable>();
        imode = fst::StringCompiler<Arc>::SYMBOL;
      } else {
        std::cout << "StringFile: Invalid parse mode or symbol table "
                  << "for input symbols" << std::endl;
        return NULL;
      }
    }
    int omode = fst::StringCompiler<Arc>::BYTE;
    // If this is an acceptor then the output symbols are whatever the input
    // symbols are.
    const fst::SymbolTable* osymbols = isymbols;
    if (args.size() > 2) {
      if (args[2]->is<string>()) {
        if (*args[2]->get<string>() == "utf8") {
          omode = fst::StringCompiler<Arc>::UTF8;
          if (FLAGS_save_symbols) osymbols = GetUtf8SymbolTable();
        } else {
          omode = fst::StringCompiler<Arc>::BYTE;
          if (FLAGS_save_symbols) osymbols = GetByteSymbolTable();
        }
      } else if (args[2]->is<fst::SymbolTable>()) {
        osymbols = args[2]->get<fst::SymbolTable>();
        omode = fst::StringCompiler<Arc>::SYMBOL;
      } else {
        std::cout << "StringFile: Invalid parse mode or symbol table "
                  << "for output symbols" << std::endl;
        return NULL;
      }
    }
    const string& filename =
        JoinPath(FLAGS_indir, *args[0]->get<string>());

    File* fp = OpenOrDie(filename, "r");
    PrefixTree pt;
    string line;
    int linenum = 0;
    bool acceptor = true;
    for (InputBuffer ibuf(fp); ibuf.ReadLine(&line);
         /* ReadLine() automatically increments */) {
      vector<string> words = Split(line, "\t");
      size_t size = words.size();
      if (size == 0) {
        ++linenum;
        continue;
      }
      // TODO(rws): Add ability to include weights
      vector<Label> ilabels;
      vector<Label> olabels;
      if (size == 1) {
        ConvertStringToLabels(words[0], &ilabels, imode, isymbols);
        pt.Add(ilabels.begin(), ilabels.end(),
               ilabels.begin(), ilabels.end());
      } else if (size == 2) {
        ConvertStringToLabels(words[0], &ilabels, imode, isymbols);
        ConvertStringToLabels(words[1], &olabels, omode, osymbols);
        pt.Add(ilabels.begin(), ilabels.end(),
               olabels.begin(), olabels.end());
        acceptor = false;
      } else {
        std::cout << "StringFile: Possible ill-formed line " << linenum
                  << " in " << filename << std::endl;
        continue;
      }
      ++linenum;
    }

    MutableTransducer* fst = new MutableTransducer();
    pt.ToFst(fst);
    if (acceptor) {
      fst::Project(fst, fst::PROJECT_INPUT);
    } else {
      MutableTransducer copy(*fst);
      fst::Push<Arc, fst::REWEIGHT_TO_INITIAL>(copy, fst,
                                                       fst::kPushLabels);
    }
    fst::RmEpsilon(fst);
    fst::ArcSort(fst, arcsort_comparer_);

    if (FLAGS_save_symbols) {
      fst->SetInputSymbols(isymbols);
      fst->SetOutputSymbols(osymbols);
    }
    return new DataType(fst);
  }

 private:
  // Same functionality as in StringCompiler (nlp/fst), but that is private to
  // the StringCompiler class
  bool ConvertStringToLabels(const string &str,
                             vector<Label> *labels,
                             int token_type,
                             const fst::SymbolTable* syms) const {
    labels->clear();
    if (token_type == fst::StringCompiler<Arc>::BYTE) {
      for (size_t i = 0; i < str.size(); ++i)
        labels->push_back(static_cast<unsigned char>(str[i]));
    } else if (token_type == fst::StringCompiler<Arc>::UTF8) {
      return fst::UTF8StringToLabels(str, labels);
    } else {
      char *c_str = new char[str.size() + 1];
      str.copy(c_str, str.size());
      c_str[str.size()] = 0;
      vector<char *> vec;
      string separator = "\n" + FLAGS_fst_field_separator;
      fst::SplitToVector(c_str, separator.c_str(), &vec, true);
      for (size_t i = 0; i < vec.size(); ++i) {
        Label label;
        if (!ConvertSymbolToLabel(vec[i], &label, syms))
          return false;
        labels->push_back(label);
      }
      delete[] c_str;
    }
    return true;
  }

  // Similar replication of code from StringCompiler (nlp/fst)
  bool ConvertSymbolToLabel(const char *s, Label* output,
                            const fst::SymbolTable* syms) const {
    int64 n;
    if (syms) {
      n = syms->Find(s);
      if (n < 0) {
        VLOG(1) << "StringFile:ConvertSymbolToLabel: Symbol \"" << s
                << "\" is not mapped to any integer label, symbol table = "
                << syms->Name();
        return false;
      }
    } else {
      char *p;
      n = strtoll(s, &p, 10);
      if (p < s + strlen(s) || n < 0) {
        VLOG(1) << "StringFile::ConvertSymbolToLabel: Bad label integer "
                << "= \"" << s << "\"";
        return false;
      }
    }
    *output = n;
    return true;
  }

  static const fst::ILabelCompare<Arc> arcsort_comparer_;

  DISALLOW_COPY_AND_ASSIGN(StringFile<Arc>);
};

template <typename Arc>
const fst::ILabelCompare<Arc> StringFile<Arc>::arcsort_comparer_ =
    fst::ILabelCompare<Arc>();

}  // namespace function
}  // namespace thrax

#endif  // THRAX_STRINGFILE_H_

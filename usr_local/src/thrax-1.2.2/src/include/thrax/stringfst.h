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
// Author: ttai@google.com (Terry Tai)
//         rws@google.com (Richard Sproat)
//
// Creates a string FST given the string text.
//
// Note that error checking in this function may be harsher or missing, as this
// function cannot be called natively by the user but must go through the
// parser.

#ifndef THRAX_STRINGFST_H_
#define THRAX_STRINGFST_H_

#include <stdlib.h>
#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <utility>
using std::pair; using std::make_pair;
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <fst/fstlib.h>
#include <fst/string.h>
#include <thrax/fst-node.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/symbols.h>
#include <thrax/compat/stlfunctions.h>
#include <thrax/compat/registry.h>

DECLARE_bool(save_symbols);  // From util/flags.cc.
DECLARE_string(indir);  // From util/flags.cc.
DECLARE_string(initial_boundary_symbol);  // From stringfst.cc
DECLARE_string(final_boundary_symbol);  // From stringfst.cc

// Default values here for labels correspond to Unicode Private Use Area B
// (Plane 16): 100000 - 10FFFB.  The last two positions (10FFFC and 10FFFD) are
// reserved (by default) for the BOS and EOS symbols.
DECLARE_int64(generated_label_start_index);
DECLARE_int64(generated_label_end_index);

DECLARE_int64(initial_boundary_marker);  // Default = 10FFFC.
DECLARE_int64(final_boundary_marker);    // Default = 10FFFD.

namespace thrax {
namespace function {

template <typename Arc>
class StringFst : public Function<Arc> {
 public:
  typedef fst::VectorFst<Arc> MutableTransducer;

  StringFst() : Function<Arc>() {}
  virtual ~StringFst() {}

 protected:
  virtual DataType* Execute(const vector<DataType*>& args) {
    CHECK_GE(args.size(), 2);

    // Find the mode (and maybe the symbol table).
    enum fst::StringCompiler<Arc>::TokenType mode =
        fst::StringCompiler<Arc>::BYTE;
    const fst::SymbolTable* symtab = NULL;
    switch (*args[0]->get<int>()) {
      case StringFstNode::BYTE: {
        CHECK_EQ(args.size(), 2);
        mode = fst::StringCompiler<Arc>::BYTE;
        break;
      }
      case StringFstNode::UTF8: {
        CHECK_EQ(args.size(), 2);
        mode = fst::StringCompiler<Arc>::UTF8;
        break;
      }
      case StringFstNode::SYMBOL_TABLE: {
        CHECK_EQ(args.size(), 3);
        mode = fst::StringCompiler<Arc>::SYMBOL;
        if (!args[2]->is<fst::SymbolTable>()) {
          std::cout
              << "StringFst: Invalid symbol table for symbol table parse mode"
              << std::endl;
          return NULL;
        }
        symtab = args[2]->get<fst::SymbolTable>();
        break;
      }
      default: {
        LOG(FATAL) << "Unhandled parse mode.";
      }
    }
    fst::StringCompiler<Arc> compiler(mode, symtab);

    // Create a single state acceptor to serve as the basis onto which we'll
    // concatenate everything.
    MutableTransducer* fst(new MutableTransducer());
    fst->SetStart(fst->AddState());
    fst->SetFinal(fst->Start(), Arc::Weight::One());

    // Go through the actual text and process each block, escaping the
    // backslashed characters and generating labels if necessary.
    const string& text = *args[1]->get<string>();
    string chunk;            // Either the raw text to be added or the name of a
                             // symbol we wish to generate a label for.
    bool in_genlab = false;  // Whether we're currently in generate-label mode.
    for (int i = 0; i < text.length(); ++i) {
      char c = text[i];

      if (c == '[' && mode != fst::StringCompiler<Arc>::SYMBOL) {
        if (in_genlab) {
          std::cout << "StringFst: Cannot start new generated label while in "
                    << "previous label" << std::endl;
          delete fst;
          return NULL;
        }
        if (!AddBlock(&compiler, &chunk, fst)) {
          std::cout << "StringFst: Failed to compile chunk: " << chunk
                    << std::endl;
          delete fst;
          return NULL;
        }
        in_genlab = true;
      } else if (c == ']' && mode != fst::StringCompiler<Arc>::SYMBOL) {
        if (!in_genlab) {
          std::cout
              << "StringFst: Cannot terminate generated label without already "
              << "being in one" << std::endl;
          delete fst;
          return NULL;
        }
        for (int i = 0; i < chunk.length(); ++i) {
          if (isspace(chunk[i])) {
            std::cout << "StringFst: Cannot have labels containing whitespace: "
                      << chunk << std::endl;
            delete fst;
            return NULL;
          }
        }
        if (!AddGeneratedLabel(&chunk, fst, mode)) {
          std::cout << "StringFst: Failed to generate label: " << chunk
                    << std::endl;
          delete fst;
          return NULL;
        }
        in_genlab = false;
      } else {
        // Here, we're just going to add a single character to the current
        // chunk.
        if (c == '\\') {
          if (++i >= text.length()) {
            std::cout
                << "StringFst: Unterminated escaped character at string end"
                << std::endl;
            delete fst;
            return NULL;
          }
          c = text[i];
          switch (c) {
            case 'n': c = '\n';
                      break;
            case 'r': c = '\r';
                      break;
            case 't': c = '\t';
                      break;
          }
        }
        chunk += c;
      }
    }
    if (in_genlab) {
      std::cout << "StringFst: Unexpected string termination in generated label"
                << std::endl;
      delete fst;
      return NULL;
    }
    if (!AddBlock(&compiler, &chunk, fst)) {
      std::cout << "StringFst: Failed to compile chunk: " << chunk << std::endl;
      delete fst;
      return NULL;
    }

    fst::RmEpsilon(fst);
    if (FLAGS_save_symbols) {
      if (symtab) {
        fst->SetInputSymbols(symtab);
        fst->SetOutputSymbols(symtab);
      } else if (mode == fst::StringCompiler<Arc>::UTF8) {
        fst->SetInputSymbols(GetUtf8SymbolTable());
        fst->SetOutputSymbols(GetUtf8SymbolTable());
      } else {
        fst->SetInputSymbols(GetByteSymbolTable());
        fst->SetOutputSymbols(GetByteSymbolTable());
      }
    }
    return new DataType(fst);
  }

 public:
  // Returns a symbol table corresponding to the generated labels used thus far
  // in this compilation.  This returns NULL if there were no generated labels.
  //
  // If FLAGS_save_symbols is set, we also add these labels to the byte and utf8
  // symbol tables, so that these can get reassigned to the transducers as
  // appropriate on write-out. However, we only want to do this is this is a
  // top-level grammar and we are saving out the fars for this grammar ---
  // i.e. it is not being imported into another grammar. The boolean argument
  // top_level indicates this.
  static fst::SymbolTable* GetLabelSymbolTable(bool top_level) {
    fst::MutexLock lock(&map_mutex_);

    if (symbol_label_map_.empty())
      return NULL;

    fst::SymbolTable* symtab = new fst::SymbolTable("");
    for (Map::const_iterator i = symbol_label_map_.begin();
         i != symbol_label_map_.end(); ++i) {
      symtab->AddSymbol(i->first, i->second);
      if (FLAGS_save_symbols && top_level) {
        AddToByteSymbolTable(i->first, i->second);
        AddToUtf8SymbolTable(i->first, i->second);
      }
    }

    return symtab;
  }

  // This takes in a symbol table and merges it into the current symbol/label
  // map, returning true on success or failure if we encounter any conflicts.
  static bool MergeLabelSymbolTable(const fst::SymbolTable& symtab) {
    fst::MutexLock lock(&map_mutex_);

    bool success = true;
    for (int i = 0; i < symtab.NumSymbols(); ++i) {
      int64 label = symtab.GetNthKey(i);
      string symbol = symtab.Find(label);

      // Checks to see if we already have this label paired with this
      // symbol. FSTs associated with the incoming symbol table will get
      // remapped as needed.
      // Four possible outcomes:
      // 1) Neither label nor symbol exist: insert this new pair.
      // 2) Label exists but mapped to another symbol: generate new_label for
      //    the symbol, and add <label, new_label> to the remapping table.
      // 3) Symbol exists but with another old_label: reassign to old_label and
      //    add <label, old_label> to the remapping table.
      // 4) Both label and symbol exist: then we need to ask whether they have
      //    the same mapping.
      typename Map::iterator slx = symbol_label_map_.find(symbol);
      typename InverseMap::iterator lsx = label_symbol_map_.find(label);
      if (slx == symbol_label_map_.end() && lsx == label_symbol_map_.end()) {
        // Case 1: Both new
        symbol_label_map_.insert(Map::value_type(symbol, label));
        label_symbol_map_.insert(InverseMap::value_type(label, symbol));
        VLOG(2) << "Loaded symbol " << symbol << " with label " << label;
        if (next_label_ <= label)   // On success, keeps track of the
          next_label_ = label + 1;  // maximum + 1 for the next available label.
      } else if (slx == symbol_label_map_.end()) {
        // Case 2: symbol is new, but label is there and therefore mapped to
        // something else.
        int64 new_label = next_label_++;
        symbol_label_map_.insert(Map::value_type(symbol, new_label));
        label_symbol_map_.insert(InverseMap::value_type(new_label, symbol));
        remap_.insert(Remap::value_type(label, new_label));
        VLOG(2) << "Remapping " << symbol << " to new label " << new_label;
      } else if (lsx == label_symbol_map_.end()) {
        // Case 3: label is new, but symbol is there and therefore mapped to
        // something else.
        int64 old_label = slx->second;
        remap_.insert(Remap::value_type(label, old_label));
        VLOG(2) << "Remapping " << symbol << " to old label " << old_label;
      } else {
        // Case 4: Both symbol and label already exist.
        const string& old_symbol = lsx->second;
        int64 old_label = slx->second;
        if (symbol == old_symbol && label == old_label) {
          // Same, so ok and nothing to do.
          continue;
        } else if (label == old_label || symbol == old_symbol) {
          // symbol -> label gets you the right label, but label -> symbol
          // doesn't. Or the other way around. This should not happen.
          LOG(WARNING) << "Detected label mismatch: "
                       << symbol << " -> " << old_label << ", "
                       << label << " -> " << old_symbol;
          success = false;
        } else {
          // Both are there but assigned to other things
          remap_.insert(Remap::value_type(label, old_label));
          VLOG(2) << "Remapping " << symbol << " to old label " << old_label;
        }
      }
    }
    return success;
  }

  static void ClearRemap() { remap_.clear(); }

  // Returns the remap value, or fst::kNoLabel
  static int64 FindRemapLabel(int64 old_label) {
    typename Remap::iterator ix = remap_.find(old_label);
    if (ix == remap_.end()) return fst::kNoLabel;
    return ix->second;
  }

  // This stores the assigned label for the provided symbol (from the map) into
  // label and returns true on a successful lookup or false if the symbol isn't
  // found.
  static bool SymbolToGeneratedLabel(const string& symbol, int64* label) {
    int64* answer = FindOrNull(symbol_label_map_, symbol);

    if (!answer)
      return false;

    *label = *answer;
    return true;
  }

 private:
  bool AddBlock(fst::StringCompiler<Arc>* compiler,
                string* chunk,
                MutableTransducer* fst) {
    VLOG(3) << "Adding block: " << *chunk;
    MutableTransducer chunk_fst;
    if (!(*compiler)(*chunk, &chunk_fst))
      return false;

    fst::Concat(fst, chunk_fst);
    chunk->clear();
    return true;
  }

  inline void WarnSingleCharacter(const string& symbol, int64 label) {
    LOG(WARNING) << "Single character generated labels are deprecated "
                 << "since they introduce symbols that have more than one "
                 << "encoding." << std::endl << '"' << symbol << '"'
                 << " is being encoded as " << label << ".  "
                 << "We suggest replacing "
                 << "\"[" << symbol << "]\" with just " << '"' << symbol << '"'
                 << " in your grammar since single-characters enclosed in "
                 << "brackets will eventually be phased out." << std::endl;
  }

  static bool GetSingleUtf8Label(const string& symbol, int64* label) {
    vector<int64> labels;
    fst::UTF8StringToLabels(symbol, &labels);
    if (labels.size() == 1) {
      *label = labels[0];
      return true;
    }
    return false;
  }

  bool AddGeneratedLabel(string* symbol, MutableTransducer* fst,
                         enum fst::StringCompiler<Arc>::TokenType mode) {
    VLOG(3) << "Finding label for symbol: " << *symbol;
    int64 label;

    // First, we'll check to see if the symbol is actually just a number.  If
    // so, we'll just use that number as the label.
    char* remainder = NULL;
    label = strtol(symbol->c_str(), &remainder, 0);

    // So if we're at the end of the string, then we've found a valid (whole)
    // number.  Otherwise (i.e., in this case) we have other junk, so we'll
    // treat it as a generated text symbol.
    if (*remainder != '\0') {
      // First of all check to see if the symbol is one of the designated
      // beginning-of-string/end-of-string delimiters. We don't put these in the
      // symbol table, but hallucinate them on the fly:
      if (*symbol == FLAGS_initial_boundary_symbol) {
        label = FLAGS_initial_boundary_marker;
      } else if (*symbol == FLAGS_final_boundary_symbol) {
        label = FLAGS_final_boundary_marker;
        // Check if the user enclosed a single character in square brackets ---
        // either a single byte or, if in UTF8 mode, a single Unicode
        // character. We don't want to generate a new label in that case since
        // this will give two conflicting labels with the same name. Instead we
        // warn the user that this may be deprecated in future versions.
      } else if (symbol->size() == 1) {
        label = (*symbol)[0];
        WarnSingleCharacter(*symbol, label);
      } else if (mode == fst::StringCompiler<Arc>::UTF8 &&
                 GetSingleUtf8Label(*symbol, &label)) {
        WarnSingleCharacter(*symbol, label);
      } else {
        // Find the right label - either the one previously generated for the
        // symbol, or a new one that we'll cache now.
        std::pair<Map::iterator, bool> ret =
            symbol_label_map_.insert(Map::value_type(*symbol, next_label_));
        if (ret.second) {
          label = next_label_++;
          label_symbol_map_.insert(InverseMap::value_type(label, *symbol));
          VLOG(2) << "Assigned symbol " << *symbol << " to label " << label;
        } else {
          label = ret.first->second;
        }
        if (label > FLAGS_generated_label_end_index) {
          LOG(WARNING) << "Exceeded maximum number of generated labels.";
          return false;
        }
      }
    }
    // Create a machine with a single arc using that label.
    MutableTransducer genlab_fst;
    int p = genlab_fst.AddState();
    int q = genlab_fst.AddState();
    genlab_fst.SetStart(p);
    genlab_fst.AddArc(p, Arc(label, label, Arc::Weight::One(), q));
    genlab_fst.SetFinal(q, Arc::Weight::One());

    fst::Concat(fst, genlab_fst);
    symbol->clear();
    return true;
  }

  // This function clears the static map for symbols to labels.  This is just to
  // clean up between test runs. This is somewhat less appropriately named than
  // it used to be since it clears more than just the symbol_label_map_
  static void ClearSymbolLabelMapForTest() {
    fst::MutexLock lock(&map_mutex_);

    symbol_label_map_.clear();
    label_symbol_map_.clear();
    remap_.clear();
    next_label_ = FLAGS_generated_label_start_index;
  }

  typedef map<string, int64> Map;
  static Map symbol_label_map_;
  typedef map<int64, string> InverseMap;
  static InverseMap label_symbol_map_;
  typedef map<int64, int64> Remap;
  static Remap remap_;
  static int64 next_label_;
  static fst::Mutex map_mutex_;

  friend class CategoryTest;
  friend class FeatureTest;
  friend class FeatureVectorTest;
  friend class StringFstTest;
  DISALLOW_COPY_AND_ASSIGN(StringFst<Arc>);
};

template <typename Arc>
typename StringFst<Arc>::Map StringFst<Arc>::symbol_label_map_;
template <typename Arc>
typename StringFst<Arc>::InverseMap StringFst<Arc>::label_symbol_map_;
template <typename Arc>
typename StringFst<Arc>::Remap StringFst<Arc>::remap_;
template <typename Arc>
int64 StringFst<Arc>::next_label_ = FLAGS_generated_label_start_index;
template <typename Arc>
fst::Mutex StringFst<Arc>::map_mutex_;

}  // namespace function
}  // namespace thrax

#endif  // THRAX_STRINGFST_H_

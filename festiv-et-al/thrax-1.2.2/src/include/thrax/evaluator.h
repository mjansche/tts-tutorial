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

#ifndef THRAX_EVALUATOR_H_
#define THRAX_EVALUATOR_H_

#include <string.h>
#include <iostream> // NOLINT
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using std::vector;

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/compat/utils.h>
#include <fst/extensions/far/far.h>
#include <fst/concat.h>
#include <fst/arc.h>
#include <fst/fst.h>
#include <fst/vector-fst.h>
#include <fst/weight.h>
#include <thrax/collection-node.h>
#include <thrax/fst-node.h>
#include <thrax/function-node.h>
#include <thrax/grammar-node.h>
#include <thrax/identifier-node.h>
#include <thrax/import-node.h>
#include <thrax/node.h>
#include <thrax/return-node.h>
#include <thrax/rule-node.h>
#include <thrax/statement-node.h>
#include <thrax/string-node.h>
#include <thrax/grm-compiler.h>
#include <thrax/identifier-counter.h>
#include <thrax/printer.h>
#include <thrax/datatype.h>
#include <thrax/function.h>
#include <thrax/optimize.h>
#include <thrax/stringfst.h>
#include <thrax/symbols.h>
#include <thrax/namespace.h>
#include <thrax/walker.h>
#include <thrax/compat/stlfunctions.h>

DECLARE_bool(always_export);
DECLARE_bool(optimize_all_fsts);
DECLARE_bool(print_rules);
DECLARE_bool(save_symbols);
DECLARE_string(indir);

namespace thrax {

// The name of the special FST that holds the symbol table of the generated
// labels.  This name should be one disallowed by the variable naming rules
// (i.e., the user shouldn't be able to create this).
static const char kStringFstSymtabFst[] = "*StringFstSymbolTable";

// Retrieves a C++ function for the proper arc type.
// See evaluator-specializations.cc for implementations.
template <typename Arc>
function::Function<Arc>* GetFunction(const string& func_name);
template <>
function::Function<fst::StdArc>* GetFunction(const string& func_name);
template <>
function::Function<fst::LogArc>* GetFunction(const string& func_name);

template <typename Arc> class GrmCompilerSpec;

template <typename Arc>
class AstEvaluator : public AstWalker {
 public:
  typedef fst::Fst<Arc> Transducer;
  typedef fst::VectorFst<Arc> MutableTransducer;
  typedef map<int64, int64> LabelMapper;

  // This constructor sets up the evaluator to run all nodes using a new
  // environment namespace.
  AstEvaluator()
      : AstWalker(), env_(new Namespace()), id_counter_(NULL), run_all_(true),
        return_value_(NULL), success_(true), optimize_embedding_(-1) {
    // This primary namespace should be the one that corresponds to the main
    // file being compiled.
    env_->SetTopLevel();

    // If we're parsing the entire file, then we need some space for local
    // variables (as we actually execute the body).
    env_->PushLocalEnvironment();

  }

  // This constructor will only run import and function nodes, loading them into
  // the provided namespace.
  explicit AstEvaluator(Namespace* env)
      : AstWalker(), env_(env), id_counter_(NULL), run_all_(false),
        return_value_(NULL), success_(true), optimize_embedding_(-1) {}

  virtual ~AstEvaluator() {
    // We only own the environment if we ran all of the nodes.  Similarly, if we
    // run all of the nodes then we should've created one layer of local
    // variable space.  Finally, the top-level grammar should own all of the
    // parsed ASTs via the grammars, so we'll kill those, too.
    if (run_all_) {
      env_->PopLocalEnvironment();
      delete env_;
      STLDeleteElements(&loaded_grammars_);
    }
    delete id_counter_;
  }

  // Three functions to control error handling in the case of bad input
  // grammars.
  void ClearSuccess() {  // Call this before a new compilation to reset the
    success_ = true;     // state of the success_ variable.
  }
  bool Success() {
    return success_;
  }
  void Error(const Node& node, const string& message) {
    std::cout << file_ << ":" << node.getline() << ": " << message << std::endl;
    success_ = false;
  }

  void SetIdCounter(AstIdentifierCounter* counter) {
    id_counter_ = counter;
  }

  virtual void Visit(CollectionNode* node) {
    LOG(FATAL) << "CollectionNode should not be visited.  Use the parent node.";
  }

  virtual void Visit(FunctionNode* node) {
    VLOG(2) << "Visiting Function";
    if (!Success()) return;

    const string& name = node->GetName()->Get();
    IdentifierNode function_inode(name);
    if (!env_->Get<FunctionNode>(function_inode)) {  // Add only if new.
      // The functions are held with the GrmCompilerSpec (more specifically the
      // abstract syntax node tree).  So, we'll insert them without deletion
      // now, and kill them all at the end when we dispose of the grammar
      // compilers.
      bool new_add = env_->InsertWithoutDelete(name, node);
      observed_function_names_.insert(name);
      CHECK(new_add)
          ; // NOLINT
    } else {
      if (observed_function_names_.find(name) !=
          observed_function_names_.end()) {
        LOG(WARNING) << "Duplicate function definition within file for "
                     << name << ". Ignoring.";
      }
    }
  }

  virtual void Visit(GrammarNode* node) {
    VLOG(2) << "Visiting GrammarNode";
    if (!Success()) return;

    CollectionNode* imports = node->GetImports();
    for (int i = 0; i < imports->Size(); ++i)
      (*imports)[i]->Accept(this);

    CollectionNode* functions = node->GetFunctions();
    for (int i = 0; i < functions->Size(); ++i)
      (*functions)[i]->Accept(this);

    if (run_all_) {
      CollectionNode* statements = node->GetStatements();
      for (int i = 0; i < statements->Size(); ++i) {
        StatementNode* stmt = static_cast<StatementNode*>((*statements)[i]);
        if (stmt->GetType() == StatementNode::RETURN_STATEMENTNODE) {
          Error(*stmt, "Cannot return from main body");
          return;
        }
        stmt->Accept(this);
      }
    }
  }

  virtual void Visit(IdentifierNode* node) {
    LOG(FATAL) << "IdentifierNode should not be visited.  Use Get() instead.";
  }

  virtual void Visit(ImportNode* node) {
    if (!Success()) return;

    // Get (and check) the path of the actual source and far.
    const string& path = JoinPath(FLAGS_indir, node->GetPath()->Get());
    if (Suffix(path) != "grm") {
      Error(*node,
            StrCat("Extension for included files should be .grm: ", path));
      return;
    }

    // Get (and check) the alias name.
    const string& alias = node->GetAlias()->Get();
    if (alias.find('.') != string::npos) {
      Error(*node, StrCat("Invalid import alias identifier: ", alias));
      return;
    }

    Namespace* prev_env_ = env_;
    env_ = env_->AddSubNamespace(path, alias);

    // First load up the function source into the local environment.
    VLOG(2) << "Opening (and parsing) imported source file: " << path;
    if (!Readable(path)) {
      Error(*node, StrCat("Unable to open grm source file: ", path));
      env_ = prev_env_;
      return;
    }
    GrmCompilerSpec<Arc>* grammar = new GrmCompilerSpec<Arc>();
    if (!grammar->ParseFile(path) ||
        !grammar->EvaluateAstWithEnvironment(env_, false)) {
      Error(*node, StrCat("Errors while importing grm source file: ", path));
      env_ = prev_env_;
      return;
    }

    loaded_grammars_.push_back(grammar);

    // Then, load up the exported FSTs - beginning by opening the FST archive.
    string far_path = path.substr(0, path.length() - 3) + "far";
    VLOG(2) << "Opening (and loading FSTs from) companion far: " << far_path;
    fst::FarReader<Arc>* far_reader =
        fst::STTableFarReader<Arc>::Open(far_path);
    if (!far_reader) {
      Error(*node, StrCat("Unable to open far archive: ", far_path));
      // We don't need to return or cleanup, as the next code will check for
      // Success() first, jumping to cleanup on failure.
    }

    // First of all look for the special FST that holds the generated labels
    // symbol table.  We must process that first so that we can know if any of
    // the labels on the incoming FSTs need to be reset.
    if (!far_reader->Done() && far_reader->Find(kStringFstSymtabFst)) {
      // First clear the remap, since any remappings that need to be done only
      // safely apply to the current FAR.
      function::StringFst<Arc>::ClearRemap();
      if (!function::StringFst<Arc>::MergeLabelSymbolTable(
              *far_reader->GetFst()->InputSymbols())) {
        Error(*node, "Failed to merge symbol tables");
        // We can gracefully exit this loop when success_ becomes false and do
        // the necessary cleanup afterwards.
      }
      far_reader->Reset();
    }
    // Then, add the FSTs to that namespace.  We'll loop through the reader, but
    // quit if we ever have an error.
    for (/* far_reader starts at the beginning */;
         Success() && !far_reader->Done(); far_reader->Next()) {
      const string& key = far_reader->GetKey();
      if (key == kStringFstSymtabFst) {
        continue;
      } else {
        // Otherwise, we just have a normal exported FST.  So we can just add it
        // into the variables.
        IdentifierNode key_inode(key);
        if (!env_->Get<DataType>(key_inode)) {  // Add only if new.
          // Must be mutable for now in case we need to change the symbol table.
          MutableTransducer tmpfst(*(far_reader->GetFst()));
          RemapGeneratedLabels(&tmpfst);
          ReassignSymbols(&tmpfst);
          Transducer* fst = tmpfst.Copy();
          bool new_add = env_->Insert(key, new DataType(fst));
          CHECK(new_add)
              ; // NOLINT
        }
      }
    }

    // Restore the previous namespace and kill the FAR reader.
    env_ = prev_env_;
    delete far_reader;  // On error, this'll be NULL but the delete is safe.
  }

  // Set the return_value_ pointer to the FST described by the FstNode.
  virtual void Visit(FstNode* node) {
    VLOG(2) << "Visiting FstNode";
    if (!Success()) return;

    CHECK(return_value_ == NULL);
    return_value_ = MakeFst(node);
  }

  virtual void Visit(RepetitionFstNode* node) {
    Visit(static_cast<FstNode*>(node));
  }

  // This is pretty much a pass through that creates and binds the FST specified
  // in the ReturnNode.
  virtual void Visit(ReturnNode* node) {
    VLOG(2) << "Visiting ReturnNode";
    if (!Success()) return;

    node->Get()->Accept(this);
  }

  virtual void Visit(RuleNode* node) {
    VLOG(2) << "Visiting RuleNode";
    if (!Success()) return;

    IdentifierNode* identifier = node->GetName();
    if (FLAGS_print_rules)
      std::cout << "Evaluating rule: " << identifier->Get() << std::endl;
    if (identifier->HasNamespaces()) {
      Error(*identifier,
            StrCat("Cannot assign to an identifier within a namespace: ",
                   identifier->Get()));
      return;
    }

    const string& name = identifier->GetIdentifier();
    node->Get()->Accept(this);
    DataType* thing = GetReturnValue();

    // Insert the new variable, dying if it clobbers a pre-existing object.
    if (!env_->InsertLocal(name, thing)) {
      Error(*identifier, StrCat("Cannot clobber existing variable: ", name));
      return;
    }
    if (node->ShouldExport()) {
      if (env_->LocalEnvironmentDepth() == 1) {
        exported_fsts_.insert(identifier);
      } else if (!FLAGS_always_export) {
        Error(*identifier,
              StrCat("Variables may only be exported from the top-level "
                     "grammar: ", name));
        return;
      }
    }
  }

  // Visiting a StatementNode simply passes on the Accept() request to the
  // underlying node.
  virtual void Visit(StatementNode* node) {
    VLOG(2) << "Visiting StatementNode";
    if (!Success()) return;

    node->Get()->Accept(this);
  }

  virtual void Visit(StringFstNode* node) {
    Visit(static_cast<FstNode*>(node));
  }

  // Sets the return_value_ with the value of the string.
  virtual void Visit(StringNode* node) {
    VLOG(2) << "Visiting StringNode";
    if (!Success()) return;

    CHECK(return_value_ == NULL);
    return_value_ = new DataType(node->Get());
  }

  // Inserts the exported FSTs from this file into the provided map.
  // Preexisting data with the same key will be clobbered.
  //
  // Boolean argument top_level indicates whether or not the Evaluator is
  // currently dealing with a top level grammar file (i.e. not an imported
  // grammar). This information gets passed down ultimately to StringFst's
  // GetLabelSymbolTable to determine (assuming --save_symbols is set), whether
  // or not to add generated labels to the byte and utf8 symbol tables.
  void GetFsts(map<string, const Transducer*>* fsts, bool top_level) {
    // Check if we ever used generated labels.  If so, get the symbol table and
    // add it to a unique FST called kStringFstSymtabFst.
    fst::SymbolTable* generated_labels =
        function::StringFst<Arc>::GetLabelSymbolTable(top_level);
    if (generated_labels) {
      MutableTransducer* label_fst = new MutableTransducer();
      label_fst->SetInputSymbols(generated_labels);
      (*fsts)[kStringFstSymtabFst] = label_fst;
      delete generated_labels;
    }

    // Get the exported FSTs and add them to the map.
    for (set<IdentifierNode*>::const_iterator fst_i = exported_fsts_.begin();
         fst_i != exported_fsts_.end(); ++fst_i) {
      const string& name = (*fst_i)->Get();
      // If always_export is set, an imported function, which in turn contains
      // a named fst variable, will have that variable exported when the
      // function is expanded in the top-level grammar, but that fst is not
      // really part of the top-level environment, so it fails to find it
      // here and will die. The choices are to try to make the compilation more
      // clever about this in the first place, which is hairy, or catch it
      // here.
      if (env_->Get<DataType>(**fst_i) == nullptr) {
        LOG(WARNING) << "Cannot find exportable fst with name "
                     << name
                     << ": ignoring.";
        continue;
      }
      VLOG(1) << "Expanding FST: " << name;
      if (!env_->Get<DataType>(**fst_i)->is<Transducer*>()) {
        Error(**fst_i,
              StrCat("Cannot export non-FST variable: ", (*fst_i)->Get()));
        return;
      }
      Transducer* fst = *env_->Get<DataType>(**fst_i)->get<Transducer*>();
      MutableTransducer *nfst = new MutableTransducer(*fst);
      // If the transducer has input symbols or output symbols, and if those are
      // either the byte symbol table or the utf8 symbol table, then we must
      // reassign those tables, since we may have added generated labels. In the
      // worst case this is a no-op.
      ReassignSymbols(nfst);
      (*fsts)[(*fst_i)->Get()] = nfst;
    }
  }

  void set_file(const string& file) { file_ = file; }

 private:
  // Extract and interpret the proper arguments from an FstNode and bind them to
  // the DataType union.  On failure, we'll return NULL.
  vector<DataType*>* GetArgumentsFromFstNode(FstNode* node, int num_arguments) {
    vector<DataType*>* args = new vector<DataType*>();
    for (int i = 0; i < num_arguments; ++i) {
      node->GetArgument(i)->Accept(this);
      DataType* return_value = GetReturnValue();
      if (!return_value) {
        STLDeleteElements(args);
        delete args;
        return NULL;
      }
      args->push_back(return_value);
    }
    return args;
  }

  // Do the same as above, but using a CollectionNode instead.
  vector<DataType*>* GetArgumentsFromCollectionNode(CollectionNode* node) {
    vector<DataType*>* args = new vector<DataType*>();
    for (int i = 0; i < node->Size(); ++i) {
      node->Get(i)->Accept(this);
      DataType* return_value = GetReturnValue();
      if (!return_value) {
        STLDeleteElements(args);
        delete args;
        return NULL;
      }
      args->push_back(return_value);
    }
    return args;
  }

  // Given a function node and an enclosing namespace, this will first bind the
  // arguments (taking ownership of the arguments) and then execute the function
  // AST in the new namespace, returning the output DataType at the end.  The
  // provided arguments are owned by this function.
  DataType* MakeFstFromLocalFunction(FunctionNode* func_node,
                                     const Node& debug_location_node,
                                     Namespace* func_namespace,
                                     vector<DataType*>* arguments) {
    Namespace* prev_env_ = env_;
    env_ = func_namespace;
    env_->PushLocalEnvironment();

    // Create a new layer of environment symbol table and bind the passed
    // arguments.
    CollectionNode* fa_node = func_node->GetArguments();
    if (fa_node->Size() != arguments->size()) {
      Error(debug_location_node,
            StrCat("Expected ", fa_node->Size(), " arguments but got ",
                   arguments->size()));
    }
    for (int i = 0; Success() && i < fa_node->Size(); ++i) {
      IdentifierNode* fa_identifier =
          static_cast<IdentifierNode*>((*fa_node)[i]);
      if (fa_identifier->HasNamespaces()) {
        Error(*fa_identifier,
              StrCat("Invalid function argument: ", fa_identifier->Get()));
        break;
      }
      const string& arg_name = fa_identifier->GetIdentifier();
      DataType* arg = (*arguments)[i]->Copy();
      env_->InsertLocal(arg_name, arg);
    }

    // Iterate over the statements and run each.
    CollectionNode* fb_node = func_node->GetBody();
    DataType* output = NULL;
    for (int i = 0; Success() && i < fb_node->Size(); ++i) {
      StatementNode* stmt = static_cast<StatementNode*>((*fb_node)[i]);
      stmt->Accept(this);
      if (stmt->GetType() == StatementNode::RETURN_STATEMENTNODE) {
        output = GetReturnValue();
        break;
      }
    }

    // Toss out the function-scope environment.
    env_->PopLocalEnvironment();
    env_ = prev_env_;

    STLDeleteElements(arguments);
    delete arguments;

    return output;
  }

  // Looks up a C++ function by name from the registerer and then executes that
  // function on that provided arguments.  This function takes ownership of the
  // arguments (via the C++ functions).  This returns the new object saved into
  // the output argument.  This function returns true if the function was found
  // and false otherwise.
  bool MakeFstFromCFunction(const string& function_name,
                            const Node& debug_location_node,
                            vector<DataType*>* arguments,
                            DataType** output) {
    function::Function<Arc>* func = GetFunction<Arc>(function_name);
    if (!func)       // If we get a NULL function, then the name was invalid,
      return false;  // so we'll return false.

    *output = func->Run(arguments);
    if (!*output)
      Error(debug_location_node, "C++ function call failed");

    return true;
  }

  // The main evaluator function - this takes in an FstNode and returns the
  // object specified.  Note that this is a misnomer (for historical reasons) -
  // we can actually return things other than an FST (whatever DataType
  // supports).
  DataType* MakeFst(FstNode* node) {
    DataType* output;
    switch (node->GetType()) {
      case FstNode::CONCAT_FSTNODE: {
        VLOG(2) << "Concat Fst:";
        vector<DataType*>* args = GetArgumentsFromFstNode(node, 2);
        CHECK(MakeFstFromCFunction("Concat", *node, args, &output));
        break;
      }
      case FstNode::UNION_FSTNODE: {
        VLOG(2) << "Union Fst:";
        vector<DataType*>* args = GetArgumentsFromFstNode(node, 2);
        CHECK(MakeFstFromCFunction("Union", *node, args, &output));
        break;
      }
      case FstNode::DIFFERENCE_FSTNODE: {
        VLOG(2) << "Difference Fst:";
        vector<DataType*>* args = GetArgumentsFromFstNode(node, 2);
        CHECK(MakeFstFromCFunction("Difference", *node, args, &output));
        break;
      }
      case FstNode::REWRITE_FSTNODE: {
        VLOG(2) << "Rewrite Fst:";
        vector<DataType*>* args = GetArgumentsFromFstNode(node, 2);
        CHECK(MakeFstFromCFunction("Rewrite", *node, args, &output));
        break;
      }
      case FstNode::REPETITION_FSTNODE: {
        VLOG(2) << "Repetition Fst:";
        vector<DataType*>* args = GetArgumentsFromFstNode(node, 1);

        RepetitionFstNode* rnode = static_cast<RepetitionFstNode*>(node);
        args->push_back(
            new DataType(static_cast<int>(rnode->GetRepetitionType())));
        if (rnode->GetRepetitionType() == RepetitionFstNode::RANGE) {
          int min, max;
          rnode->GetRange(&min, &max);
          args->push_back(new DataType(min));
          args->push_back(new DataType(max));
        }

        CHECK(MakeFstFromCFunction("Closure", *node, args, &output));
        break;
      }
      case FstNode::COMPOSITION_FSTNODE: {
        VLOG(2) << "Composition Fst:";
        if (optimize_embedding_ > -1) optimize_embedding_++;
        if (optimize_embedding_ > 1) node->SetOptimize();
        vector<DataType*>* args = GetArgumentsFromFstNode(node, 2);
        args->push_back(new DataType("right"));  // ArcSort the right FST.
        CHECK(MakeFstFromCFunction("Compose", *node, args, &output));
        break;
      }
      case FstNode::IDENTIFIER_FSTNODE: {
        IdentifierNode* identifier =
            static_cast<IdentifierNode*>(node->GetArgument(0));
        VLOG(2) << "Identifier Fst: " << identifier->Get();
        DataType* original = env_->Get<DataType>(*identifier);
        if (!original) {
          Error(*identifier, StrCat("Undefined symbol: ", identifier->Get()));
          return NULL;
        }
        output = original->Copy();

        // If we're currently at the top level namespace (i.e., compiling the
        // main body of the primary compilation target), and if the identifier
        // is one without aliases, then we may wish to free the memory if this
        // is the final access.
        if (env_->IsTopLevel() &&
            env_->LocalEnvironmentDepth() == 1 &&
            !identifier->HasNamespaces() &&
            !id_counter_->Decrement(identifier->GetIdentifier())) {
          VLOG(3) << "Erasing local variable: " << identifier->GetIdentifier();
          CHECK(env_->EraseLocal(identifier->GetIdentifier()));
        }

        break;
      }
      case FstNode::STRING_FSTNODE: {
        StringFstNode* snode = static_cast<StringFstNode*>(node);
        const string& text =
            static_cast<StringNode*>(node->GetArgument(0))->Get();
        VLOG(2) << "String Fst: " << text;

        vector<DataType*>* args = new vector<DataType*>(2);
        (*args)[0] = new DataType(static_cast<int>(snode->GetParseMode()));
        (*args)[1] = new DataType(text);

        if (snode->GetParseMode() == StringFstNode::SYMBOL_TABLE) {
          // In the case of the symbol table, we need to evaluate its node and
          // pick up the actual table.  Error checking will be handled by the
          // StringFst function.
          snode->GetArgument(1)->Accept(this);
          DataType* thing = GetReturnValue();
          args->push_back(thing);  // args takes ownership of the DataType.
        }

        CHECK(MakeFstFromCFunction("StringFst", *node, args, &output));
        break;
      }
      case FstNode::FUNCTION_FSTNODE: {
        IdentifierNode* func_identifier_node =
            static_cast<IdentifierNode*>(node->GetArgument(0));
        const string& name = func_identifier_node->Get();
        VLOG(2) << "Function Call Fst: " << name;
        if (name == "Optimize") optimize_embedding_ = 0;

        vector<DataType*>* args = GetArgumentsFromCollectionNode(
            static_cast<CollectionNode*>(node->GetArgument(1)));
        if (!Success() || !args) {
          Error(*func_identifier_node,
                StrCat("Unable to bind all arguments for function call: ",
                       func_identifier_node->Get()));
          return NULL;
        }

        bool function_found = false;
        Namespace* func_namespace;
        FunctionNode* func_node =
            env_->Get<FunctionNode>(*func_identifier_node, &func_namespace);
        output = NULL;
        if (func_node) {
          // Get the function and make sure the names match and then call the
          // function.
          CHECK_EQ(func_node->GetName()->Get(),
                    func_identifier_node->GetIdentifier());
          output = MakeFstFromLocalFunction(
              func_node, *func_identifier_node, func_namespace, args);
          function_found = true;
        } else if (!func_identifier_node->HasNamespaces()) {
          // If we didn't find a natural function with that name, then we might
          // have a C++ function, provided that the identifier has no
          // namespaces.
          if (MakeFstFromCFunction(func_identifier_node->GetIdentifier(),
                                   *func_identifier_node,
                                   args,
                                   &output))
            function_found = true;
        }

        if (!function_found) {
          Error(*func_identifier_node, StrCat("Undefined function identifier: ",
                                              func_identifier_node->Get()));
          return NULL;
        }

        // Now that we are done, set the optimize_embedding_ setting back to the
        // default.
        optimize_embedding_ = -1;
        break;
      }
      default: {
        LOG(FATAL) << "Unknown FstNode type: " << node->GetType();
      }
    }

    // If we have an FST as the output, then we might have some extra
    // post-processing.
    if (output && output->is<Transducer*>()) {
      // First, we can attach the weight if appropriate.
      if (node->HasWeight()) {
        Transducer* unweighted_fst = *output->get<Transducer*>();
        Transducer* weighted_fst =
            AttachWeight(*unweighted_fst, node->GetWeight());
        delete output;
        output = new DataType(weighted_fst);
      }

      // Now, we might wish to always optimize the FSTs, or the node is slated
      // to be optimized (e.g. a composition node within an Optimize
      // FUNCTION_FSTNODE).
      if (FLAGS_optimize_all_fsts || node->ShouldOptimize()) {
        Transducer* optimized_fst = function::Optimize<Arc>::ActuallyOptimize(
            **output->get<Transducer*>());
        delete output;
        output = new DataType(optimized_fst);
        // This is the interesting case to be able to keep track of.
        if (node->ShouldOptimize())
          VLOG(2) << "Optimizing at line " << node->getline();
      }
    }
    return output;
  }

  // Releases control of the return_value_ and returns it.
  DataType* GetReturnValue() {
    DataType* ret = return_value_;
    return_value_ = NULL;
    return ret;
  }

  // Given an FST, we'll create a new one with the weight tacked on as a final
  // state.
  static Transducer* AttachWeight(const Transducer& input_fst,
                                  const string& weight_str) {
    // Generate the appropriate weight.
    typename Arc::Weight weight(0);
    istringstream iss(weight_str);
    iss >> weight;  // Interpret the weight instead of reading it as raw bytes.

    // Then, create a single state FST with the appropriate final state weight.
    MutableTransducer weight_fst;
    int state = weight_fst.AddState();
    weight_fst.SetStart(state);
    weight_fst.SetFinal(state, weight);

    // If we are saving symbols then we have to add the symbol tables of our
    // input fst to this new single state FST
    if (FLAGS_save_symbols) {
      weight_fst.SetInputSymbols(input_fst.InputSymbols());
      weight_fst.SetOutputSymbols(input_fst.OutputSymbols());
    }

    // Glue on the actual input FST to the front; this will automatically set
    // the weight properly.
    return new fst::ConcatFst<Arc>(input_fst, weight_fst);
  }

  // If FLAGS_save_symbols is set for the system the imported fsts will have
  // symbol tables.  If these are either the byte symbol table or the utf8
  // symbol table, they may have additional generated labels associated with
  // them.  These generated labels will cause the symbol tables to potentially
  // be in conflict with other symbol tables either from other imported fsts, or
  // from the ones created in this grammar.  Since all of the generated labels
  // from all the imported files as well as the ones generated in this grammar
  // are stored in the StringFst class, and since these will be added back in to
  // any byte or utf8 symbol tables on write-out, the easiest thing to do is
  // reset the symbol tables to be just the basic byte or utf8 symbol tables,
  // and then when the new fsts are written out, reconstruct all of the
  // generated labels in those symbol tables.
  void ReassignSymbols(MutableTransducer* fst) {
    if (fst->InputSymbols()) {
      if (fst->InputSymbols()->Name() ==
          function::kByteSymbolTableName) {
        fst->SetInputSymbols(function::GetByteSymbolTable());
      } else if (fst->InputSymbols()->Name() ==
                 function::kUtf8SymbolTableName) {
        fst->SetInputSymbols(function::GetUtf8SymbolTable());
      }
    }
    if (fst->OutputSymbols()) {
      if (fst->OutputSymbols()->Name() ==
          function::kByteSymbolTableName) {
        fst->SetOutputSymbols(function::GetByteSymbolTable());
      } else if (fst->OutputSymbols()->Name() ==
                 function::kUtf8SymbolTableName) {
        fst->SetOutputSymbols(function::GetUtf8SymbolTable());
      }
    }
  }

  // Remap the generated labels of this fst using a StringFst's remap
  void RemapGeneratedLabels(MutableTransducer* fst) {
    for (fst::StateIterator<MutableTransducer> siter(*fst);
         !siter.Done();
         siter.Next()) {
      typename Arc::StateId s = siter.Value();
      for (fst::MutableArcIterator<MutableTransducer> aiter(fst, s);
           !aiter.Done();
           aiter.Next()) {
        Arc arc = aiter.Value();
        int64 label = function::StringFst<Arc>::FindRemapLabel(arc.ilabel);
        if (label != fst::kNoLabel) arc.ilabel = label;
        label = function::StringFst<Arc>::FindRemapLabel(arc.olabel);
        if (label != fst::kNoLabel) arc.olabel = label;
        aiter.SetValue(arc);
      }
    }
  }

  Namespace* env_;
  AstIdentifierCounter* id_counter_;
  bool run_all_;

  // A list of the names of the FSTs we want exported at the end.  We'll find
  // these FSTs from the local environment.  Note that these pointers are owned
  // by the original AST, not us.
  set<IdentifierNode*> exported_fsts_;

  // A list of grammars that we've opened (and thus need to close at the end).
  // TODO(ttai): This is dangerous right now since if we have two simulatneous
  // compilations within the same process, then they'll be incorrectly sharing
  // this.  It'd be nice to have a copy-function on the AST nodes so we can just
  // make a copy of the relevant bits of the AST (the function nodes) and store
  // just that.
  static vector<GrmCompilerSpec<Arc>*> loaded_grammars_;

  // This is the "return" datatype that is returned by a number of nodes.
  DataType* return_value_;

  AstPrinter printer_;

  // Signified whether the compilation (evaluation) was a success.
  bool success_;

  // Name of file that we're currently evaluating.
  string file_;

  // Level of embedding under Optimize[] of a composition. If it is more than 1,
  // then optimize the composition. (If it is 1, then it's at the top level
  // under the Optimize[] and it will be optimized anyway.
  int optimize_embedding_;

  // Used in the evaluator to keep track of whether the same function name has
  // been defined in the current file more than once.
  set<string> observed_function_names_;

  DISALLOW_COPY_AND_ASSIGN(AstEvaluator<Arc>);
};

template <typename Arc>
vector<GrmCompilerSpec<Arc>*> AstEvaluator<Arc>::loaded_grammars_;

}  // namespace thrax

#endif  // THRAX_EVALUATOR_H_

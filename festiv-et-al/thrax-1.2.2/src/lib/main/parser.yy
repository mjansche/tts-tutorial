%{
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
// All Rights Reserved.
//
// Author: wojciech@google.com (Wojciech Skut)
//         ttai@google.com (Terry Tai)
//         rws@google.com (Richard Sproat)
//
// Bison parser for FST generation.

#include <fst/compat.h>
#include <thrax/compat/compat.h>
#include <thrax/compat/utils.h>
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
#include <thrax/lexer.h>

DECLARE_bool(always_export);

using namespace thrax;



#define CTRL (static_cast<GrmCompilerParserInterface*>(parm))

class FuncOrStmt {
public:
  CollectionNode *funcs_;
  CollectionNode *stmts_;
};

namespace thrax_rewriter {
  int yylex(void *, void *parm);
     void yyerror(void *parm, const char *s);

/* For some reason the brain-dead new version of bison *generates* code
   that depends on this definition, but does not actually generate
   a definition for this. I have not figured out how to get around that
   in a cleaner way. */

void yyerror(void *parm, const char *s) {
   std::cout << "Parse Failed: " << s << std::endl;
}
%}

%define api.pure full
%parse-param { void *parm }
%lex-param { void *parm }

%union {
  int                        int_type;
  FunctionNode*              function_node_type;
  ImportNode*                import_node_type;
  ReturnNode*                return_node_type;
  RuleNode*                  rule_node_type;
  IdentifierNode*            identifier_node_type;
  CollectionNode*            collection_node_type;
  StatementNode*             statement_node_type;
  FstNode*                   fst_node_type;
  StringNode*                string_node_type;
  GrammarNode*               grammar_node_type;
  Node*                      node_type;
  FuncOrStmt*                function_or_statement_node_type;
}

%type <grammar_node_type>         grammar
%type <function_or_statement_node_type>  func_or_stmt_list
%type <collection_node_type>      stmt_list
%type <statement_node_type>       stmt
%type <return_node_type>          return_stmt
%type <rule_node_type>            rule_stmt
%type <node_type>                 rule_body
%type <string_node_type>          quoted_string
%type <string_node_type>          quoted_fst_string
%type <string_node_type>          weight
%type <identifier_node_type>      descriptor

%type <function_node_type>        func_decl
%type <collection_node_type>      func_arglist
%type <collection_node_type>      func_arguments
%type <collection_node_type>      func_body

%type <collection_node_type>      import_list
%type <import_node_type>          import_request

%type <node_type>                 obj               // Can be FstNode or StringNode.
%type <node_type>                 atomic_obj
%type <node_type>                 concat_fst
%type <node_type>                 union_fst
%type <node_type>                 difference_fst
%type <node_type>                 grouped_obj
%type <node_type>                 repetition_fst
%type <node_type>                 composition_fst
%type <node_type>                 funccall_obj
%type <collection_node_type>      funccall_arglist
%type <collection_node_type>      funccall_arguments
%type <node_type>                 identifier_obj
%type <node_type>                 string_fst
%type <node_type>                 fst_with_output
%type <node_type>                 fst_with_weight

%type <int_type>                  number

%token tDESCR tDQSTRING tQSTRING tINTEGER tFLOAT tLPAREN tRPAREN
       tLBRACE tRBRACE tPIPE tSTAR tPLUS tQMARK tDOT
       tCOMMA tAT tCOLON tSEMICOLON tEQUALS tCONCAT tUNARY
       tDOLLAR tLBRACKET tRBRACKET tANGLE_STRING
       tSLASH tUNDERSCORE tMINUS tREAD_STRINGS
       tREAD_WEIGHTED_STRINGS tCASE_INSENSITIVE
       tKEYWORD_EXPORT tKEYWORD_FUNC tKEYWORD_RETURN
       tKEYWORD_BYTE tKEYWORD_UTF8
       tKEYWORD_AS tKEYWORD_IMPORT

%left tPIPE
%left tCOLON
%left tMINUS
%left tCONCAT
%left tUNARY

%%

grammar:
  import_list func_or_stmt_list
    { GrammarNode* node = new GrammarNode($1, $2->funcs_, $2->stmts_);
      delete $2;
      $$ = node;
      CTRL->SetAst(static_cast<Node*>($$)); }
| error
    { CTRL->Error("Generic parsing error.");
      $$ = NULL; }
;

import_list:
  /* empty */
    { $$ = new CollectionNode(); }
| import_request import_list
    { $2->AddFront($1);
      $$ = $2; }
;

func_or_stmt_list:
  /* empty */
    { $$ = new FuncOrStmt();
      $$->funcs_ = new CollectionNode();
      $$->stmts_ = new CollectionNode();
    }
| stmt func_or_stmt_list
    { $2->stmts_->AddFront($1);
      $$ = $2; }
| func_decl func_or_stmt_list
    { $2->funcs_->AddFront($1);
      $$ = $2; }
;

stmt_list:
  /* empty */
    { $$ = new CollectionNode(); }
| stmt stmt_list
    { $2->AddFront($1);
      $$ = $2; }
;

stmt:
  rule_stmt
    { StatementNode* node = new StatementNode(StatementNode::RULE_STATEMENTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->Set($1);
      $$ = node; }
| return_stmt
    { StatementNode* node = new StatementNode(StatementNode::RETURN_STATEMENTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->Set($1);
      $$ = node; }
| import_request
    { CTRL->Error("import statements must occur in the first block of the grammar.");
      $$ = NULL; }
| error tSEMICOLON
    { CTRL->Error("Invalid statement (or previous statement).");
      $$ = NULL; }
;

return_stmt:
  tKEYWORD_RETURN obj tSEMICOLON
    { ReturnNode* node = new ReturnNode($2);
      $$ = node; }
;

rule_stmt:
  descriptor tEQUALS rule_body tSEMICOLON
    { RuleNode* node = new RuleNode($1, $3,
                                    FLAGS_always_export ?
                                    RuleNode::EXPORT :
                                    RuleNode::DO_NOT_EXPORT);
      $$ = node; }
| tKEYWORD_EXPORT descriptor tEQUALS rule_body tSEMICOLON
    { RuleNode* node = new RuleNode($2, $4, RuleNode::EXPORT);
      $$ = node; }
;

rule_body:
  obj
    { $$ = $1; }
;

descriptor:
  tDESCR
    { const string& name = CTRL->GetLexer()->YYString();
      int begin_pos = CTRL->GetLexer()->YYBeginPos();
      IdentifierNode* node = new IdentifierNode(name, begin_pos);
      node->SetLine(CTRL->GetLexer()->line_number());
      if (!node->IsValid())
        CTRL->Error(StringPrintf("Illegal identifier: %s", name.c_str()));
      $$ = node; }
;

atomic_obj:
  quoted_string    { $$ = $1; }
| string_fst       { $$ = $1; }
| identifier_obj   { $$ = $1; }
| funccall_obj     { $$ = $1; }
| grouped_obj      { $$ = $1; }
;

obj:
  fst_with_weight { $$ = $1; }
;

// Concatenation: L R
concat_fst:
  repetition_fst concat_fst %prec tCONCAT
    { FstNode* node = new FstNode(FstNode::CONCAT_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument($1);
      node->AddArgument($2);
      $$ = node; }
| repetition_fst
    { $$ = $1; }
;

// Union: L|R
union_fst:
  composition_fst tPIPE union_fst
    { FstNode* node = new FstNode(FstNode::UNION_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument($1);
      node->AddArgument($3);
      $$ = node; }
| composition_fst
    { $$ = $1; }
;

// Difference: L-R
difference_fst:
  difference_fst tMINUS concat_fst
    { FstNode* node = new FstNode(FstNode::DIFFERENCE_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument($1);
      node->AddArgument($3);
      $$ = node; }
| concat_fst
    { $$ = $1; }
;

// Parentheses: (R)
grouped_obj:
  tLPAREN obj tRPAREN
    { $$ = $2; }
;

// Function Call: f[thing]
funccall_obj:
  descriptor funccall_arglist
    { FstNode* node = new FstNode(FstNode::FUNCTION_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument($1);
      node->AddArgument($2);
      $$ = node; }

funccall_arglist:
  tLBRACKET tRBRACKET
    { $$ = new CollectionNode(); }
| tLBRACKET funccall_arguments tRBRACKET
    { $$ = $2; }
;

funccall_arguments:
  obj
    { CollectionNode* node = new CollectionNode();
      node->AddFront($1);
      $$ = node; }
| obj tCOMMA funccall_arguments
    { $3->AddFront($1);
      $$ = $3; }
// TODO(rws): These are only needed by StringFile. There should be a better way
// to do this.
| tKEYWORD_BYTE
    { CollectionNode* node = new CollectionNode();
      StringNode* nnode = new StringNode("byte");
      node->AddFront(nnode);
      $$ = node; }
| tKEYWORD_BYTE tCOMMA funccall_arguments
    { StringNode* nnode = new StringNode("byte");
      $3->AddFront(nnode);
      $$ = $3; }
| tKEYWORD_UTF8
    { CollectionNode* node = new CollectionNode();
      StringNode* nnode = new StringNode("utf8");
      node->AddFront(nnode);
      $$ = node; }
| tKEYWORD_UTF8 tCOMMA funccall_arguments
    { StringNode* nnode = new StringNode("utf8");
      $3->AddFront(nnode);
      $$ = $3; }
;

// Repetition: R*, R+, R?, R{d,d}, R{d}
repetition_fst:
  atomic_obj tSTAR %prec tUNARY
    { RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::STAR);
      node->AddArgument($1);
      $$ = node; }
| atomic_obj tPLUS %prec tUNARY
    { RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::PLUS);
      node->AddArgument($1);
      $$ = node; }
| atomic_obj tQMARK %prec tUNARY
    { RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::QUESTION);
      node->AddArgument($1);
      $$ = node; }
| atomic_obj tLBRACE number tCOMMA number tRBRACE
    { if ($3 > $5)
        CTRL->Error(StringPrintf("Reversed repetition bounds: %d > %d", $3, $5));
      if ($3 < 0)
        CTRL->Error(StringPrintf("Start bound must be non-negative: %d", $3));
      if ($5 < 0)
        CTRL->Error(StringPrintf("End bound must be non-negative: %d", $5));
      RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::RANGE);
      node->AddArgument($1);
      node->SetRange($3, $5);
      $$ = node; }
| atomic_obj tLBRACE number tRBRACE
    { if ($3 < 0)
        CTRL->Error(StringPrintf("Repetition count should be non-negative: %d", $3));
      RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::RANGE);
      node->AddArgument($1);
      node->SetRange($3, $3);
      $$ = node; }
| atomic_obj
    { $$ = $1; }
;

// Composition: A @ B
composition_fst:
  composition_fst tAT difference_fst
    { FstNode* node = new FstNode(FstNode::COMPOSITION_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument($1);
      node->AddArgument($3);
      $$ = node; }
| difference_fst
    { $$ = $1; }
;

identifier_obj:
  descriptor
    { FstNode* node = new FstNode(FstNode::IDENTIFIER_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument($1);
      $$ = node; }
;

string_fst:
  quoted_fst_string
    { StringFstNode* node = new StringFstNode(StringFstNode::BYTE);
      node->AddArgument($1);
      node->SetLine($1->getline());  // Get the line from the actual text line.
      $$ = node; }
| quoted_fst_string tDOT tKEYWORD_BYTE
    { StringFstNode* node = new StringFstNode(StringFstNode::BYTE);
      node->AddArgument($1);
      node->SetLine($1->getline());
      $$ = node; }
| quoted_fst_string tDOT tKEYWORD_UTF8
    { StringFstNode* node = new StringFstNode(StringFstNode::UTF8);
      node->AddArgument($1);
      node->SetLine($1->getline());
      $$ = node; }
| quoted_fst_string tDOT identifier_obj
    { StringFstNode* node = new StringFstNode(StringFstNode::SYMBOL_TABLE);
      node->AddArgument($1);
      node->AddArgument($3);
      node->SetLine($1->getline());
      $$ = node; }
| quoted_fst_string tDOT funccall_obj
    { StringFstNode* node = new StringFstNode(StringFstNode::SYMBOL_TABLE);
      node->AddArgument($1);
      node->AddArgument($3);
      node->SetLine($1->getline());
      $$ = node; }
;

fst_with_output:
  union_fst tCOLON union_fst
    { FstNode* node = new FstNode(FstNode::REWRITE_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument($1);
      node->AddArgument($3);
      $$ = node; }
| union_fst
    { $$ = $1; }
;

fst_with_weight:
  fst_with_output weight
    // TODO: Figure out how to limit outselves to just getting a known FstNode
    // instead of a generic Node.  The current implementation is dangerous.
    { if (!static_cast<FstNode*>($1)->SetWeight($2))
        CTRL->Error("Rules may have only one weight.");
      $$ = $1; }
| fst_with_output
    { $$ = $1; }
;

// TODO: Decide if we should actually use a number node instead of a direct
// number.  Compare this to how we handle quoted strings and weights...
number:
  tINTEGER
    { $$ = atoi(CTRL->GetLexer()->YYString().c_str()); }
;

quoted_fst_string:
  tDQSTRING
    { StringNode* node = new StringNode(CTRL->GetLexer()->YYString());
      node->SetLine(CTRL->GetLexer()->line_number());
      $$ = node; }
;

quoted_string:
  tQSTRING
    { StringNode* node = new StringNode(CTRL->GetLexer()->YYString());
      node->SetLine(CTRL->GetLexer()->line_number());
      $$ = node; }
;

weight:
  tANGLE_STRING
    { StringNode* node = new StringNode(CTRL->GetLexer()->YYString());
      node->SetLine(CTRL->GetLexer()->line_number());
      $$ = node; }
;

import_request:
  tKEYWORD_IMPORT quoted_string tKEYWORD_AS descriptor tSEMICOLON
    { ImportNode* node = new ImportNode($2, $4);
      node->SetLine(CTRL->GetLexer()->line_number());
      $$ = node; }
| error tSEMICOLON
    { CTRL->Error("Invalid import statement.");
      $$ = NULL; }
;

func_decl:
  tKEYWORD_FUNC descriptor func_arglist func_body
    { FunctionNode* node = new FunctionNode($2, $3, $4);
      node->SetLine(node->GetName()->getline());  // Use the identifier's location.
      $$ = node; }
| error tRBRACE
    { CTRL->Error("Invalid function declaration.");
      $$ = NULL; }
;

func_arglist:
  tLBRACKET tRBRACKET
    { $$ = new CollectionNode(); }
| tLBRACKET func_arguments tRBRACKET
    { $$ = $2; }
;

func_arguments:
  descriptor
    { CollectionNode* node = new CollectionNode();
      node->AddFront($1);
      $$ = node; }
| descriptor tCOMMA func_arguments
    { $3->AddFront($1);
      $$ = $3; };
;

func_body:
  tLBRACE stmt_list tRBRACE
    { $$ = $2; }
;

%%

// Local definitions, definitions that need the semantics of the
// token/type defines.

// yylex() calls Lexer::YYLex()
int yylex(void *, void *parm) {
  switch(CTRL->GetLexer()->YYLex()) {
    case Lexer::EOS:
      return 0;
    case Lexer::QUOTED_STRING:
      return tQSTRING;
    case Lexer::DOUBLE_QUOTED_STRING:
      return tDQSTRING;
    case Lexer::DESCRIPTOR:
      return tDESCR;
    case Lexer::INTEGER:
      return tINTEGER;
    case Lexer::FLOAT:
      return tFLOAT;
    case Lexer::ANGLE_STRING:
      return tANGLE_STRING;
    case Lexer::CONNECTOR: {
      string connector = CTRL->GetLexer()->YYString();
      if (connector.length() != 1) {
        CTRL->Error(StringPrintf("Parse error - unknown connector: %s", connector.c_str()));
        return 0;
      }
      switch (CTRL->GetLexer()->YYString()[0]) {
        case '$': return tDOLLAR;
        case '(': return tLPAREN;
        case ')': return tRPAREN;
        case '*': return tSTAR;
        case '+': return tPLUS;
        case ',': return tCOMMA;
        case '-': return tMINUS;
        case '.': return tDOT;
        case '/': return tSLASH;
        case ':': return tCOLON;
        case ';': return tSEMICOLON;
        case '=': return tEQUALS;
        case '?': return tQMARK;
        case '@': return tAT;
        case '[': return tLBRACKET;
        case ']': return tRBRACKET;
        case '_': return tUNDERSCORE;
        case '{': return tLBRACE;
        case '}': return tRBRACE;
        case '|': return tPIPE;
        default:  CTRL->Error(StringPrintf("Parse error - unknown connector: %s", connector.c_str()));
                  return 0;
      }
    }
    case Lexer::KEYWORD: {
      string keyword = CTRL->GetLexer()->YYString();
      if (keyword == "export") {
        return tKEYWORD_EXPORT;
      } else if (keyword == "as") {
        return tKEYWORD_AS;
      } else if (keyword == "func") {
        return tKEYWORD_FUNC;
      } else if (keyword == "import") {
        return tKEYWORD_IMPORT;
      } else if (keyword == "return") {
        return tKEYWORD_RETURN;
      } else if (keyword == "byte") {
        return tKEYWORD_BYTE;
      } else if (keyword == "utf8") {
        return tKEYWORD_UTF8;
      } else {
        CTRL->Error(StringPrintf("Parse error - unknown keyword: %s", keyword.c_str()));
        return 0;
      }
    }
  }
  return 0;
}

int yyerror(const char *s) {
  std::cout << "Parse Failed: " << s << std::endl;
  return 0;
}

} // namespace thrax_rewriter

namespace thrax {

int CallParser(GrmCompilerParserInterface* compiler) {
  return thrax_rewriter::yyparse(compiler);
}

}  // namespace thrax

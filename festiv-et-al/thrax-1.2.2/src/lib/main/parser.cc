/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "main/parser.yy" /* yacc.c:339  */

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

#line 133 "main/parser.cc" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    tDESCR = 258,
    tDQSTRING = 259,
    tQSTRING = 260,
    tINTEGER = 261,
    tFLOAT = 262,
    tLPAREN = 263,
    tRPAREN = 264,
    tLBRACE = 265,
    tRBRACE = 266,
    tPIPE = 267,
    tSTAR = 268,
    tPLUS = 269,
    tQMARK = 270,
    tDOT = 271,
    tCOMMA = 272,
    tAT = 273,
    tCOLON = 274,
    tSEMICOLON = 275,
    tEQUALS = 276,
    tCONCAT = 277,
    tUNARY = 278,
    tDOLLAR = 279,
    tLBRACKET = 280,
    tRBRACKET = 281,
    tANGLE_STRING = 282,
    tSLASH = 283,
    tUNDERSCORE = 284,
    tMINUS = 285,
    tREAD_STRINGS = 286,
    tREAD_WEIGHTED_STRINGS = 287,
    tCASE_INSENSITIVE = 288,
    tKEYWORD_EXPORT = 289,
    tKEYWORD_FUNC = 290,
    tKEYWORD_RETURN = 291,
    tKEYWORD_BYTE = 292,
    tKEYWORD_UTF8 = 293,
    tKEYWORD_AS = 294,
    tKEYWORD_IMPORT = 295
  };
#endif
/* Tokens.  */
#define tDESCR 258
#define tDQSTRING 259
#define tQSTRING 260
#define tINTEGER 261
#define tFLOAT 262
#define tLPAREN 263
#define tRPAREN 264
#define tLBRACE 265
#define tRBRACE 266
#define tPIPE 267
#define tSTAR 268
#define tPLUS 269
#define tQMARK 270
#define tDOT 271
#define tCOMMA 272
#define tAT 273
#define tCOLON 274
#define tSEMICOLON 275
#define tEQUALS 276
#define tCONCAT 277
#define tUNARY 278
#define tDOLLAR 279
#define tLBRACKET 280
#define tRBRACKET 281
#define tANGLE_STRING 282
#define tSLASH 283
#define tUNDERSCORE 284
#define tMINUS 285
#define tREAD_STRINGS 286
#define tREAD_WEIGHTED_STRINGS 287
#define tCASE_INSENSITIVE 288
#define tKEYWORD_EXPORT 289
#define tKEYWORD_FUNC 290
#define tKEYWORD_RETURN 291
#define tKEYWORD_BYTE 292
#define tKEYWORD_UTF8 293
#define tKEYWORD_AS 294
#define tKEYWORD_IMPORT 295

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 72 "main/parser.yy" /* yacc.c:355  */

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

#line 266 "main/parser.cc" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *parm);



/* Copy the second part of user declarations.  */

#line 280 "main/parser.cc" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   126

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  72
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  116

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   145,   145,   150,   157,   158,   165,   169,   172,   179,
     180,   186,   191,   196,   199,   205,   211,   217,   223,   228,
     239,   240,   241,   242,   243,   247,   252,   258,   264,   270,
     276,   282,   288,   294,   302,   304,   309,   313,   318,   323,
     327,   332,   340,   344,   348,   352,   363,   370,   376,   382,
     387,   395,   400,   405,   410,   416,   425,   431,   436,   442,
     449,   454,   461,   468,   475,   479,   485,   489,   495,   497,
     502,   506,   512
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "tDESCR", "tDQSTRING", "tQSTRING",
  "tINTEGER", "tFLOAT", "tLPAREN", "tRPAREN", "tLBRACE", "tRBRACE",
  "tPIPE", "tSTAR", "tPLUS", "tQMARK", "tDOT", "tCOMMA", "tAT", "tCOLON",
  "tSEMICOLON", "tEQUALS", "tCONCAT", "tUNARY", "tDOLLAR", "tLBRACKET",
  "tRBRACKET", "tANGLE_STRING", "tSLASH", "tUNDERSCORE", "tMINUS",
  "tREAD_STRINGS", "tREAD_WEIGHTED_STRINGS", "tCASE_INSENSITIVE",
  "tKEYWORD_EXPORT", "tKEYWORD_FUNC", "tKEYWORD_RETURN", "tKEYWORD_BYTE",
  "tKEYWORD_UTF8", "tKEYWORD_AS", "tKEYWORD_IMPORT", "$accept", "grammar",
  "import_list", "func_or_stmt_list", "stmt_list", "stmt", "return_stmt",
  "rule_stmt", "rule_body", "descriptor", "atomic_obj", "obj",
  "concat_fst", "union_fst", "difference_fst", "grouped_obj",
  "funccall_obj", "funccall_arglist", "funccall_arguments",
  "repetition_fst", "composition_fst", "identifier_obj", "string_fst",
  "fst_with_output", "fst_with_weight", "number", "quoted_fst_string",
  "quoted_string", "weight", "import_request", "func_decl", "func_arglist",
  "func_arguments", "func_body", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295
};
# endif

#define YYPACT_NINF -75

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-75)))

#define YYTABLE_NINF -10

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       1,   -13,    11,    15,    10,    18,   -75,   -75,     8,   -75,
       3,   -75,    59,    59,    73,   -75,    10,   -75,   -75,    46,
     -75,    10,   -13,   -75,    59,   -75,   -75,    47,    49,   -75,
      73,    55,    69,    65,   -75,    72,    56,   -75,   -75,    73,
      13,   -75,   -75,    68,   -75,    76,   -75,   -75,    73,   -75,
      74,    73,    25,    86,    91,    61,   -75,    95,   -75,   -75,
     -75,   -75,    73,    73,   -75,    73,    73,   -75,   -75,     2,
      82,   -75,   -75,    83,   -75,    89,    81,    21,   -75,   -75,
     -75,    92,    93,    94,    87,   -75,    32,   -75,   -75,   -75,
      56,   -75,   -75,   -75,   -75,   -75,   -75,    59,   -75,    88,
     101,    21,    67,    67,    67,   -75,   -75,    95,   -75,   -75,
     -75,   -75,   -75,   -75,   103,   -75
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     3,     0,     0,     0,     0,    65,    62,     0,     1,
       0,    19,     0,     0,     0,     2,     0,    12,    11,     0,
      13,     0,     0,     5,     0,    67,    14,     0,     0,    61,
       0,    50,    47,     0,    31,    57,    49,    24,    23,    27,
      29,    22,    21,    59,    25,    51,    20,     7,     0,     8,
       0,     0,     0,     0,     0,     0,    33,     0,    42,    43,
      44,    15,     0,     0,    26,     0,     0,    63,    58,     0,
       0,    18,    64,     0,    68,    70,     0,     0,    66,    32,
      34,    38,    40,    36,     0,    60,     0,    56,    30,    28,
      48,    52,    53,    55,    54,    16,    17,     0,    69,     0,
       0,     0,     0,     0,     0,    35,    46,     0,    71,    72,
      10,    39,    41,    37,     0,    45
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -75,   -75,   110,    17,    16,   -74,   -75,   -75,    70,    -4,
     -75,    12,   -33,    -6,    50,   -75,    51,   -75,   -14,   -75,
     -75,    53,   -75,   -75,   -75,    19,   -75,   116,   -75,    29,
     -75,   -75,    22,   -75
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,    15,   100,    16,    17,    18,    70,    31,
      32,    83,    34,    35,    36,    37,    38,    56,    84,    39,
      40,    41,    42,    43,    44,    86,    45,    46,    68,    20,
      21,    53,    76,    78
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      19,    -4,     1,   101,    -4,    11,    64,     6,    27,    28,
      -6,    10,    19,    11,    25,     9,     7,    19,    -4,    22,
      50,    -4,    99,    26,    11,    65,    33,   101,    11,     5,
      88,    66,    -9,    47,     5,    -4,    -4,    -4,    49,    91,
      92,     2,    54,   106,    12,    13,    14,    24,    75,   107,
       2,    74,    -4,    -4,    -4,    12,    87,    14,     2,    89,
      71,     2,    11,    71,    11,    29,     7,    48,    51,    30,
      11,    29,     7,    19,    52,    30,    11,    29,     7,    57,
      55,    30,    58,    59,    60,    61,    63,    80,   111,   112,
     113,    62,    69,    75,    72,    67,    77,    19,    81,    82,
      79,    85,    95,    96,    81,    82,    97,    98,    26,   102,
     103,   104,   109,   105,   115,    23,    90,   110,     8,   108,
      93,    73,    94,     0,     0,     0,   114
};

static const yytype_int8 yycheck[] =
{
       4,     0,     1,    77,     3,     3,    39,    20,    12,    13,
       0,     1,    16,     3,    11,     0,     5,    21,     0,     1,
      24,     3,     1,    20,     3,    12,    14,   101,     3,     0,
      63,    18,    11,    16,     5,    34,    35,    36,    21,    37,
      38,    40,    30,    11,    34,    35,    36,    39,    52,    17,
      40,    26,    34,    35,    36,    34,    62,    36,    40,    65,
      48,    40,     3,    51,     3,     4,     5,    21,    21,     8,
       3,     4,     5,    77,    25,     8,     3,     4,     5,    10,
      25,     8,    13,    14,    15,    20,    30,    26,   102,   103,
     104,    19,    16,    97,    20,    27,    10,   101,    37,    38,
       9,     6,    20,    20,    37,    38,    17,    26,    20,    17,
      17,    17,    11,    26,    11,     5,    66,   101,     2,    97,
      69,    51,    69,    -1,    -1,    -1,   107
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    40,    42,    43,    70,    20,     5,    68,     0,
       1,     3,    34,    35,    36,    44,    46,    47,    48,    50,
      70,    71,     1,    43,    39,    11,    20,    50,    50,     4,
       8,    50,    51,    52,    53,    54,    55,    56,    57,    60,
      61,    62,    63,    64,    65,    67,    68,    44,    21,    44,
      50,    21,    25,    72,    52,    25,    58,    10,    13,    14,
      15,    20,    19,    30,    53,    12,    18,    27,    69,    16,
      49,    52,    20,    49,    26,    50,    73,    10,    74,     9,
      26,    37,    38,    52,    59,     6,    66,    54,    53,    54,
      55,    37,    38,    57,    62,    20,    20,    17,    26,     1,
      45,    46,    17,    17,    17,    26,    11,    17,    73,    11,
      45,    59,    59,    59,    66,    11
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    41,    42,    42,    43,    43,    44,    44,    44,    45,
      45,    46,    46,    46,    46,    47,    48,    48,    49,    50,
      51,    51,    51,    51,    51,    52,    53,    53,    54,    54,
      55,    55,    56,    57,    58,    58,    59,    59,    59,    59,
      59,    59,    60,    60,    60,    60,    60,    60,    61,    61,
      62,    63,    63,    63,    63,    63,    64,    64,    65,    65,
      66,    67,    68,    69,    70,    70,    71,    71,    72,    72,
      73,    73,    74
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     0,     2,     0,     2,     2,     0,
       2,     1,     1,     1,     2,     3,     4,     5,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     3,     1,
       3,     1,     3,     2,     2,     3,     1,     3,     1,     3,
       1,     3,     2,     2,     2,     6,     4,     1,     3,     1,
       1,     1,     3,     3,     3,     3,     3,     1,     2,     1,
       1,     1,     1,     1,     5,     2,     4,     2,     2,     3,
       1,     3,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (parm, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, parm); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *parm)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (parm);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *parm)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, parm);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void *parm)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , parm);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, parm); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *parm)
{
  YYUSE (yyvaluep);
  YYUSE (parm);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *parm)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, parm);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 146 "main/parser.yy" /* yacc.c:1646  */
    { GrammarNode* node = new GrammarNode((yyvsp[-1].collection_node_type), (yyvsp[0].function_or_statement_node_type)->funcs_, (yyvsp[0].function_or_statement_node_type)->stmts_);
      delete (yyvsp[0].function_or_statement_node_type);
      (yyval.grammar_node_type) = node;
      CTRL->SetAst(static_cast<Node*>((yyval.grammar_node_type))); }
#line 1460 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 3:
#line 151 "main/parser.yy" /* yacc.c:1646  */
    { CTRL->Error("Generic parsing error.");
      (yyval.grammar_node_type) = NULL; }
#line 1467 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 4:
#line 157 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.collection_node_type) = new CollectionNode(); }
#line 1473 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 5:
#line 159 "main/parser.yy" /* yacc.c:1646  */
    { (yyvsp[0].collection_node_type)->AddFront((yyvsp[-1].import_node_type));
      (yyval.collection_node_type) = (yyvsp[0].collection_node_type); }
#line 1480 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 165 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.function_or_statement_node_type) = new FuncOrStmt();
      (yyval.function_or_statement_node_type)->funcs_ = new CollectionNode();
      (yyval.function_or_statement_node_type)->stmts_ = new CollectionNode();
    }
#line 1489 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 170 "main/parser.yy" /* yacc.c:1646  */
    { (yyvsp[0].function_or_statement_node_type)->stmts_->AddFront((yyvsp[-1].statement_node_type));
      (yyval.function_or_statement_node_type) = (yyvsp[0].function_or_statement_node_type); }
#line 1496 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 8:
#line 173 "main/parser.yy" /* yacc.c:1646  */
    { (yyvsp[0].function_or_statement_node_type)->funcs_->AddFront((yyvsp[-1].function_node_type));
      (yyval.function_or_statement_node_type) = (yyvsp[0].function_or_statement_node_type); }
#line 1503 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 9:
#line 179 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.collection_node_type) = new CollectionNode(); }
#line 1509 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 181 "main/parser.yy" /* yacc.c:1646  */
    { (yyvsp[0].collection_node_type)->AddFront((yyvsp[-1].statement_node_type));
      (yyval.collection_node_type) = (yyvsp[0].collection_node_type); }
#line 1516 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 11:
#line 187 "main/parser.yy" /* yacc.c:1646  */
    { StatementNode* node = new StatementNode(StatementNode::RULE_STATEMENTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->Set((yyvsp[0].rule_node_type));
      (yyval.statement_node_type) = node; }
#line 1525 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 12:
#line 192 "main/parser.yy" /* yacc.c:1646  */
    { StatementNode* node = new StatementNode(StatementNode::RETURN_STATEMENTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->Set((yyvsp[0].return_node_type));
      (yyval.statement_node_type) = node; }
#line 1534 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 197 "main/parser.yy" /* yacc.c:1646  */
    { CTRL->Error("import statements must occur in the first block of the grammar.");
      (yyval.statement_node_type) = NULL; }
#line 1541 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 200 "main/parser.yy" /* yacc.c:1646  */
    { CTRL->Error("Invalid statement (or previous statement).");
      (yyval.statement_node_type) = NULL; }
#line 1548 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 206 "main/parser.yy" /* yacc.c:1646  */
    { ReturnNode* node = new ReturnNode((yyvsp[-1].node_type));
      (yyval.return_node_type) = node; }
#line 1555 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 212 "main/parser.yy" /* yacc.c:1646  */
    { RuleNode* node = new RuleNode((yyvsp[-3].identifier_node_type), (yyvsp[-1].node_type),
                                    FLAGS_always_export ?
                                    RuleNode::EXPORT :
                                    RuleNode::DO_NOT_EXPORT);
      (yyval.rule_node_type) = node; }
#line 1565 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 218 "main/parser.yy" /* yacc.c:1646  */
    { RuleNode* node = new RuleNode((yyvsp[-3].identifier_node_type), (yyvsp[-1].node_type), RuleNode::EXPORT);
      (yyval.rule_node_type) = node; }
#line 1572 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 224 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1578 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 229 "main/parser.yy" /* yacc.c:1646  */
    { const string& name = CTRL->GetLexer()->YYString();
      int begin_pos = CTRL->GetLexer()->YYBeginPos();
      IdentifierNode* node = new IdentifierNode(name, begin_pos);
      node->SetLine(CTRL->GetLexer()->line_number());
      if (!node->IsValid())
        CTRL->Error(StringPrintf("Illegal identifier: %s", name.c_str()));
      (yyval.identifier_node_type) = node; }
#line 1590 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 239 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].string_node_type); }
#line 1596 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 240 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1602 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 241 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1608 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 242 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1614 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 243 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1620 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 247 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1626 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 253 "main/parser.yy" /* yacc.c:1646  */
    { FstNode* node = new FstNode(FstNode::CONCAT_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument((yyvsp[-1].node_type));
      node->AddArgument((yyvsp[0].node_type));
      (yyval.node_type) = node; }
#line 1636 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 27:
#line 259 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1642 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 28:
#line 265 "main/parser.yy" /* yacc.c:1646  */
    { FstNode* node = new FstNode(FstNode::UNION_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument((yyvsp[-2].node_type));
      node->AddArgument((yyvsp[0].node_type));
      (yyval.node_type) = node; }
#line 1652 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 271 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1658 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 277 "main/parser.yy" /* yacc.c:1646  */
    { FstNode* node = new FstNode(FstNode::DIFFERENCE_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument((yyvsp[-2].node_type));
      node->AddArgument((yyvsp[0].node_type));
      (yyval.node_type) = node; }
#line 1668 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 283 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1674 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 289 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[-1].node_type); }
#line 1680 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 33:
#line 295 "main/parser.yy" /* yacc.c:1646  */
    { FstNode* node = new FstNode(FstNode::FUNCTION_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument((yyvsp[-1].identifier_node_type));
      node->AddArgument((yyvsp[0].collection_node_type));
      (yyval.node_type) = node; }
#line 1690 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 303 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.collection_node_type) = new CollectionNode(); }
#line 1696 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 305 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.collection_node_type) = (yyvsp[-1].collection_node_type); }
#line 1702 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 310 "main/parser.yy" /* yacc.c:1646  */
    { CollectionNode* node = new CollectionNode();
      node->AddFront((yyvsp[0].node_type));
      (yyval.collection_node_type) = node; }
#line 1710 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 314 "main/parser.yy" /* yacc.c:1646  */
    { (yyvsp[0].collection_node_type)->AddFront((yyvsp[-2].node_type));
      (yyval.collection_node_type) = (yyvsp[0].collection_node_type); }
#line 1717 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 319 "main/parser.yy" /* yacc.c:1646  */
    { CollectionNode* node = new CollectionNode();
      StringNode* nnode = new StringNode("byte");
      node->AddFront(nnode);
      (yyval.collection_node_type) = node; }
#line 1726 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 324 "main/parser.yy" /* yacc.c:1646  */
    { StringNode* nnode = new StringNode("byte");
      (yyvsp[0].collection_node_type)->AddFront(nnode);
      (yyval.collection_node_type) = (yyvsp[0].collection_node_type); }
#line 1734 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 40:
#line 328 "main/parser.yy" /* yacc.c:1646  */
    { CollectionNode* node = new CollectionNode();
      StringNode* nnode = new StringNode("utf8");
      node->AddFront(nnode);
      (yyval.collection_node_type) = node; }
#line 1743 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 333 "main/parser.yy" /* yacc.c:1646  */
    { StringNode* nnode = new StringNode("utf8");
      (yyvsp[0].collection_node_type)->AddFront(nnode);
      (yyval.collection_node_type) = (yyvsp[0].collection_node_type); }
#line 1751 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 341 "main/parser.yy" /* yacc.c:1646  */
    { RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::STAR);
      node->AddArgument((yyvsp[-1].node_type));
      (yyval.node_type) = node; }
#line 1759 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 345 "main/parser.yy" /* yacc.c:1646  */
    { RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::PLUS);
      node->AddArgument((yyvsp[-1].node_type));
      (yyval.node_type) = node; }
#line 1767 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 44:
#line 349 "main/parser.yy" /* yacc.c:1646  */
    { RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::QUESTION);
      node->AddArgument((yyvsp[-1].node_type));
      (yyval.node_type) = node; }
#line 1775 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 45:
#line 353 "main/parser.yy" /* yacc.c:1646  */
    { if ((yyvsp[-3].int_type) > (yyvsp[-1].int_type))
        CTRL->Error(StringPrintf("Reversed repetition bounds: %d > %d", (yyvsp[-3].int_type), (yyvsp[-1].int_type)));
      if ((yyvsp[-3].int_type) < 0)
        CTRL->Error(StringPrintf("Start bound must be non-negative: %d", (yyvsp[-3].int_type)));
      if ((yyvsp[-1].int_type) < 0)
        CTRL->Error(StringPrintf("End bound must be non-negative: %d", (yyvsp[-1].int_type)));
      RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::RANGE);
      node->AddArgument((yyvsp[-5].node_type));
      node->SetRange((yyvsp[-3].int_type), (yyvsp[-1].int_type));
      (yyval.node_type) = node; }
#line 1790 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 46:
#line 364 "main/parser.yy" /* yacc.c:1646  */
    { if ((yyvsp[-1].int_type) < 0)
        CTRL->Error(StringPrintf("Repetition count should be non-negative: %d", (yyvsp[-1].int_type)));
      RepetitionFstNode* node = new RepetitionFstNode(RepetitionFstNode::RANGE);
      node->AddArgument((yyvsp[-3].node_type));
      node->SetRange((yyvsp[-1].int_type), (yyvsp[-1].int_type));
      (yyval.node_type) = node; }
#line 1801 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 47:
#line 371 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1807 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 377 "main/parser.yy" /* yacc.c:1646  */
    { FstNode* node = new FstNode(FstNode::COMPOSITION_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument((yyvsp[-2].node_type));
      node->AddArgument((yyvsp[0].node_type));
      (yyval.node_type) = node; }
#line 1817 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 383 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1823 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 50:
#line 388 "main/parser.yy" /* yacc.c:1646  */
    { FstNode* node = new FstNode(FstNode::IDENTIFIER_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument((yyvsp[0].identifier_node_type));
      (yyval.node_type) = node; }
#line 1832 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 396 "main/parser.yy" /* yacc.c:1646  */
    { StringFstNode* node = new StringFstNode(StringFstNode::BYTE);
      node->AddArgument((yyvsp[0].string_node_type));
      node->SetLine((yyvsp[0].string_node_type)->getline());  // Get the line from the actual text line.
      (yyval.node_type) = node; }
#line 1841 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 52:
#line 401 "main/parser.yy" /* yacc.c:1646  */
    { StringFstNode* node = new StringFstNode(StringFstNode::BYTE);
      node->AddArgument((yyvsp[-2].string_node_type));
      node->SetLine((yyvsp[-2].string_node_type)->getline());
      (yyval.node_type) = node; }
#line 1850 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 406 "main/parser.yy" /* yacc.c:1646  */
    { StringFstNode* node = new StringFstNode(StringFstNode::UTF8);
      node->AddArgument((yyvsp[-2].string_node_type));
      node->SetLine((yyvsp[-2].string_node_type)->getline());
      (yyval.node_type) = node; }
#line 1859 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 411 "main/parser.yy" /* yacc.c:1646  */
    { StringFstNode* node = new StringFstNode(StringFstNode::SYMBOL_TABLE);
      node->AddArgument((yyvsp[-2].string_node_type));
      node->AddArgument((yyvsp[0].node_type));
      node->SetLine((yyvsp[-2].string_node_type)->getline());
      (yyval.node_type) = node; }
#line 1869 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 417 "main/parser.yy" /* yacc.c:1646  */
    { StringFstNode* node = new StringFstNode(StringFstNode::SYMBOL_TABLE);
      node->AddArgument((yyvsp[-2].string_node_type));
      node->AddArgument((yyvsp[0].node_type));
      node->SetLine((yyvsp[-2].string_node_type)->getline());
      (yyval.node_type) = node; }
#line 1879 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 426 "main/parser.yy" /* yacc.c:1646  */
    { FstNode* node = new FstNode(FstNode::REWRITE_FSTNODE);
      node->SetLine(CTRL->GetLexer()->line_number());
      node->AddArgument((yyvsp[-2].node_type));
      node->AddArgument((yyvsp[0].node_type));
      (yyval.node_type) = node; }
#line 1889 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 432 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1895 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 439 "main/parser.yy" /* yacc.c:1646  */
    { if (!static_cast<FstNode*>((yyvsp[-1].node_type))->SetWeight((yyvsp[0].string_node_type)))
        CTRL->Error("Rules may have only one weight.");
      (yyval.node_type) = (yyvsp[-1].node_type); }
#line 1903 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 443 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.node_type) = (yyvsp[0].node_type); }
#line 1909 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 450 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.int_type) = atoi(CTRL->GetLexer()->YYString().c_str()); }
#line 1915 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 61:
#line 455 "main/parser.yy" /* yacc.c:1646  */
    { StringNode* node = new StringNode(CTRL->GetLexer()->YYString());
      node->SetLine(CTRL->GetLexer()->line_number());
      (yyval.string_node_type) = node; }
#line 1923 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 462 "main/parser.yy" /* yacc.c:1646  */
    { StringNode* node = new StringNode(CTRL->GetLexer()->YYString());
      node->SetLine(CTRL->GetLexer()->line_number());
      (yyval.string_node_type) = node; }
#line 1931 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 469 "main/parser.yy" /* yacc.c:1646  */
    { StringNode* node = new StringNode(CTRL->GetLexer()->YYString());
      node->SetLine(CTRL->GetLexer()->line_number());
      (yyval.string_node_type) = node; }
#line 1939 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 476 "main/parser.yy" /* yacc.c:1646  */
    { ImportNode* node = new ImportNode((yyvsp[-3].string_node_type), (yyvsp[-1].identifier_node_type));
      node->SetLine(CTRL->GetLexer()->line_number());
      (yyval.import_node_type) = node; }
#line 1947 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 480 "main/parser.yy" /* yacc.c:1646  */
    { CTRL->Error("Invalid import statement.");
      (yyval.import_node_type) = NULL; }
#line 1954 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 486 "main/parser.yy" /* yacc.c:1646  */
    { FunctionNode* node = new FunctionNode((yyvsp[-2].identifier_node_type), (yyvsp[-1].collection_node_type), (yyvsp[0].collection_node_type));
      node->SetLine(node->GetName()->getline());  // Use the identifier's location.
      (yyval.function_node_type) = node; }
#line 1962 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 490 "main/parser.yy" /* yacc.c:1646  */
    { CTRL->Error("Invalid function declaration.");
      (yyval.function_node_type) = NULL; }
#line 1969 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 496 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.collection_node_type) = new CollectionNode(); }
#line 1975 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 498 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.collection_node_type) = (yyvsp[-1].collection_node_type); }
#line 1981 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 503 "main/parser.yy" /* yacc.c:1646  */
    { CollectionNode* node = new CollectionNode();
      node->AddFront((yyvsp[0].identifier_node_type));
      (yyval.collection_node_type) = node; }
#line 1989 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 71:
#line 507 "main/parser.yy" /* yacc.c:1646  */
    { (yyvsp[0].collection_node_type)->AddFront((yyvsp[-2].identifier_node_type));
      (yyval.collection_node_type) = (yyvsp[0].collection_node_type); }
#line 1996 "main/parser.cc" /* yacc.c:1646  */
    break;

  case 72:
#line 513 "main/parser.yy" /* yacc.c:1646  */
    { (yyval.collection_node_type) = (yyvsp[-1].collection_node_type); }
#line 2002 "main/parser.cc" /* yacc.c:1646  */
    break;


#line 2006 "main/parser.cc" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (parm, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (parm, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, parm);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, parm);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (parm, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, parm);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, parm);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 516 "main/parser.yy" /* yacc.c:1906  */


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

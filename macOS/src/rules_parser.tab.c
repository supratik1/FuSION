/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BEGIN_RULE = 258,
     END_RULE = 259,
     COLON = 260,
     FOR_ALL = 261,
     THERE_EXISTS = 262,
     IN = 263,
     IS = 264,
     AND = 265,
     OR = 266,
     NOT = 267,
     IMPLIES = 268,
     ALL_NODES = 269,
     LEFT_PAREN = 270,
     RIGHT_PAREN = 271,
     PRED = 272,
     SUCC = 273,
     NODE_TYPE = 274,
     EDGE_TYPE = 275,
     EDGE_SUBTYPE = 276,
     EDGE_SUBTYPE_DNF = 277,
     NODE_INPUT_EDGE_TYPE = 278,
     NODE_OUTPUT_EDGE_TYPE = 279,
     NODE_INPUT_EDGE_SUBTYPE = 280,
     NODE_OUTPUT_EDGE_SUBTYPE = 281,
     GENE = 282,
     GROUP = 283,
     COMPOUND = 284,
     PPREL = 285,
     ECREL = 286,
     GEREL = 287,
     PCREL = 288,
     ACTIVE = 289,
     INHIBIT = 290,
     EXPRESS = 291,
     REPRESS = 292,
     INDIRECT = 293,
     STATE_CHANGE = 294,
     ASSOC = 295,
     DISSOC = 296,
     MISSING_INTERACTION = 297,
     COMPONENT = 298,
     COMMA = 299,
     SEMICOLON = 300,
     EDGE = 301,
     NODE = 302,
     ACTIVATED = 303,
     INHIBITED = 304,
     OVEREXPRESSED = 305,
     REPRESSED = 306,
     DIFFER_EXPRESSED = 307,
     NOT_DIFFER_EXPRESSED = 308,
     PRESENT = 309,
     ABSENT = 310,
     VARIABLE = 311
   };
#endif
/* Tokens.  */
#define BEGIN_RULE 258
#define END_RULE 259
#define COLON 260
#define FOR_ALL 261
#define THERE_EXISTS 262
#define IN 263
#define IS 264
#define AND 265
#define OR 266
#define NOT 267
#define IMPLIES 268
#define ALL_NODES 269
#define LEFT_PAREN 270
#define RIGHT_PAREN 271
#define PRED 272
#define SUCC 273
#define NODE_TYPE 274
#define EDGE_TYPE 275
#define EDGE_SUBTYPE 276
#define EDGE_SUBTYPE_DNF 277
#define NODE_INPUT_EDGE_TYPE 278
#define NODE_OUTPUT_EDGE_TYPE 279
#define NODE_INPUT_EDGE_SUBTYPE 280
#define NODE_OUTPUT_EDGE_SUBTYPE 281
#define GENE 282
#define GROUP 283
#define COMPOUND 284
#define PPREL 285
#define ECREL 286
#define GEREL 287
#define PCREL 288
#define ACTIVE 289
#define INHIBIT 290
#define EXPRESS 291
#define REPRESS 292
#define INDIRECT 293
#define STATE_CHANGE 294
#define ASSOC 295
#define DISSOC 296
#define MISSING_INTERACTION 297
#define COMPONENT 298
#define COMMA 299
#define SEMICOLON 300
#define EDGE 301
#define NODE 302
#define ACTIVATED 303
#define INHIBITED 304
#define OVEREXPRESSED 305
#define REPRESSED 306
#define DIFFER_EXPRESSED 307
#define NOT_DIFFER_EXPRESSED 308
#define PRESENT 309
#define ABSENT 310
#define VARIABLE 311




/* Copy the first part of user declarations.  */
#line 1 "src/rules_parser.y"

#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <vector>
#include "../include/rules_parser_datatypes.h"

using namespace std;


//comment this out in the integrated version
int yylex(void);
//void yyerror(const char *);
void yyerror(char const *);
//to be commented ends

list<rule_t*> list_of_rules;


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 29 "src/rules_parser.y"
{
  char* str;
  opr_t opr;
  fun_name_t fun_name;
  fun_name2_t fun_name2;
  quantifier_t quantifier;
  node_t node_type;
  edge_t edge_type;
  edge_subt edge_subtype;
  node_state_t node_state;
  node_state2_t node_state2;
  edge_state_t edge_state;
  set_nodes_t* set_nodes;
  list_edge_subtypes_t* list_edge_subtypes;
  //logical_edge_subt_t* logical_edge_subtypes;
  edge_subt_literal_t* edge_subtype_literal;
  ANDed_edgesub_literals_t* ANDed_edge_subtype_clause;
  DNF_edgesub_t* DNF_edge_subtype;
  list_antecedent_t* list_antecedent;
  logical_stmt_t* logical_stmt;
  consequent_t* consequent;
  rule_t* rule;
  list_rules_t* list_rules;
}
/* Line 193 of yacc.c.  */
#line 255 "src/rules_parser.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 268 "src/rules_parser.tab.c"

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
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
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
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   161

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  64
/* YYNRULES -- Number of states.  */
#define YYNSTATES  163

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   311

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    14,    19,    24,    30,    36,
      39,    43,    47,    52,    57,    62,    67,    72,    77,    82,
      91,   100,   111,   113,   115,   117,   119,   121,   123,   125,
     127,   129,   131,   133,   135,   137,   139,   141,   143,   145,
     147,   149,   151,   155,   157,   160,   162,   166,   168,   172,
     176,   183,   190,   203,   214,   217,   221,   225,   227,   229,
     231,   233,   235,   237,   239
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      58,     0,    -1,    59,    -1,    58,    59,    -1,     3,    60,
       5,    70,     4,    -1,     6,    62,     8,    61,    -1,     7,
      62,     8,    61,    -1,    60,     6,    62,     8,    61,    -1,
      60,     7,    62,     8,    61,    -1,    12,    61,    -1,    61,
      10,    61,    -1,    61,    11,    61,    -1,    17,    15,    62,
      16,    -1,    18,    15,    62,    16,    -1,    19,    15,    63,
      16,    -1,    23,    15,    64,    16,    -1,    24,    15,    64,
      16,    -1,    25,    15,    65,    16,    -1,    26,    15,    65,
      16,    -1,    20,    15,    62,    44,    62,    44,    64,    16,
      -1,    21,    15,    62,    44,    62,    44,    65,    16,    -1,
      46,    15,    62,    44,    62,    44,    64,    44,    66,    16,
      -1,    14,    -1,    56,    -1,    27,    -1,    28,    -1,    29,
      -1,    30,    -1,    31,    -1,    32,    -1,    33,    -1,    34,
      -1,    35,    -1,    36,    -1,    37,    -1,    38,    -1,    39,
      -1,    40,    -1,    41,    -1,    42,    -1,    43,    -1,    65,
      -1,    66,    45,    65,    -1,    65,    -1,    12,    65,    -1,
      67,    -1,    68,    10,    67,    -1,    68,    -1,    69,    11,
      68,    -1,    71,    13,    71,    -1,    47,    15,    62,    16,
       9,    72,    -1,    47,    15,    62,    16,     9,    73,    -1,
      46,    15,    62,    44,    62,    44,    64,    44,    66,    16,
       9,    74,    -1,    22,    15,    62,    44,    62,    44,    69,
      16,     9,    74,    -1,    12,    71,    -1,    71,    11,    71,
      -1,    71,    10,    71,    -1,    48,    -1,    49,    -1,    51,
      -1,    50,    -1,    52,    -1,    53,    -1,    54,    -1,    55,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   138,   138,   149,   161,   171,   186,   201,   216,   232,
     252,   274,   296,   316,   336,   354,   372,   390,   408,   426,
     448,   470,   516,   535,   544,   546,   548,   551,   553,   555,
     557,   560,   562,   564,   566,   568,   570,   572,   574,   576,
     578,   581,   590,   600,   609,   619,   628,   638,   647,   703,
     712,   734,   756,   780,   830,   852,   876,   943,   945,   947,
     949,   952,   954,   957,   959
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BEGIN_RULE", "END_RULE", "COLON",
  "FOR_ALL", "THERE_EXISTS", "IN", "IS", "AND", "OR", "NOT", "IMPLIES",
  "ALL_NODES", "LEFT_PAREN", "RIGHT_PAREN", "PRED", "SUCC", "NODE_TYPE",
  "EDGE_TYPE", "EDGE_SUBTYPE", "EDGE_SUBTYPE_DNF", "NODE_INPUT_EDGE_TYPE",
  "NODE_OUTPUT_EDGE_TYPE", "NODE_INPUT_EDGE_SUBTYPE",
  "NODE_OUTPUT_EDGE_SUBTYPE", "GENE", "GROUP", "COMPOUND", "PPREL",
  "ECREL", "GEREL", "PCREL", "ACTIVE", "INHIBIT", "EXPRESS", "REPRESS",
  "INDIRECT", "STATE_CHANGE", "ASSOC", "DISSOC", "MISSING_INTERACTION",
  "COMPONENT", "COMMA", "SEMICOLON", "EDGE", "NODE", "ACTIVATED",
  "INHIBITED", "OVEREXPRESSED", "REPRESSED", "DIFFER_EXPRESSED",
  "NOT_DIFFER_EXPRESSED", "PRESENT", "ABSENT", "VARIABLE", "$accept",
  "list_rules", "rule", "antecedent", "set_nodes", "var", "node_t",
  "edge_t", "edge_subt", "list_edge_subtypes", "edge_subtype_literal",
  "ANDed_edge_subtype_clause", "DNF_edge_subtype", "consequent",
  "logical_statement", "node_state", "node_state2", "edge_state", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    57,    58,    58,    59,    60,    60,    60,    60,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    62,    63,    63,    63,    64,    64,    64,
      64,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    66,    66,    67,    67,    68,    68,    69,    69,    70,
      71,    71,    71,    71,    71,    71,    71,    72,    72,    72,
      72,    73,    73,    74,    74
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     5,     4,     4,     5,     5,     2,
       3,     3,     4,     4,     4,     4,     4,     4,     4,     8,
       8,    10,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     2,     1,     3,     1,     3,     3,
       6,     6,    12,    10,     2,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     2,     0,     0,     0,     1,     3,    23,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     6,    54,
       0,     0,     0,     4,     0,     0,     0,     0,     0,     9,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    56,    55,    49,     7,     8,
       0,     0,    24,    25,    26,     0,     0,     0,    27,    28,
      29,    30,     0,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,     0,     0,     0,    10,    11,     0,
       0,     0,    12,    13,    14,     0,     0,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      57,    58,    60,    59,    61,    62,    50,    51,     0,     0,
       0,     0,    43,    45,    47,     0,     0,     0,     0,     0,
      44,     0,     0,     0,     0,    19,    20,     0,    46,    48,
       0,    41,     0,     0,    63,    64,    53,     0,     0,    21,
       0,    42,    52
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     6,    37,    10,    75,    82,   132,   152,
     133,   134,   135,    21,    22,   126,   127,   156
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -57
static const yytype_int16 yypact[] =
{
       8,    37,     3,   -57,   -42,   -42,    74,   -57,   -57,   -57,
      32,    39,     9,   -42,   -42,    91,    91,     9,    38,    55,
      57,    46,    58,    66,    69,    91,   -57,    77,    78,    82,
      83,    89,    92,    98,   113,   114,   115,    72,    72,   -57,
     -42,   -42,   -42,   -57,     9,     9,     9,    91,    91,   -57,
     -42,   -42,     1,   -42,   -42,   -13,   -13,    23,    23,   -42,
      91,    91,    40,    43,   116,   -57,   121,    75,    72,    72,
     117,   118,   -57,   -57,   -57,   119,    94,    95,   -57,   -57,
     -57,   -57,   120,   124,   -57,   -57,   -57,   -57,   -57,   -57,
     -57,   -57,   -57,   -57,   125,   126,    99,   -57,   134,   -42,
     -42,    90,   -57,   -57,   -57,   -42,   -42,   -57,   -57,   -57,
     -57,   -42,   101,   102,   -26,   103,   104,   105,    84,   -13,
     -57,   -57,   -57,   -57,   -57,   -57,   -57,   -57,   -13,    23,
     -13,    23,   -57,   -57,   140,    -1,   107,   136,   137,   110,
     -57,    84,    84,   146,    23,   -57,   -57,    23,   -57,   140,
      35,   -57,    -4,    -3,   -57,   -57,   -57,   147,    23,   -57,
      35,   -57,   -57
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -57,   -57,   155,   -57,    -9,    -5,   -57,   -52,   -56,    11,
      18,    19,   -57,   -57,   -12,   -57,   -57,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      11,    94,    95,     7,    83,    39,     1,    38,    23,    24,
     142,     1,   157,   159,     9,   143,    49,    78,    79,    80,
      81,    17,   120,   121,   122,   123,   124,   125,    72,    73,
      74,    18,    65,    66,    67,    62,    63,    64,    68,    69,
      15,   158,   158,     4,     5,    70,    71,    16,    76,    77,
      43,    97,    98,    40,    96,    19,    20,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,   136,    44,    45,
      41,    46,    42,   138,    47,   140,   137,    48,   139,    12,
      13,    14,    60,    61,    99,    44,    45,   100,   151,   154,
     155,   151,    50,    51,   112,   113,   131,    52,    53,   114,
     115,   116,   161,    25,    54,    26,   117,    55,    27,    28,
      29,    30,    31,    56,    32,    33,    34,    35,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    57,    58,
      59,    44,   101,   102,   103,   104,   107,    36,   105,   106,
     108,   109,   110,   111,    60,   118,   119,   128,   129,   130,
     141,   144,   145,   146,   147,   150,   160,     8,   153,   148,
     162,   149
};

static const yytype_uint8 yycheck[] =
{
       5,    57,    58,     0,    56,    17,     3,    16,    13,    14,
      11,     3,    16,    16,    56,    16,    25,    30,    31,    32,
      33,    12,    48,    49,    50,    51,    52,    53,    27,    28,
      29,    22,    44,    45,    46,    40,    41,    42,    47,    48,
       8,    45,    45,     6,     7,    50,    51,     8,    53,    54,
       4,    60,    61,    15,    59,    46,    47,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,   119,    10,    11,
      15,    13,    15,   129,     8,   131,   128,     8,   130,     5,
       6,     7,    10,    11,    44,    10,    11,    44,   144,    54,
      55,   147,    15,    15,    99,   100,    12,    15,    15,     9,
     105,   106,   158,    12,    15,    14,   111,    15,    17,    18,
      19,    20,    21,    15,    23,    24,    25,    26,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    15,    15,
      15,    10,    16,    16,    16,    16,    16,    46,    44,    44,
      16,    16,    16,    44,    10,    44,    44,    44,    44,    44,
      10,    44,    16,    16,    44,     9,     9,     2,   147,   141,
     160,   142
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    58,    59,     6,     7,    60,     0,    59,    56,
      62,    62,     5,     6,     7,     8,     8,    12,    22,    46,
      47,    70,    71,    62,    62,    12,    14,    17,    18,    19,
      20,    21,    23,    24,    25,    26,    46,    61,    61,    71,
      15,    15,    15,     4,    10,    11,    13,     8,     8,    61,
      15,    15,    15,    15,    15,    15,    15,    15,    15,    15,
      10,    11,    62,    62,    62,    71,    71,    71,    61,    61,
      62,    62,    27,    28,    29,    63,    62,    62,    30,    31,
      32,    33,    64,    64,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    65,    65,    62,    61,    61,    44,
      44,    16,    16,    16,    16,    44,    44,    16,    16,    16,
      16,    44,    62,    62,     9,    62,    62,    62,    44,    44,
      48,    49,    50,    51,    52,    53,    72,    73,    44,    44,
      44,    12,    65,    67,    68,    69,    64,    64,    65,    64,
      65,    10,    11,    16,    44,    16,    16,    44,    67,    68,
       9,    65,    66,    66,    54,    55,    74,    16,    45,    16,
       9,    65,    74
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
     `$$ = $1'.

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
#line 139 "src/rules_parser.y"
    {
					list_rules_t* temp = new list_rules_t;
					assert(temp != NULL);
					temp->rule = (yyvsp[(1) - (1)].rule);
					temp->next = NULL;
					if(list_of_rules.empty())
						list_of_rules.push_back(temp->rule);
					(yyval.list_rules) = temp;
				;}
    break;

  case 3:
#line 150 "src/rules_parser.y"
    {
					list_rules_t* temp = new list_rules_t;
					assert(temp != NULL);
					temp->rule = (yyvsp[(2) - (2)].rule);
					temp->next = (yyvsp[(1) - (2)].list_rules);
					if(!list_of_rules.empty())
						list_of_rules.push_back(temp->rule);
					(yyval.list_rules) = temp;
				;}
    break;

  case 4:
#line 162 "src/rules_parser.y"
    {
				rule_t* temp = new rule_t;
				assert(temp != NULL);
				temp->antecedent = (yyvsp[(2) - (5)].list_antecedent); 
				temp->consequent = (yyvsp[(4) - (5)].consequent);
				(yyval.rule) = temp;
			;}
    break;

  case 5:
#line 172 "src/rules_parser.y"
    {
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = FOR_ALL_QUANT;
					temp->var = new char[strlen((yyvsp[(2) - (4)].str))+1];
					assert(temp->var != NULL);
					strcpy(temp->var, (yyvsp[(2) - (4)].str));
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = (yyvsp[(4) - (4)].set_nodes); 
					temp->next = NULL;
					(yyval.list_antecedent) = temp;
				;}
    break;

  case 6:
#line 187 "src/rules_parser.y"
    {
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = THERE_EXISTS_QUANT;
					temp->var = new char[strlen((yyvsp[(2) - (4)].str))+1];
					assert(temp->var != NULL);
					strcpy(temp->var, (yyvsp[(2) - (4)].str));
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = (yyvsp[(4) - (4)].set_nodes); 
					temp->next = NULL;
					(yyval.list_antecedent) = temp;
				;}
    break;

  case 7:
#line 202 "src/rules_parser.y"
    {
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = FOR_ALL_QUANT;
					temp->var = new char[strlen((yyvsp[(3) - (5)].str))+1];
					assert(temp->var != NULL);
					strcpy(temp->var, (yyvsp[(3) - (5)].str));
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = (yyvsp[(5) - (5)].set_nodes); 
					temp->next = (yyvsp[(1) - (5)].list_antecedent);
					(yyval.list_antecedent) = temp;
				;}
    break;

  case 8:
#line 217 "src/rules_parser.y"
    {
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = THERE_EXISTS_QUANT;
					temp->var = new char[strlen((yyvsp[(3) - (5)].str))+1];
					assert(temp->var != NULL);
					strcpy(temp->var, (yyvsp[(3) - (5)].str));
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = (yyvsp[(5) - (5)].set_nodes); 
					temp->next = (yyvsp[(1) - (5)].list_antecedent);
					(yyval.list_antecedent) = temp;
				;}
    break;

  case 9:
#line 233 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NOT_OPR;
					temp->left = new set_nodes_t;
					assert(temp->left != NULL);
					temp->left = (yyvsp[(2) - (2)].set_nodes);
					temp->right = NULL;
					temp->fun_name = NONE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 10:
#line 253 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = AND_OPR;
					temp->left = new set_nodes_t;
					assert(temp->left != NULL);
					temp->left = (yyvsp[(1) - (3)].set_nodes);
					temp->right = new set_nodes_t;
					assert(temp->right != NULL);
					temp->right = (yyvsp[(3) - (3)].set_nodes);
					temp->fun_name = NONE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 11:
#line 275 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = OR_OPR;
					temp->left = new set_nodes_t;
					assert(temp->left != NULL);
					temp->left = (yyvsp[(1) - (3)].set_nodes);
					temp->right = new set_nodes_t;
					assert(temp->right != NULL);
					temp->right = (yyvsp[(3) - (3)].set_nodes);
					temp->fun_name = NONE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 12:
#line 297 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = PRED_FUN;
					temp->arg1 = new char[strlen((yyvsp[(3) - (4)].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[(3) - (4)].str));
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 13:
#line 317 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = SUCC_FUN;
					temp->arg1 = new char[strlen((yyvsp[(3) - (4)].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[(3) - (4)].str));
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 14:
#line 337 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_TYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = (yyvsp[(3) - (4)].node_type);
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 15:
#line 355 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_INPUT_EDGE_TYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = (yyvsp[(3) - (4)].edge_type);
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 16:
#line 373 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_OUTPUT_EDGE_TYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = (yyvsp[(3) - (4)].edge_type);
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 17:
#line 391 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_INPUT_EDGE_SUBTYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = (yyvsp[(3) - (4)].edge_subtype);
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 18:
#line 409 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_OUTPUT_EDGE_SUBTYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = (yyvsp[(3) - (4)].edge_subtype);
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 19:
#line 427 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_TYPE_FUN;
					temp->arg1 = new char[strlen((yyvsp[(3) - (8)].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[(3) - (8)].str));
					temp->arg2 = new char[strlen((yyvsp[(5) - (8)].str))+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, (yyvsp[(5) - (8)].str));
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = (yyvsp[(7) - (8)].edge_type);
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 20:
#line 449 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_SUBTYPE_FUN;
					temp->arg1 = new char[strlen((yyvsp[(3) - (8)].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[(3) - (8)].str));
					temp->arg2 = new char[strlen((yyvsp[(5) - (8)].str))+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, (yyvsp[(5) - (8)].str));
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = (yyvsp[(7) - (8)].edge_subtype);
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 21:
#line 471 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_FUN;
					temp->arg1 = new char[strlen((yyvsp[(3) - (10)].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[(3) - (10)].str));
					temp->arg2 = new char[strlen((yyvsp[(5) - (10)].str))+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, (yyvsp[(5) - (10)].str));
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = (yyvsp[(7) - (10)].edge_type);
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = (yyvsp[(9) - (10)].list_edge_subtypes);
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 22:
#line 517 "src/rules_parser.y"
    {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = ALL_NODES_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				;}
    break;

  case 23:
#line 536 "src/rules_parser.y"
    {
				char* temp = new char[strlen((yyvsp[(1) - (1)].str))+1];
				assert(temp != NULL);
				strcpy(temp, (yyvsp[(1) - (1)].str));
				(yyval.str) = temp;
			;}
    break;

  case 24:
#line 544 "src/rules_parser.y"
    {(yyval.node_type) = GENE_NODE;;}
    break;

  case 25:
#line 546 "src/rules_parser.y"
    {(yyval.node_type) = GROUP_NODE;;}
    break;

  case 26:
#line 548 "src/rules_parser.y"
    {(yyval.node_type) = COMPOUND_NODE;;}
    break;

  case 27:
#line 551 "src/rules_parser.y"
    {(yyval.edge_type) = PPREL_EDGE;;}
    break;

  case 28:
#line 553 "src/rules_parser.y"
    {(yyval.edge_type) = ECREL_EDGE;;}
    break;

  case 29:
#line 555 "src/rules_parser.y"
    {(yyval.edge_type) = GEREL_EDGE;;}
    break;

  case 30:
#line 557 "src/rules_parser.y"
    {(yyval.edge_type) = PCREL_EDGE;;}
    break;

  case 31:
#line 560 "src/rules_parser.y"
    {(yyval.edge_subtype) = ACTIVE_EDGE;;}
    break;

  case 32:
#line 562 "src/rules_parser.y"
    {(yyval.edge_subtype) = INHIBIT_EDGE;;}
    break;

  case 33:
#line 564 "src/rules_parser.y"
    {(yyval.edge_subtype) = EXPRESS_EDGE;;}
    break;

  case 34:
#line 566 "src/rules_parser.y"
    {(yyval.edge_subtype) = REPRESS_EDGE;;}
    break;

  case 35:
#line 568 "src/rules_parser.y"
    {(yyval.edge_subtype) = INDIRECT_EDGE;;}
    break;

  case 36:
#line 570 "src/rules_parser.y"
    {(yyval.edge_subtype) = STATE_CHANGE_EDGE;;}
    break;

  case 37:
#line 572 "src/rules_parser.y"
    {(yyval.edge_subtype) = ASSOC_EDGE;;}
    break;

  case 38:
#line 574 "src/rules_parser.y"
    {(yyval.edge_subtype) = DISSOC_EDGE;;}
    break;

  case 39:
#line 576 "src/rules_parser.y"
    {(yyval.edge_subtype) = MISSING_INTERACTION_EDGE;;}
    break;

  case 40:
#line 578 "src/rules_parser.y"
    {(yyval.edge_subtype) = COMPONENT_EDGE;;}
    break;

  case 41:
#line 582 "src/rules_parser.y"
    {
					list_edge_subtypes_t* temp = new list_edge_subtypes_t;
					assert(temp != NULL);
					temp->edge_subtype_data = (yyvsp[(1) - (1)].edge_subtype);
					temp->next = NULL;
					(yyval.list_edge_subtypes) = temp;
				;}
    break;

  case 42:
#line 591 "src/rules_parser.y"
    {
					list_edge_subtypes_t* temp = new list_edge_subtypes_t;
					assert(temp != NULL);
					temp->edge_subtype_data = (yyvsp[(3) - (3)].edge_subtype);
					temp->next = (yyvsp[(1) - (3)].list_edge_subtypes);
					(yyval.list_edge_subtypes) = temp;
				;}
    break;

  case 43:
#line 601 "src/rules_parser.y"
    {
								edge_subt_literal_t* temp = new edge_subt_literal_t;
								assert(temp != NULL);
								temp->opr = NONE_OPR;
								temp->edge_subtype_data = (yyvsp[(1) - (1)].edge_subtype);
								(yyval.edge_subtype_literal) = temp;
							;}
    break;

  case 44:
#line 610 "src/rules_parser.y"
    {
								edge_subt_literal_t* temp = new edge_subt_literal_t;
								assert(temp != NULL);
								temp->opr = NOT_OPR;
								temp->edge_subtype_data = (yyvsp[(2) - (2)].edge_subtype);
								(yyval.edge_subtype_literal) = temp;
							;}
    break;

  case 45:
#line 620 "src/rules_parser.y"
    {
									ANDed_edgesub_literals_t* temp = new ANDed_edgesub_literals_t;
									assert(temp != NULL);
									temp->curr_literal= (yyvsp[(1) - (1)].edge_subtype_literal);
									temp->next = NULL;									
									(yyval.ANDed_edge_subtype_clause) = temp;
								;}
    break;

  case 46:
#line 629 "src/rules_parser.y"
    {
									ANDed_edgesub_literals_t* temp = new ANDed_edgesub_literals_t;
									assert(temp != NULL);
									temp->curr_literal= (yyvsp[(3) - (3)].edge_subtype_literal);
									temp->next = (yyvsp[(1) - (3)].ANDed_edge_subtype_clause);									
									(yyval.ANDed_edge_subtype_clause) = temp;
								;}
    break;

  case 47:
#line 639 "src/rules_parser.y"
    {
							DNF_edgesub_t* temp = new DNF_edgesub_t;
							assert(temp != NULL);
							temp->current_ANDed_clause = (yyvsp[(1) - (1)].ANDed_edge_subtype_clause);
							temp->next = NULL;
							(yyval.DNF_edge_subtype)= temp;
						;}
    break;

  case 48:
#line 648 "src/rules_parser.y"
    {
							DNF_edgesub_t* temp = new DNF_edgesub_t;
							assert(temp != NULL);
							temp->current_ANDed_clause = (yyvsp[(3) - (3)].ANDed_edge_subtype_clause);
							temp->next = (yyvsp[(1) - (3)].DNF_edge_subtype);
							(yyval.DNF_edge_subtype)= temp;
						;}
    break;

  case 49:
#line 704 "src/rules_parser.y"
    {
					consequent_t* temp = new consequent_t;
					temp->left_of_implies = (yyvsp[(1) - (3)].logical_stmt);
					temp->right_of_implies = (yyvsp[(3) - (3)].logical_stmt);
					(yyval.consequent) = temp;
				;}
    break;

  case 50:
#line 713 "src/rules_parser.y"
    {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = NODE_FUN2;
							temp->arg1 = new char[strlen((yyvsp[(3) - (6)].str))+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, (yyvsp[(3) - (6)].str));
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = (yyvsp[(6) - (6)].node_state);
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							(yyval.logical_stmt) = temp;
						;}
    break;

  case 51:
#line 735 "src/rules_parser.y"
    {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = NODE_FUN2;
							temp->arg1 = new char[strlen((yyvsp[(3) - (6)].str))+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, (yyvsp[(3) - (6)].str));
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = (yyvsp[(6) - (6)].node_state2);
							temp->state_edge = EDGE_NONE_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							(yyval.logical_stmt) = temp;
						;}
    break;

  case 52:
#line 757 "src/rules_parser.y"
    {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = EDGE_FUN2;
							temp->arg1 = new char[strlen((yyvsp[(3) - (12)].str))+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, (yyvsp[(3) - (12)].str));
							temp->arg2 = new char[strlen((yyvsp[(5) - (12)].str))+1];
							assert(temp->arg2 != NULL);
							strcpy(temp->arg2, (yyvsp[(5) - (12)].str));
							temp->arg3 = (yyvsp[(7) - (12)].edge_type);
							temp->arg4_list = (yyvsp[(9) - (12)].list_edge_subtypes);
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = (yyvsp[(12) - (12)].edge_state);
							(yyval.logical_stmt) = temp;
						;}
    break;

  case 53:
#line 781 "src/rules_parser.y"
    {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = EDGE_SUBTYPE_DNF_FUN2;
							temp->arg1 = new char[strlen((yyvsp[(3) - (10)].str))+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, (yyvsp[(3) - (10)].str));
							temp->arg2 = new char[strlen((yyvsp[(5) - (10)].str))+1];
							assert(temp->arg2 != NULL);
							strcpy(temp->arg2, (yyvsp[(5) - (10)].str));
							temp->arg3 = NONE_EDGE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = (yyvsp[(7) - (10)].DNF_edge_subtype);
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = (yyvsp[(10) - (10)].edge_state);
							(yyval.logical_stmt) = temp;
						;}
    break;

  case 54:
#line 831 "src/rules_parser.y"
    {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NOT_OPR;
							temp->left = new logical_stmt_t;
							assert(temp->left != NULL);
							temp->left = (yyvsp[(2) - (2)].logical_stmt);
							temp->right = NULL;
							temp->fun_name2 = NONE_FUN2;
							temp->arg1 = NULL;
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							(yyval.logical_stmt) = temp;
						;}
    break;

  case 55:
#line 853 "src/rules_parser.y"
    {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = OR_OPR;
							temp->left = new logical_stmt_t;
							assert(temp->left != NULL);
							temp->left = (yyvsp[(1) - (3)].logical_stmt);
							temp->right = new logical_stmt_t;
							assert(temp->right != NULL);
							temp->right = (yyvsp[(3) - (3)].logical_stmt);
							temp->fun_name2 = NONE_FUN2;
							temp->arg1 = NULL;
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							(yyval.logical_stmt) = temp;
						;}
    break;

  case 56:
#line 877 "src/rules_parser.y"
    {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = AND_OPR;
							temp->left = new logical_stmt_t;
							assert(temp->left != NULL);
							temp->left = (yyvsp[(1) - (3)].logical_stmt);
							temp->right = new logical_stmt_t;
							assert(temp->right != NULL);
							temp->right = (yyvsp[(3) - (3)].logical_stmt);
							temp->fun_name2 = NONE_FUN2;
							temp->arg1 = NULL;
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							(yyval.logical_stmt) = temp;
						;}
    break;

  case 57:
#line 943 "src/rules_parser.y"
    {(yyval.node_state) = ACTIVATED_STATE;;}
    break;

  case 58:
#line 945 "src/rules_parser.y"
    {(yyval.node_state) = INHIBITED_STATE;;}
    break;

  case 59:
#line 947 "src/rules_parser.y"
    {(yyval.node_state) = REPRESSED_STATE;;}
    break;

  case 60:
#line 949 "src/rules_parser.y"
    {(yyval.node_state) = OVEREXPRESSED_STATE;;}
    break;

  case 61:
#line 952 "src/rules_parser.y"
    {(yyval.node_state2) = DIFFER_EXPRESSED_STATE;;}
    break;

  case 62:
#line 954 "src/rules_parser.y"
    {(yyval.node_state2) = NOT_DIFFER_EXPRESSED_STATE;;}
    break;

  case 63:
#line 957 "src/rules_parser.y"
    {(yyval.edge_state) = PRESENT_STATE;;}
    break;

  case 64:
#line 959 "src/rules_parser.y"
    {(yyval.edge_state) = ABSENT_STATE;;}
    break;


/* Line 1267 of yacc.c.  */
#line 2421 "src/rules_parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
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

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 962 "src/rules_parser.y"


void yyerror(char const* message)
{
    cerr << message;
}

int init (void) {
    //yyparse();
    return (1);
}


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

/* Substitute the variable and function names.  */
#define yyparse rules_parserparse
#define yylex   rules_parserlex
#define yyerror rules_parsererror
#define yylval  rules_parserlval
#define yychar  rules_parserchar
#define yydebug rules_parserdebug
#define yynerrs rules_parsernerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     t_String = 258,
     t_Arrow = 259,
     t_True = 260,
     t_False = 261,
     t_Constant = 262,
     t_Integer = 263,
     t_Variable = 264,
     t_OBrack = 265,
     t_CBrack = 266,
     t_Comma = 267,
     t_Eq = 268,
     t_BoolVariable = 269
   };
#endif
/* Tokens.  */
#define t_String 258
#define t_Arrow 259
#define t_True 260
#define t_False 261
#define t_Constant 262
#define t_Integer 263
#define t_Variable 264
#define t_OBrack 265
#define t_CBrack 266
#define t_Comma 267
#define t_Eq 268
#define t_BoolVariable 269




/* Copy the first part of user declarations.  */
#line 1 "rules_parser.y"


#include <string>
#include "DAGSimplifier.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DagManager.h"
#include "helping_functions.h"
  using namespace std;
  int yylex(void);
  void yyerror(char const *);
  extern ofstream *outfile;
  int rules_parserlex(void);
  void rules_parsererror(char const *);
  vector<t_RewriteRuleExpression *> assign_arguments_of_simplification_expressions
    (
     class t_VectorOfRewriteRuleExpressions *v_exprs_in
     );

 extern t_DAGSimplifier *ds;


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
#line 31 "rules_parser.y"
{
  char  *string_value;
  int   int_value;
  class t_RewriteRuleExpression *bool_expression_type;
  class t_RewriteRuleExpression *expression_type;
  class t_RewriteRule *rule_type;
  
  class t_VectorOfRewriteRuleExpressions *vector_of_exprs_type;
}
/* Line 193 of yacc.c.  */
#line 166 "rules_parser.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 179 "rules_parser.tab.c"

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   42

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  15
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  15
/* YYNRULES -- Number of rules.  */
#define YYNRULES  32
/* YYNRULES -- Number of states.  */
#define YYNSTATES  42

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   269

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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    14,    19,    21,    23,
      25,    27,    29,    31,    33,    35,    37,    39,    44,    46,
      48,    50,    52,    54,    56,    58,    60,    62,    64,    67,
      69,    72,    74
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      16,     0,    -1,    17,    -1,    17,    18,    -1,    -1,     8,
      19,     4,    23,    -1,    10,    29,    27,    11,    -1,    20,
      -1,    22,    -1,    21,    -1,     3,    -1,     9,    -1,    14,
      -1,     5,    -1,     6,    -1,     7,    -1,     8,    -1,    10,
      29,    28,    11,    -1,    24,    -1,    26,    -1,    25,    -1,
       3,    -1,     9,    -1,    14,    -1,     5,    -1,     6,    -1,
       7,    -1,     8,    -1,    27,    19,    -1,    19,    -1,    28,
      23,    -1,    23,    -1,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    53,    53,    63,    69,    74,    89,   107,   113,   119,
     126,   137,   151,   165,   177,   188,   200,   217,   226,   232,
     238,   245,   255,   269,   283,   291,   299,   310,   323,   334,
     346,   357,   373
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "t_String", "t_Arrow", "t_True",
  "t_False", "t_Constant", "t_Integer", "t_Variable", "t_OBrack",
  "t_CBrack", "t_Comma", "t_Eq", "t_BoolVariable", "$accept", "RulesFile",
  "RulesList", "Rule", "nt_LHSExpr", "LHSVariable", "LHSBoolVariable",
  "LHSConstant", "nt_RHSExpr", "RHSVariable", "RHSBoolVariable",
  "RHSConstant", "nt_LHS_Arguments_List", "nt_RHS_Arguments_List",
  "Operator", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    15,    16,    17,    17,    18,    19,    19,    19,    19,
      19,    20,    21,    22,    22,    22,    22,    23,    23,    23,
      23,    23,    24,    25,    26,    26,    26,    26,    27,    27,
      28,    28,    29
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     4,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     2,     1,     0,     3,    10,    13,    14,    15,
      16,    11,     0,    12,     0,     7,     9,     8,    32,     0,
       0,    29,     0,    21,    24,    25,    26,    27,    22,     0,
      23,     5,    18,    20,    19,     6,    28,     0,    31,     0,
      17,    30
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,     5,    14,    15,    16,    17,    31,    32,
      33,    34,    22,    39,    19
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -8
static const yytype_int8 yypact[] =
{
      -8,     9,    -7,    -8,    17,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    16,    -8,    24,    -8,    -8,    -8,    -8,    17,
      27,    -8,    -3,    -8,    -8,    -8,    -8,    -8,    -8,    16,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    27,    -8,     7,
      -8,    -8
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
      -8,    -8,    -8,    -8,    20,    -8,    -8,    -8,     1,    -8,
      -8,    -8,    -8,    -8,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
       6,     4,     7,     8,     9,    10,    11,    12,    35,     3,
      23,    13,    24,    25,    26,    27,    28,    29,    40,    18,
       6,    30,     7,     8,     9,    10,    11,    12,    20,    37,
      23,    13,    24,    25,    26,    27,    28,    29,    38,    21,
      41,    30,    36
};

static const yytype_uint8 yycheck[] =
{
       3,     8,     5,     6,     7,     8,     9,    10,    11,     0,
       3,    14,     5,     6,     7,     8,     9,    10,    11,     3,
       3,    14,     5,     6,     7,     8,     9,    10,     4,    29,
       3,    14,     5,     6,     7,     8,     9,    10,    37,    19,
      39,    14,    22
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    16,    17,     0,     8,    18,     3,     5,     6,     7,
       8,     9,    10,    14,    19,    20,    21,    22,     3,    29,
       4,    19,    27,     3,     5,     6,     7,     8,     9,    10,
      14,    23,    24,    25,    26,    11,    19,    29,    23,    28,
      11,    23
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
#line 54 "rules_parser.y"
    {
	   //cout<<"Read the RulesFile successfully\n";
	   const string sRewriteRulesFile = 
	       t_ConfigurationOptions::getConfig()->m_sTempInfoDir + "/DAGManager/RewriteRulesRead";
	   ofstream *outfile = new ofstream();
	   outfile->open(sRewriteRulesFile.c_str());
	   ds->printAllRewriteRules(outfile);
	   outfile->close();
	 ;}
    break;

  case 3:
#line 64 "rules_parser.y"
    {
	    //cout<<"Matched RulesList \n";
	  ;}
    break;

  case 4:
#line 69 "rules_parser.y"
    {
	    //cout<<"Matched RulesList with 0 rules\n";
	  ;}
    break;

  case 5:
#line 75 "rules_parser.y"
    {
	   //cout<<"----------------------------------------\n";
	   //cout<<"Read an rule successfully\n";
	   t_RewriteRule * rule = new t_RewriteRule((yyvsp[(2) - (4)].expression_type), (yyvsp[(4) - (4)].expression_type), stringToInteger(toString((yyvsp[(1) - (4)].string_value))));
	   ds->addARewriteRule(rule);

	   t_RewriteRuleExpression *lhs_expr = (yyvsp[(2) - (4)].expression_type);
	   ds->addValidStringOfLHSOfARule(lhs_expr->getValidSubstringOfRewriteRuleExpression());
	   (yyval.rule_type) = rule;
         ;}
    break;

  case 6:
#line 90 "rules_parser.y"
    {
	  //cout<<"Creating an nt_Expr with operator and arguments\n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions((yyvsp[(3) - (4)].vector_of_exprs_type));
	  string valid_substring = (yyvsp[(2) - (4)].string_value);
	  for(int i=0; i<arguments.size();i++)
	    {
	      valid_substring += ",";
	      valid_substring+=arguments[i]->getValidSubstringOfRewriteRuleExpression();
	    }
	  //cout<<"Adding the VSS "<<valid_substring<<endl;
	  ds->addValidSubstring(valid_substring);
	  
	  t_RewriteRuleExpression *expression = new t_RewriteRuleExpression((yyvsp[(2) - (4)].string_value), other, arguments, 0,valid_substring);
	  //cout<<"Created an nt_Expr\n";
	  (yyval.expression_type) = expression;
	;}
    break;

  case 7:
#line 108 "rules_parser.y"
    {
	  //cout<<"Creating an nt_Expr with Variable\n";
	  (yyval.expression_type) = (yyvsp[(1) - (1)].expression_type);
	;}
    break;

  case 8:
#line 114 "rules_parser.y"
    {
	  //cout<<"Creating an nt_Expr with Constant\n";
	  (yyval.expression_type) = (yyvsp[(1) - (1)].expression_type);
	;}
    break;

  case 9:
#line 120 "rules_parser.y"
    {
	  //cout<<"Creating an nt_Expr with Bool Variable\n";
	  //cout<<"Created and VariableRewriteRuleExpression\n";
	  (yyval.expression_type) = (yyvsp[(1) - (1)].expression_type);
	;}
    break;

  case 10:
#line 127 "rules_parser.y"
    {
	string variable_name = (yyvsp[(1) - (1)].string_value);
	   int variable_index = 0;
	   ds->addValidSubstring((yyvsp[(1) - (1)].string_value));
	   //cout<<"Adding the VSS "<<"$1"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), placeholder,variable_index, (yyvsp[(1) - (1)].string_value));
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;

	;}
    break;

  case 11:
#line 138 "rules_parser.y"
    {
	   //these are basically e_1 e_2 ...
	   string variable_name = (yyvsp[(1) - (1)].string_value);
	   //string var_to_create = "e";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   ds->addValidSubstring("e");
	   //cout<<"Adding the VSS e"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), placeholder, variable_index, "e");
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;
	   
          ;}
    break;

  case 12:
#line 152 "rules_parser.y"
    {
	   //These are basically b_0 b_1 b_2 ...
	   string variable_name = (yyvsp[(1) - (1)].string_value);
	   //string var_to_create = "b";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   ds->addValidSubstring("b");
	   //cout<<"Adding the VSS "<<"b"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), placeholder,variable_index, "b");
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;
         ;}
    break;

  case 13:
#line 166 "rules_parser.y"
    {
	  //this is single constant true
	  t_RewriteRuleExpression *true_expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), fixed_string,0, "true");
	  string valid_substring = "true";
	  //cout<<"Adding the VSS "<<valid_substring<<endl;
	  ds->addValidSubstring(valid_substring);

	  //cout<<"Creating a constant true\n";
	  (yyval.expression_type) = true_expr;
	;}
    break;

  case 14:
#line 178 "rules_parser.y"
    {
	  //this is single constant false
	 t_RewriteRuleExpression *false_expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), fixed_string,0,"false");
	 string valid_substring = "false";
	 //cout<<"Adding the VSS "<<valid_substring<<endl;
	 ds->addValidSubstring(valid_substring);
	 //cout<<"Creating a constant false\n";
	 (yyval.expression_type) = false_expr;
       ;}
    break;

  case 15:
#line 189 "rules_parser.y"
    {
	  //these are basically constants like c_1 c_2 c_3....
	  string variable_name = (yyvsp[(1) - (1)].string_value);
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   ds->addValidSubstring("c");
	   //cout<<"Adding the VSS c\n";
	   t_RewriteRuleExpression *const_expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), placeholder,variable_index,"c");
	   //cout<<"Creating a constant false\n";
	   (yyval.expression_type) = const_expr;
	;}
    break;

  case 16:
#line 201 "rules_parser.y"
    {
	 ds->addValidSubstring((yyvsp[(1) - (1)].string_value));
	 //cout<<"Adding the VSS  "<<$1<<endl;
	 t_RewriteRuleExpression *integer_expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), fixed_string,0,(yyvsp[(1) - (1)].string_value));
	 string valid_substring = (yyvsp[(1) - (1)].string_value);
	 //cout<<"Adding the VSS "<<valid_substring<<endl;
	 ds->addValidSubstring(valid_substring);
	 //cout<<"Creating an integer \n";
	 (yyval.expression_type) = integer_expr;
       ;}
    break;

  case 17:
#line 218 "rules_parser.y"
    {
	  //cout<<"Creating an nt_Expr with operator and arguments\n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions((yyvsp[(3) - (4)].vector_of_exprs_type));
	  t_RewriteRuleExpression *expression = new t_RewriteRuleExpression((yyvsp[(2) - (4)].string_value), other, arguments, 0,"");
	  //cout<<"Created an nt_Expr\n";
	  (yyval.expression_type) = expression;
	;}
    break;

  case 18:
#line 227 "rules_parser.y"
    {
	  //cout<<"Creating an nt_Expr with Variable\n";
	  (yyval.expression_type) = (yyvsp[(1) - (1)].expression_type);
	;}
    break;

  case 19:
#line 233 "rules_parser.y"
    {
	  //cout<<"Creating an nt_Expr with Constant\n";
	  (yyval.expression_type) = (yyvsp[(1) - (1)].expression_type);
	;}
    break;

  case 20:
#line 239 "rules_parser.y"
    {
	  //cout<<"Creating an nt_Expr with Bool Variable\n";
	  //cout<<"Created and VariableRewriteRuleExpression\n";
	  (yyval.expression_type) = (yyvsp[(1) - (1)].expression_type);
	;}
    break;

  case 21:
#line 246 "rules_parser.y"
    {
	string variable_name = (yyvsp[(1) - (1)].string_value);
	   int variable_index = 0;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), placeholder,variable_index, (yyvsp[(1) - (1)].string_value));
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;

	;}
    break;

  case 22:
#line 256 "rules_parser.y"
    {
	   //these are basically e_1 e_2 ...
	   string variable_name = (yyvsp[(1) - (1)].string_value);
	   //string var_to_create = "e";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   //ds->addValidSubstring("e");
	   ////cout<<"Adding the VSS e"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), placeholder, variable_index, "e");
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;
	   
          ;}
    break;

  case 23:
#line 270 "rules_parser.y"
    {
	   //These are basically b_0 b_1 b_2 ...
	   string variable_name = (yyvsp[(1) - (1)].string_value);
	   //string var_to_create = "b";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   //ds->addValidSubstring("b");
	   ////cout<<"Adding the VSS "<<"b"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), placeholder,variable_index, "b");
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;
         ;}
    break;

  case 24:
#line 284 "rules_parser.y"
    {
	  //this is single constant true
	  t_RewriteRuleExpression *true_expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), fixed_string,0, "true");
	  //cout<<"Creating a constant true\n";
	  (yyval.expression_type) = true_expr;
	;}
    break;

  case 25:
#line 292 "rules_parser.y"
    {
	  //this is single constant false
	 t_RewriteRuleExpression *false_expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), placeholder,0,"false");
	 //cout<<"Creating a constant false\n";
	 (yyval.expression_type) = false_expr;
       ;}
    break;

  case 26:
#line 300 "rules_parser.y"
    {
	  //these are basically constants like c_1 c_2 c_3....
	  string variable_name = (yyvsp[(1) - (1)].string_value);
	   //string var_to_create = "c";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   t_RewriteRuleExpression *const_expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), placeholder,variable_index,"c");
	   //cout<<"Creating a constant false\n";
	   (yyval.expression_type) = const_expr;
	;}
    break;

  case 27:
#line 311 "rules_parser.y"
    {
	 t_RewriteRuleExpression * integer_expr;
	 integer_expr = new t_RewriteRuleExpression((yyvsp[(1) - (1)].string_value), fixed_string,0,(yyvsp[(1) - (1)].string_value));
	 //cout<<"Creating an integer \n";
	 (yyval.expression_type) = integer_expr;
       /*Operand_RewriteRuleExpressions_List is the list of (comma seperated) optional operands of an operator. Do not confuse with Expr_Vector*/
       ;}
    break;

  case 28:
#line 324 "rules_parser.y"
    {
	  //cout<<"Creating vector of arguments \n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions((yyvsp[(1) - (2)].vector_of_exprs_type));
	  t_RewriteRuleExpression *arg_to_add = (yyvsp[(2) - (2)].expression_type);
	  arguments.push_back(arg_to_add);
	  t_VectorOfRewriteRuleExpressions *vector_of_expressions = new t_VectorOfRewriteRuleExpressions(arguments);
	  //cout<<"Created vector of arguments\n";
	  (yyval.vector_of_exprs_type) = vector_of_expressions;
	;}
    break;

  case 29:
#line 335 "rules_parser.y"
    {
	    //cout<<"Creating vector of arguments with only one argument\n";
	    vector<t_RewriteRuleExpression *> arguments;
	    arguments.push_back((yyvsp[(1) - (1)].expression_type));
	    t_VectorOfRewriteRuleExpressions *vector_of_expressions
	      = new t_VectorOfRewriteRuleExpressions(arguments);
	    
	    //cout<<"Created vector of arguments with only one argument\n";
	    (yyval.vector_of_exprs_type) = vector_of_expressions;
	  ;}
    break;

  case 30:
#line 347 "rules_parser.y"
    {
	  //cout<<"Creating vector of arguments \n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions((yyvsp[(1) - (2)].vector_of_exprs_type));
	  t_RewriteRuleExpression *arg_to_add = (yyvsp[(2) - (2)].expression_type);
	  arguments.push_back(arg_to_add);
	  t_VectorOfRewriteRuleExpressions *vector_of_expressions = new t_VectorOfRewriteRuleExpressions(arguments);
	  //cout<<"Created vector of arguments\n";
	  (yyval.vector_of_exprs_type) = vector_of_expressions;
	;}
    break;

  case 31:
#line 358 "rules_parser.y"
    {
	    //cout<<"Creating vector of arguments with only one argument\n";
	    vector<t_RewriteRuleExpression *> arguments;
	    arguments.push_back((yyvsp[(1) - (1)].expression_type));
	    t_VectorOfRewriteRuleExpressions *vector_of_expressions = new t_VectorOfRewriteRuleExpressions(arguments);
	    //cout<<"Created vector of arguments with only one argument\n";
	    (yyval.vector_of_exprs_type) = vector_of_expressions;
	  ;}
    break;

  case 32:
#line 374 "rules_parser.y"
    { //cout<<"Matching an operator "<<$1<<endl;  
	     (yyval.string_value)=(yyvsp[(1) - (1)].string_value);
	   ;}
    break;


/* Line 1267 of yacc.c.  */
#line 1774 "rules_parser.tab.c"
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


#line 377 "rules_parser.y"







void yyerror(char const *s) 
{
  fprintf(stderr, "%s\n", s);
  fprintf(stderr, "ERROR!! In Parsing the simplification rules file \n");
  abort();
}




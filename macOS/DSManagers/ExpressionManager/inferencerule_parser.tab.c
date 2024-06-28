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
#define yyparse inferencerule_parserparse
#define yylex   inferencerule_parserlex
#define yyerror inferencerule_parsererror
#define yylval  inferencerule_parserlval
#define yychar  inferencerule_parserchar
#define yydebug inferencerule_parserdebug
#define yynerrs inferencerule_parsernerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     t_String = 258,
     t_StringLiteral = 259,
     t_TypeValue = 260,
     t_OBrack = 261,
     t_CBrack = 262,
     t_Comma = 263,
     t_Eq = 264,
     t_OSquareBrack = 265,
     t_CSquareBrack = 266,
     t_OperatorString = 267,
     t_MinArgs = 268,
     t_MaxArgs = 269,
     t_Operand1 = 270,
     t_Operand2 = 271,
     t_Operand3 = 272,
     t_OtherOperandsString = 273,
     t_InferString = 274,
     t_InfinityString = 275,
     t_Hash = 276,
     t_Comments = 277,
     t_Add = 278,
     t_Max = 279,
     t_Min = 280,
     t_Addall = 281,
     INT_NUMBER = 282,
     t_ArgumentNumber = 283,
     t_Variable = 284,
     t_print = 285,
     t_FCBrack = 286,
     t_FOBrack = 287,
     t_Once = 288,
     t_Every = 289,
     t_SemiColon = 290,
     t_KeyWordsOfPrint = 291,
     t_printAll = 292,
     t_CastString = 293
   };
#endif
/* Tokens.  */
#define t_String 258
#define t_StringLiteral 259
#define t_TypeValue 260
#define t_OBrack 261
#define t_CBrack 262
#define t_Comma 263
#define t_Eq 264
#define t_OSquareBrack 265
#define t_CSquareBrack 266
#define t_OperatorString 267
#define t_MinArgs 268
#define t_MaxArgs 269
#define t_Operand1 270
#define t_Operand2 271
#define t_Operand3 272
#define t_OtherOperandsString 273
#define t_InferString 274
#define t_InfinityString 275
#define t_Hash 276
#define t_Comments 277
#define t_Add 278
#define t_Max 279
#define t_Min 280
#define t_Addall 281
#define INT_NUMBER 282
#define t_ArgumentNumber 283
#define t_Variable 284
#define t_print 285
#define t_FCBrack 286
#define t_FOBrack 287
#define t_Once 288
#define t_Every 289
#define t_SemiColon 290
#define t_KeyWordsOfPrint 291
#define t_printAll 292
#define t_CastString 293




/* Copy the first part of user declarations.  */
#line 1 "inferencerule_parser.y"


#include <string>
    //#include "yacc_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>

#include "ExpressionValue.h"
#include "InferTypeOfExpression.h"

    using namespace std;

    int yylex(void);
    void yyerror(char const *);

    string getTypeValue(string type) 
    {
      string result;
      if (type == "SIGNED_INTEGER") 
	{
	  result = toString(TYPE_SIGNED_INTEGER);
	} 
      else if (type == "UNSIGNED_INTEGER") 
	{
	  result = toString(TYPE_UNSIGNED_INTEGER);
	} 
      else if (type == "SIGNED_BITVECTOR") 
	{
	  result = toString(TYPE_SIGNED_BITVECTOR);
	} 
      else if (type == "UNSIGNED_BITVECTOR") 
	{
	  result = toString(TYPE_UNSIGNED_BITVECTOR);
	} 
      else if (type == "REAL") 
	{
	  result = toString(TYPE_REAL);
	} 
      else 
	{
	  result = toString(TYPE_BOOL);
	}
      return result;
    }    
    //Map for handling  Inference Rule
    map <string, t_InferTypeOfExpression *> mapOfTypeInferenceRules;
    


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
#line 57 "inferencerule_parser.y"
{
  char *string_value;
  int int_value;
}
/* Line 193 of yacc.c.  */
#line 240 "inferencerule_parser.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 253 "inferencerule_parser.tab.c"

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
#define YYFINAL  8
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   107

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  39
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  40
/* YYNRULES -- Number of states.  */
#define YYNSTATES  108

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   293

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
      35,    36,    37,    38
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    27,    44,    61,    76,    89,
      91,    96,   102,   104,   109,   116,   123,   130,   135,   138,
     140,   142,   144,   145,   147,   149,   151,   153,   158,   161,
     163,   167,   172,   175,   177,   179,   181,   183,   185,   188,
     192
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      40,     0,    -1,    41,    -1,    41,    40,    -1,    12,    48,
      13,    47,    14,    47,    15,    42,    16,    42,    17,    42,
      18,    42,    19,    43,    49,    55,    -1,    12,    48,    13,
      47,    14,    47,    15,    42,    16,    42,    18,    42,    19,
      43,    49,    55,    -1,    12,    48,    13,    47,    14,    47,
      15,    42,    16,    42,    17,    42,    19,    43,    49,    55,
      -1,    12,    48,    13,    47,    14,    47,    15,    42,    16,
      42,    19,    43,    49,    55,    -1,    12,    48,    13,    47,
      14,    47,    15,    42,    19,    43,    49,    55,    -1,    46,
      -1,     5,    10,    47,    11,    -1,     5,    10,    21,    47,
      11,    -1,    42,    -1,     5,    10,    44,    11,    -1,    24,
       6,    45,     8,    45,     7,    -1,    25,     6,    45,     8,
      45,     7,    -1,    23,     6,    45,     8,    45,     7,    -1,
      26,     6,    45,     7,    -1,    21,    47,    -1,    47,    -1,
      44,    -1,    22,    -1,    -1,    27,    -1,    20,    -1,    29,
      -1,     3,    -1,    30,    32,    50,    31,    -1,    51,    50,
      -1,    51,    -1,    54,    52,    35,    -1,    54,    37,    52,
      35,    -1,    53,    52,    -1,    53,    -1,     4,    -1,    36,
      -1,    33,    -1,    34,    -1,    38,    56,    -1,    27,     8,
      56,    -1,    27,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    97,    97,   101,   106,   184,   244,   282,   325,   354,
     359,   370,   382,   388,   401,   410,   419,   429,   440,   449,
     456,   461,   466,   468,   473,   478,   483,   490,   502,   511,
     517,   526,   536,   545,   554,   559,   564,   573,   583,   589,
     596
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "t_String", "t_StringLiteral",
  "t_TypeValue", "t_OBrack", "t_CBrack", "t_Comma", "t_Eq",
  "t_OSquareBrack", "t_CSquareBrack", "t_OperatorString", "t_MinArgs",
  "t_MaxArgs", "t_Operand1", "t_Operand2", "t_Operand3",
  "t_OtherOperandsString", "t_InferString", "t_InfinityString", "t_Hash",
  "t_Comments", "t_Add", "t_Max", "t_Min", "t_Addall", "INT_NUMBER",
  "t_ArgumentNumber", "t_Variable", "t_print", "t_FCBrack", "t_FOBrack",
  "t_Once", "t_Every", "t_SemiColon", "t_KeyWordsOfPrint", "t_printAll",
  "t_CastString", "$accept", "ListOfInferenceRules", "InferenceRule",
  "TypeTuple", "InferredTypeTuple", "Computation",
  "ArgumentForComputation", "Comments", "Integer", "Operator",
  "PrintString", "ParameterOfPrintBlock", "ParameterOfPrintStatement",
  "ParameterOfPrintWordList", "ParameterOfPrintWord", "OnceOrEvery",
  "TypeCastOperands", "OperandsNumbers", 0
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
     285,   286,   287,   288,   289,   290,   291,   292,   293
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    39,    40,    40,    41,    41,    41,    41,    41,    41,
      42,    42,    43,    43,    44,    44,    44,    44,    45,    45,
      45,    46,    46,    47,    47,    47,    48,    49,    50,    50,
      51,    51,    52,    52,    53,    53,    54,    54,    55,    56,
      56
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,    18,    16,    16,    14,    12,     1,
       4,     5,     1,     4,     6,     6,     6,     4,     2,     1,
       1,     1,     0,     1,     1,     1,     1,     4,     2,     1,
       3,     4,     2,     1,     1,     1,     1,     1,     2,     3,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      22,     0,    21,     0,     2,     9,    26,     0,     1,     3,
       0,    24,    23,    25,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    12,     0,     0,
      10,     0,     0,     0,     0,     0,     0,    11,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     8,     0,
       0,     0,     0,     0,     0,     0,     0,    13,    36,    37,
       0,    29,     0,    40,    38,     0,     0,     0,     7,     0,
      20,     0,    19,     0,     0,     0,    27,    28,    34,    35,
       0,     0,    33,     0,     0,     0,     0,    18,     0,     0,
       0,    17,     0,    30,    32,    39,     0,     6,     5,     0,
       0,     0,    31,     0,    16,    14,    15,     4
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,    27,    28,    70,    71,     5,    72,     7,
      36,    60,    61,    81,    82,    62,    48,    64
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -50
static const yytype_int8 yypact[] =
{
       3,     9,   -50,    26,     3,   -50,   -50,    -9,   -50,   -50,
     -11,   -50,   -50,   -50,    15,   -11,    47,    43,    45,    14,
      18,    43,    63,   -11,    61,    33,    68,   -50,    51,    69,
     -50,    43,    43,    63,    40,    52,    44,   -50,     4,    64,
      51,    79,    80,    81,    82,    78,     1,    65,   -50,    43,
      63,    63,    44,    50,    50,    50,    50,   -50,   -50,   -50,
      59,     1,     7,    83,   -50,    74,    51,    51,   -50,   -11,
     -50,    86,   -50,    87,    88,    90,   -50,   -50,   -50,   -50,
      10,    66,    10,    65,    63,    44,    44,   -50,    50,    50,
      50,   -50,    67,   -50,   -50,   -50,    51,   -50,   -50,    91,
      92,    93,   -50,    44,   -50,   -50,   -50,   -50
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -50,    99,   -50,     0,   -31,    70,   -48,   -50,   -10,   -50,
     -39,    46,   -50,   -24,   -50,   -50,   -49,    22
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      14,    52,    40,    68,    10,    16,    73,    74,    75,    11,
      24,    78,     6,    29,    78,     1,    12,    19,    13,    66,
      67,    25,    49,    50,    24,     2,     8,    85,    86,    15,
      21,    38,    39,    22,    58,    59,    97,    98,    11,    23,
      99,   100,   101,    79,    80,    12,    79,    13,    18,    65,
      31,    32,    33,    96,   107,    20,    92,   103,    94,    87,
      11,    23,    17,    41,    42,    43,    44,    12,    26,    13,
      11,    69,    30,    41,    42,    43,    44,    12,    34,    13,
      37,    35,    47,    51,    46,    53,    54,    55,    56,    57,
      76,    83,    63,    84,    88,    89,    90,    91,   104,   105,
     106,    93,   102,     9,    45,    95,     0,    77
};

static const yytype_int8 yycheck[] =
{
      10,    40,    33,    52,    13,    15,    54,    55,    56,    20,
      20,     4,     3,    23,     4,    12,    27,    17,    29,    50,
      51,    21,    18,    19,    34,    22,     0,    66,    67,    14,
      16,    31,    32,    19,    33,    34,    85,    86,    20,    21,
      88,    89,    90,    36,    37,    27,    36,    29,     5,    49,
      17,    18,    19,    84,   103,    10,    80,    96,    82,    69,
      20,    21,    15,    23,    24,    25,    26,    27,     5,    29,
      20,    21,    11,    23,    24,    25,    26,    27,    10,    29,
      11,    30,    38,    19,    32,     6,     6,     6,     6,    11,
      31,     8,    27,    19,     8,     8,     8,     7,     7,     7,
       7,    35,    35,     4,    34,    83,    -1,    61
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    12,    22,    40,    41,    46,     3,    48,     0,    40,
      13,    20,    27,    29,    47,    14,    47,    15,     5,    42,
      10,    16,    19,    21,    47,    42,     5,    42,    43,    47,
      11,    17,    18,    19,    10,    30,    49,    11,    42,    42,
      43,    23,    24,    25,    26,    44,    32,    38,    55,    18,
      19,    19,    49,     6,     6,     6,     6,    11,    33,    34,
      50,    51,    54,    27,    56,    42,    43,    43,    55,    21,
      44,    45,    47,    45,    45,    45,    31,    50,     4,    36,
      37,    52,    53,     8,    19,    49,    49,    47,     8,     8,
       8,     7,    52,    35,    52,    56,    43,    55,    55,    45,
      45,    45,    35,    49,     7,     7,     7,    55
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
#line 98 "inferencerule_parser.y"
    {
			  ////cout << "Inference rule is there\n";
			;}
    break;

  case 3:
#line 102 "inferencerule_parser.y"
    {
			   ////cout << "Inference Rule\n";
			 ;}
    break;

  case 4:
#line 107 "inferencerule_parser.y"
    {
				bool variableNumberOfArguments = ((yyvsp[(4) - (18)].int_value) != (yyvsp[(6) - (18)].int_value)) ? true : false;
				bool unlimitedNumberOfArguments = ((yyvsp[(6) - (18)].int_value) == INFINITY_WIDTH) ? true : false;
		    		////cout << "Unlimited operads:" << unlimitedNumberOfArguments;
				/** **Special Case**
				 * Here handling only the operator that is applied to single type and
     				 * infinite number of arguments (Symmetric )
     				 */
				for (int i = (yyvsp[(4) - (18)].int_value); i <= (yyvsp[(6) - (18)].int_value); i++) {
        				string signature;
        				t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

        				string operatorSymbol = toString((yyvsp[(2) - (18)].string_value));

        				int minargs = (yyvsp[(4) - (18)].int_value);
        				int maxargs = i;
        				vector <TypeOfExpressionTuple *> operandTypes;
        				if (unlimitedNumberOfArguments)
                                        {
            					inferenceRule->setOperand1Type(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(8) - (18)].string_value)));
            					operandTypes.push_back(inferenceRule->getOperand1Type());
            					inferenceRule->setOperand2Type( inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(10) - (18)].string_value)));
            					operandTypes.push_back(inferenceRule->getOperand2Type());
            					inferenceRule->setOperand3Type(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(12) - (18)].string_value)));
            					operandTypes.push_back(inferenceRule->getOperand3Type());
            					inferenceRule->setOtherOperandsType(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(14) - (18)].string_value)));
            					operandTypes.push_back(inferenceRule->getOtherOperandsType());
            					inferenceRule->setInferredType ((yyvsp[(16) - (18)].string_value));
                                                inferenceRule->setPrintExpressionFormat(toString((yyvsp[(17) - (18)].string_value)));
						inferenceRule->type_cast_operands = toString((yyvsp[(18) - (18)].string_value));
                                                //signature = inferenceRule->generateKey($2, operandTypes);
						// Key is only with one operand type
						signature = operatorSymbol + "@";
						signature = signature + toString(operandTypes[0]->m_type);

            					////cout << "signature\n" << signature 
						//   << "\nInserting into the map\n";
            					mapOfTypeInferenceRules[signature] = inferenceRule;
            					break;
        				} 
					else 
					{
            					if (i >= 1) 
						{
	                				inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(8) - (18)].string_value));
                					operandTypes.push_back(inferenceRule->operand1Type);
            					}
            					if (i >= 2) 
						{
                					inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(10) - (18)].string_value));
                					operandTypes.push_back(inferenceRule->operand2Type);
            					}
            					if (i >= 3) 
						{
                					inferenceRule->operand3Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(12) - (18)].string_value));
                					operandTypes.push_back(inferenceRule->operand3Type);
            					}
            					if (i > 3) 
						{
                					inferenceRule->otherOperandsType = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(14) - (18)].string_value));
                					operandTypes.push_back(inferenceRule->otherOperandsType);
            					}
            					signature = inferenceRule->generateKey((yyvsp[(2) - (18)].string_value), operandTypes);
            					inferenceRule->inferredType = (yyvsp[(16) - (18)].string_value);
						inferenceRule->type_cast_operands = toString((yyvsp[(18) - (18)].string_value));
                                                inferenceRule->printExpressionFormat=toString((yyvsp[(17) - (18)].string_value));
            					mapOfTypeInferenceRules[signature] = inferenceRule;
            					////cout << "\nsignature " << signature << "\nInserting into the map\n";
            					mapOfTypeInferenceRules[signature] = inferenceRule;
        				}
        				/**
         			         *  For Infinte length only consider the operator Symbol as Signature
         				 *  push Only other Operand type
         				 */
        				////cout<<"Inference accepted"<<endl;
    				}
			;}
    break;

  case 5:
#line 185 "inferencerule_parser.y"
    {
    				bool variableNumberOfArguments = ((yyvsp[(4) - (16)].int_value) != (yyvsp[(6) - (16)].int_value)) ? true : false;
    				bool unlimitedNumberOfArguments = ((yyvsp[(6) - (16)].int_value) == INFINITY_WIDTH) ? true : false;
    				//cout << "Unlimited operads:" << unlimitedNumberOfArguments;
    				/** **Special Case**
     				 * Here handling only the operator that is applied to single type and
     				 * infinite number of arguments (Symmetric )
     				 */
    				for (int i = (yyvsp[(4) - (16)].int_value); i <= (yyvsp[(6) - (16)].int_value); i++) {
		        		string signature;
        				t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

        				inferenceRule->operatorSymbol = toString((yyvsp[(2) - (16)].string_value));

        				inferenceRule->minargs = (yyvsp[(4) - (16)].int_value);
        				inferenceRule->maxargs = i;
        				inferenceRule->variableNumberOfArguments = variableNumberOfArguments;
        				inferenceRule->unlimitedNumberOfArguments = unlimitedNumberOfArguments;
                                        inferenceRule->printExpressionFormat=toString((yyvsp[(15) - (16)].string_value));
					inferenceRule->type_cast_operands = toString((yyvsp[(16) - (16)].string_value));
        				vector <TypeOfExpressionTuple *> operandTypes;
					
        				if (unlimitedNumberOfArguments) {
					  //signature = inferenceRule->generateKey($2, operandTypes);
					  inferenceRule->maxargs = INFINITY_WIDTH;
					  operandTypes.push_back(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(8) - (16)].string_value)));
					  inferenceRule->setOperand1Type(operandTypes[0]);
						signature = inferenceRule->operatorSymbol + "@";
						signature = signature + toString(operandTypes[0]->m_type);

            					//cout << "signature\n" << signature << "\nInserting into the map\n";
				            	inferenceRule->inferredType = (yyvsp[(14) - (16)].string_value);
            					mapOfTypeInferenceRules[signature] = inferenceRule;
            					break;
        				} 
					else 
					{
 				           if (i >= 1) 
					   {
	                			inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(8) - (16)].string_value));
                				operandTypes.push_back(inferenceRule->operand1Type);
            				   }
            				   if (i >= 2) 
					   {
			                	inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(10) - (16)].string_value));
                				operandTypes.push_back(inferenceRule->operand2Type);
            				   }
            				   inferenceRule->otherOperandsType = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(12) - (16)].string_value));
            				   operandTypes.push_back(inferenceRule->otherOperandsType);
            				   signature = inferenceRule->generateKey((yyvsp[(2) - (16)].string_value), operandTypes);
            				   inferenceRule->inferredType = (yyvsp[(14) - (16)].string_value);
            				   mapOfTypeInferenceRules[signature] = inferenceRule;
            				   //cout << "\nsignature " << signature << "\nInserting into the map\n";
            				   mapOfTypeInferenceRules[signature] = inferenceRule;
        				}			
        				//cout<<"Inference accepted"<<endl;
    				}
				//cout << "\nPrints:" << $15;
			;}
    break;

  case 6:
#line 245 "inferencerule_parser.y"
    {
	    			bool variableNumberOfArguments = ((yyvsp[(4) - (16)].int_value) != (yyvsp[(6) - (16)].int_value)) ? true : false;
    				bool unlimitedNumberOfArguments = ((yyvsp[(6) - (16)].int_value) == INFINITY_WIDTH) ? true : false;
    				for (int i = (yyvsp[(4) - (16)].int_value); i <= (yyvsp[(6) - (16)].int_value); i++) 
				{
				        string signature;

				        t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

				        inferenceRule->operatorSymbol = toString((yyvsp[(2) - (16)].string_value));
	
				        inferenceRule->minargs = (yyvsp[(4) - (16)].int_value);
				        inferenceRule->maxargs = i;
                                        inferenceRule->printExpressionFormat=toString((yyvsp[(15) - (16)].string_value));
					inferenceRule->type_cast_operands = toString((yyvsp[(16) - (16)].string_value));
				        vector <TypeOfExpressionTuple *> operandTypes;
				        inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(8) - (16)].string_value));
				        operandTypes.push_back(inferenceRule->operand1Type);
					
				        if (i >= 2) 
					{
				            inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(10) - (16)].string_value));
				            operandTypes.push_back(inferenceRule->operand2Type);
				        }
				        if (i >= 3) 
					{
				            inferenceRule->operand3Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(12) - (16)].string_value));
				            operandTypes.push_back(inferenceRule->operand3Type);
				        }
				        signature = inferenceRule->generateKey((yyvsp[(2) - (16)].string_value), operandTypes);
				        inferenceRule->variableNumberOfArguments = variableNumberOfArguments;
				        inferenceRule->inferredType = (yyvsp[(14) - (16)].string_value);
				        mapOfTypeInferenceRules[signature] = inferenceRule;
				        //cout << "signature\n" << signature << "\nInserting into the map\n";
				        //cout<<"3 Operands";
				    }
			;}
    break;

  case 7:
#line 283 "inferencerule_parser.y"
    {
			    bool variableNumberOfArguments = ((yyvsp[(4) - (14)].int_value) != (yyvsp[(6) - (14)].int_value)) ? true : false;
			    bool unlimitedNumberOfArguments = ((yyvsp[(6) - (14)].int_value) == INFINITY_WIDTH) ? true : false;
			    for (int i = (yyvsp[(4) - (14)].int_value); i <= (yyvsp[(6) - (14)].int_value); i++) 
			    {
        			if ((yyvsp[(6) - (14)].int_value) != 2) 
				{
				  string str;
				  str = "Error while parsing file (Minargs != Maxargs or Maxargs  not 2)";
				  char * msg = new char[str.length() + 1];
				  strcpy(msg, str.c_str());
				  yyerror(msg);
			        }
				//cout<<"Inference rule of 2 operands:";
			        
			        t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();
				string signature;

			        inferenceRule->operatorSymbol = toString((yyvsp[(2) - (14)].string_value));

			        inferenceRule->minargs = i;
			        inferenceRule->maxargs = i;

                                inferenceRule->printExpressionFormat=toString((yyvsp[(13) - (14)].string_value));
				inferenceRule->type_cast_operands = toString((yyvsp[(14) - (14)].string_value));
			        vector <TypeOfExpressionTuple *> operandTypes;

			        inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(8) - (14)].string_value));
			        operandTypes.push_back(inferenceRule->operand1Type);

			        if (i == 2) 
				{
			            inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(10) - (14)].string_value));
			            operandTypes.push_back(inferenceRule->operand2Type);
			        }
			        signature = inferenceRule->generateKey((yyvsp[(2) - (14)].string_value), operandTypes);
			        inferenceRule->inferredType = (yyvsp[(12) - (14)].string_value);
			        inferenceRule->variableNumberOfArguments = variableNumberOfArguments;
			        mapOfTypeInferenceRules[signature] = inferenceRule;
			        //cout << "\nsignature " << signature << "\nInserted into the map\n";
			    }
			;}
    break;

  case 8:
#line 326 "inferencerule_parser.y"
    {
			    string signature;
			    if ((yyvsp[(6) - (12)].int_value) != (yyvsp[(4) - (12)].int_value) || (yyvsp[(4) - (12)].int_value) != 1) 
			    {
			      string str;
			      str = "Error while parsing file (Minargs != Maxargs or Maxargs  not 1)";
			      char * msg = new char[str.length() + 1];
			      strcpy(msg, str.c_str());
			      yyerror(msg);
    			    }
			    t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

			    inferenceRule->operatorSymbol = toString((yyvsp[(2) - (12)].string_value));

			    inferenceRule->minargs = (yyvsp[(4) - (12)].int_value);
			    inferenceRule->maxargs = (yyvsp[(6) - (12)].int_value);
                            inferenceRule->printExpressionFormat=toString((yyvsp[(11) - (12)].string_value));
			    inferenceRule->type_cast_operands = toString((yyvsp[(12) - (12)].string_value));
			    vector <TypeOfExpressionTuple *> operandTypes;
			    inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[(8) - (12)].string_value));
			    operandTypes.push_back(inferenceRule->operand1Type);
			    signature = inferenceRule->generateKey((yyvsp[(2) - (12)].string_value), operandTypes);
			    inferenceRule->inferredType = (yyvsp[(10) - (12)].string_value);
			    mapOfTypeInferenceRules[signature] = inferenceRule;

			    //cout << "\nsignature " << signature << "Inserted into map\n";
    			    //cout<<"Single Oprand type inferencing \n";
			;}
    break;

  case 9:
#line 355 "inferencerule_parser.y"
    {
			    //cout<<"Comments Accepted"<<endl;
			 ;}
    break;

  case 10:
#line 360 "inferencerule_parser.y"
    {
			    string passtype((yyvsp[(1) - (4)].string_value));
			    string result = getTypeValue(passtype);
			    result += "@";
			    result += toString((yyvsp[(3) - (4)].int_value));
			    char *res = new char[result.length() + 1];
			    strcpy(res, result.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Type tuple is being created \n";
			;}
    break;

  case 11:
#line 371 "inferencerule_parser.y"
    {
			    string passtype((yyvsp[(1) - (5)].string_value));
			    string result = getTypeValue(passtype);
			    result += "@#";
			    result += toString((yyvsp[(4) - (5)].int_value));
			    char *res = new char[result.length() + 1];
			    strcpy(res, result.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Type tuple with some operand length is recieved \n";
			;}
    break;

  case 12:
#line 383 "inferencerule_parser.y"
    {
			    //cout << "Inferring the type directly according to operand\n";
			    //cout << $1;
			    (yyval.string_value) = (yyvsp[(1) - (1)].string_value);
			;}
    break;

  case 13:
#line 389 "inferencerule_parser.y"
    {
			    string passtype((yyvsp[(1) - (4)].string_value));
			    string result = getTypeValue(passtype);
			    result += "@";
			    result += toString((yyvsp[(3) - (4)].string_value));
			    char *res = new char[result.length() + 1];
			    strcpy(res, result.c_str());
			    (yyval.string_value) = res;
			    //cout<<"\nInferred type:"<<res;
			    //cout<<"Inferring the tuple along with some computation\n";
			;}
    break;

  case 14:
#line 402 "inferencerule_parser.y"
    {
			    string computation;
			    computation = "%max(" + toString((yyvsp[(3) - (6)].string_value)) + "," + toString((yyvsp[(5) - (6)].string_value)) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computation - Max\n"<<res;
			;}
    break;

  case 15:
#line 411 "inferencerule_parser.y"
    {
			    string computation;
			    computation = "%min(" + toString((yyvsp[(3) - (6)].string_value)) + "," + toString((yyvsp[(5) - (6)].string_value)) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computataion - Min\n"<<res;
			;}
    break;

  case 16:
#line 420 "inferencerule_parser.y"
    {
			    string computation;	
			    computation = "%add(" + toString((yyvsp[(3) - (6)].string_value));
			    computation = computation + "," + toString((yyvsp[(5) - (6)].string_value)) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computataion - Min\n"<<res;
			;}
    break;

  case 17:
#line 430 "inferencerule_parser.y"
    {
			    string computation;
			    computation = "%addall(";
			    computation += (toString((yyvsp[(3) - (4)].string_value)) + ")");
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computation - Add all\n"<<res;
			;}
    break;

  case 18:
#line 441 "inferencerule_parser.y"
    {
			    string res = "#";
			    res.append(toString((yyvsp[(2) - (2)].int_value)));
			    char *resCopy = new char[res.length() + 1];
			    strcpy(resCopy, res.c_str());
			//cout<<"Hash Int:"<<$2;
			    (yyval.string_value) = resCopy;
			;}
    break;

  case 19:
#line 450 "inferencerule_parser.y"
    {
			    string str(toString((yyvsp[(1) - (1)].int_value)));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
			    (yyval.string_value) = res;
			;}
    break;

  case 20:
#line 457 "inferencerule_parser.y"
    {
			    (yyval.string_value) = (yyvsp[(1) - (1)].string_value);
			;}
    break;

  case 21:
#line 462 "inferencerule_parser.y"
    {
			    //Nothing to do
			    //cout<<"Recieved Comments Nothing to do\n";
			;}
    break;

  case 23:
#line 469 "inferencerule_parser.y"
    {
			    (yyval.int_value) = (yyvsp[(1) - (1)].int_value);
			    //cout<<"Integer Number\n"<<$1;
			;}
    break;

  case 24:
#line 474 "inferencerule_parser.y"
    {
			    (yyval.int_value) = INFINITY_WIDTH;
			   // //cout << "Infinity Recieved\n";
			;}
    break;

  case 25:
#line 479 "inferencerule_parser.y"
    {
			    (yyval.int_value) = VARIABLE_WIDTH; //For bitvector SIGNED_BITVECTOR[$k]
			;}
    break;

  case 26:
#line 484 "inferencerule_parser.y"
    {
			    //cout<<"Operator in YACC:"<<$1;
    				(yyval.string_value) = (yyvsp[(1) - (1)].string_value);
			;}
    break;

  case 27:
#line 491 "inferencerule_parser.y"
    {
			    //cout<<endl<<"----";
			    //cout<<endl<<toString($3)<<endl;
			    string str((yyvsp[(3) - (4)].string_value));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
    			    (yyval.string_value) = res;
                            //cout<<"\n"<<res;
			;}
    break;

  case 28:
#line 503 "inferencerule_parser.y"
    {
			    //cout<<$1<<endl<<$2<<endl;
				string res((yyvsp[(1) - (2)].string_value));
				res.append((yyvsp[(2) - (2)].string_value));
				char *result=new char[res.length()+1];
				strcpy(result,res.c_str());
				(yyval.string_value)=result;	
			;}
    break;

  case 29:
#line 512 "inferencerule_parser.y"
    {
			    (yyval.string_value) = (yyvsp[(1) - (1)].string_value);
			    //cout<<$1<<endl;
			;}
    break;

  case 30:
#line 518 "inferencerule_parser.y"
    {
			   // //cout<<$1<<endl<<$2<<endl;
			    string str((yyvsp[(1) - (3)].string_value));
			    str += (toString((yyvsp[(2) - (3)].string_value)) + ";");
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
                            (yyval.string_value) = res;
			;}
    break;

  case 31:
#line 527 "inferencerule_parser.y"
    {
                            string str((yyvsp[(1) - (4)].string_value));
                            str+= ("$printall"+toString((yyvsp[(3) - (4)].string_value))+";");
                            char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
                            (yyval.string_value) = res;
                        ;}
    break;

  case 32:
#line 537 "inferencerule_parser.y"
    {
			    string str((yyvsp[(1) - (2)].string_value));
			    str += (" " + toString((yyvsp[(2) - (2)].string_value)));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
			    (yyval.string_value) = res;
                           //cout<<res<<endl;
			;}
    break;

  case 33:
#line 546 "inferencerule_parser.y"
    {
			    //char *res= new char[str.length()+1];
			    //strcpy(res,str.c_str());
			    //$$=res;
			    (yyval.string_value) = (yyvsp[(1) - (1)].string_value);
			    //cout<<$1<<endl;
			;}
    break;

  case 34:
#line 555 "inferencerule_parser.y"
    {
			    (yyval.string_value) = (yyvsp[(1) - (1)].string_value);
			    //cout<<$1<<endl;
			;}
    break;

  case 35:
#line 560 "inferencerule_parser.y"
    {
    				(yyval.string_value) = (yyvsp[(1) - (1)].string_value);
			    //cout<<$1<<endl;
			;}
    break;

  case 36:
#line 565 "inferencerule_parser.y"
    {
			  string str("$once");
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  (yyval.string_value) = res;
			  // $$ = "$once";
			  //cout<<"Once"<<endl;
			;}
    break;

  case 37:
#line 574 "inferencerule_parser.y"
    {
			  string str("$every");
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  (yyval.string_value) = res;
			  // $$ = "$every";
			  //cout<<"Every"<<endl;
			;}
    break;

  case 38:
#line 584 "inferencerule_parser.y"
    {
			 //cout<<"Type cast:"<<$2<<endl;
			 (yyval.string_value) = (yyvsp[(2) - (2)].string_value);
		       ;}
    break;

  case 39:
#line 590 "inferencerule_parser.y"
    {
			  string str(toString((yyvsp[(1) - (3)].int_value)));
			  str += ("," + toString((yyvsp[(3) - (3)].string_value)));
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  (yyval.string_value) = res; 
			;}
    break;

  case 40:
#line 597 "inferencerule_parser.y"
    {
			  string str(toString((yyvsp[(1) - (1)].int_value)));
			  if( (yyvsp[(1) - (1)].int_value) == 0 )
			    {
			      str = "";
			    }			  
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  (yyval.string_value) = res;
			;}
    break;


/* Line 1267 of yacc.c.  */
#line 2144 "inferencerule_parser.tab.c"
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


#line 607 "inferencerule_parser.y"



void yyerror(char const *s) 
{
    fprintf(stderr, "%s\n", s);
    abort();
}
/*
int main() {
    yyparse();
    return 0;
}
*/


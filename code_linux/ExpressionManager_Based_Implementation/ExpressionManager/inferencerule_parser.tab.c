/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         inferencerule_parserparse
#define yylex           inferencerule_parserlex
#define yyerror         inferencerule_parsererror
#define yydebug         inferencerule_parserdebug
#define yynerrs         inferencerule_parsernerrs

#define yylval          inferencerule_parserlval
#define yychar          inferencerule_parserchar

/* Copy the first part of user declarations.  */
#line 1 "inferencerule_parser.y" /* yacc.c:339  */


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
    void yyerror(char *);

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
    

#line 129 "inferencerule_parser.tab.c" /* yacc.c:339  */

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

/* In a future release of Bison, this section will be replaced
   by #include "inferencerule_parser.tab.h".  */
#ifndef YY_INFERENCERULE_PARSER_INFERENCERULE_PARSER_TAB_H_INCLUDED
# define YY_INFERENCERULE_PARSER_INFERENCERULE_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int inferencerule_parserdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 57 "inferencerule_parser.y" /* yacc.c:355  */

  char *string_value;
  int int_value;

#line 213 "inferencerule_parser.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE inferencerule_parserlval;

int inferencerule_parserparse (void);

#endif /* !YY_INFERENCERULE_PARSER_INFERENCERULE_PARSER_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 230 "inferencerule_parser.tab.c" /* yacc.c:358  */

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
#define YYFINAL  8
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   107

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  39
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  40
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  108

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   293

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
      35,    36,    37,    38
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    97,    97,   101,   106,   184,   244,   282,   321,   346,
     351,   362,   374,   380,   393,   402,   411,   421,   432,   441,
     448,   453,   458,   460,   465,   470,   475,   482,   494,   503,
     509,   518,   528,   537,   546,   551,   556,   561,   567,   573,
     580
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
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
  "TypeCastOperands", "OperandsNumbers", YY_NULLPTR
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
     285,   286,   287,   288,   289,   290,   291,   292,   293
};
# endif

#define YYPACT_NINF -50

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-50)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
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

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
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

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -50,    99,   -50,     0,   -31,    70,   -48,   -50,   -10,   -50,
     -39,    46,   -50,   -24,   -50,   -50,   -49,    22
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     3,     4,    27,    28,    70,    71,     5,    72,     7,
      36,    60,    61,    81,    82,    62,    48,    64
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
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

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    39,    40,    40,    41,    41,    41,    41,    41,    41,
      42,    42,    43,    43,    44,    44,    44,    44,    45,    45,
      45,    46,    46,    47,    47,    47,    48,    49,    50,    50,
      51,    51,    52,    52,    53,    53,    54,    54,    55,    56,
      56
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,    18,    16,    16,    14,    12,     1,
       4,     5,     1,     4,     6,     6,     6,     4,     2,     1,
       1,     1,     0,     1,     1,     1,     1,     4,     2,     1,
       3,     4,     2,     1,     1,     1,     1,     1,     2,     3,
       1
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
      yyerror (YY_("syntax error: cannot back up")); \
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
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
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
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
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
      yychar = yylex ();
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
#line 98 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			  ////cout << "Inference rule is there\n";
			}
#line 1380 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 102 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			   ////cout << "Inference Rule\n";
			 }
#line 1388 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 107 "inferencerule_parser.y" /* yacc.c:1646  */
    {
				bool variableNumberOfArguments = ((yyvsp[-14].int_value) != (yyvsp[-12].int_value)) ? true : false;
				bool unlimitedNumberOfArguments = ((yyvsp[-12].int_value) == INFINITY_WIDTH) ? true : false;
		    		////cout << "Unlimited operads:" << unlimitedNumberOfArguments;
				/** **Special Case**
				 * Here handling only the operator that is applied to single type and
     				 * infinite number of arguments (Symmetric )
     				 */
				for (int i = (yyvsp[-14].int_value); i <= (yyvsp[-12].int_value); i++) {
        				string signature;
        				t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

        				string operatorSymbol = toString((yyvsp[-16].string_value));

        				int minargs = (yyvsp[-14].int_value);
        				int maxargs = i;
        				vector <TypeOfExpressionTuple *> operandTypes;
        				if (unlimitedNumberOfArguments)
                                        {
            					inferenceRule->setOperand1Type(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-10].string_value)));
            					operandTypes.push_back(inferenceRule->getOperand1Type());
            					inferenceRule->setOperand2Type( inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-8].string_value)));
            					operandTypes.push_back(inferenceRule->getOperand2Type());
            					inferenceRule->setOperand3Type(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-6].string_value)));
            					operandTypes.push_back(inferenceRule->getOperand3Type());
            					inferenceRule->setOtherOperandsType(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-4].string_value)));
            					operandTypes.push_back(inferenceRule->getOtherOperandsType());
            					inferenceRule->setInferredType ((yyvsp[-2].string_value));
                                                inferenceRule->setPrintExpressionFormat(toString((yyvsp[-1].string_value)));
						inferenceRule->type_cast_operands = toString((yyvsp[0].string_value));
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
	                				inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-10].string_value));
                					operandTypes.push_back(inferenceRule->operand1Type);
            					}
            					if (i >= 2) 
						{
                					inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-8].string_value));
                					operandTypes.push_back(inferenceRule->operand2Type);
            					}
            					if (i >= 3) 
						{
                					inferenceRule->operand3Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-6].string_value));
                					operandTypes.push_back(inferenceRule->operand3Type);
            					}
            					if (i > 3) 
						{
                					inferenceRule->otherOperandsType = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-4].string_value));
                					operandTypes.push_back(inferenceRule->otherOperandsType);
            					}
            					signature = inferenceRule->generateKey((yyvsp[-16].string_value), operandTypes);
            					inferenceRule->inferredType = (yyvsp[-2].string_value);
						inferenceRule->type_cast_operands = toString((yyvsp[0].string_value));
                                                inferenceRule->printExpressionFormat=toString((yyvsp[-1].string_value));
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
			}
#line 1470 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 185 "inferencerule_parser.y" /* yacc.c:1646  */
    {
    				bool variableNumberOfArguments = ((yyvsp[-12].int_value) != (yyvsp[-10].int_value)) ? true : false;
    				bool unlimitedNumberOfArguments = ((yyvsp[-10].int_value) == INFINITY_WIDTH) ? true : false;
    				//cout << "Unlimited operads:" << unlimitedNumberOfArguments;
    				/** **Special Case**
     				 * Here handling only the operator that is applied to single type and
     				 * infinite number of arguments (Symmetric )
     				 */
    				for (int i = (yyvsp[-12].int_value); i <= (yyvsp[-10].int_value); i++) {
		        		string signature;
        				t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

        				inferenceRule->operatorSymbol = toString((yyvsp[-14].string_value));

        				inferenceRule->minargs = (yyvsp[-12].int_value);
        				inferenceRule->maxargs = i;
        				inferenceRule->variableNumberOfArguments = variableNumberOfArguments;
        				inferenceRule->unlimitedNumberOfArguments = unlimitedNumberOfArguments;
                                        inferenceRule->printExpressionFormat=toString((yyvsp[-1].string_value));
					inferenceRule->type_cast_operands = toString((yyvsp[0].string_value));
        				vector <TypeOfExpressionTuple *> operandTypes;
					
        				if (unlimitedNumberOfArguments) {
					  //signature = inferenceRule->generateKey($2, operandTypes);
					  inferenceRule->maxargs = INFINITY_WIDTH;
					  operandTypes.push_back(inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-8].string_value)));
					  inferenceRule->setOperand1Type(operandTypes[0]);
						signature = inferenceRule->operatorSymbol + "@";
						signature = signature + toString(operandTypes[0]->m_type);

            					//cout << "signature\n" << signature << "\nInserting into the map\n";
				            	inferenceRule->inferredType = (yyvsp[-2].string_value);
            					mapOfTypeInferenceRules[signature] = inferenceRule;
            					break;
        				} 
					else 
					{
 				           if (i >= 1) 
					   {
	                			inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-8].string_value));
                				operandTypes.push_back(inferenceRule->operand1Type);
            				   }
            				   if (i >= 2) 
					   {
			                	inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-6].string_value));
                				operandTypes.push_back(inferenceRule->operand2Type);
            				   }
            				   inferenceRule->otherOperandsType = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-4].string_value));
            				   operandTypes.push_back(inferenceRule->otherOperandsType);
            				   signature = inferenceRule->generateKey((yyvsp[-14].string_value), operandTypes);
            				   inferenceRule->inferredType = (yyvsp[-2].string_value);
            				   mapOfTypeInferenceRules[signature] = inferenceRule;
            				   //cout << "\nsignature " << signature << "\nInserting into the map\n";
            				   mapOfTypeInferenceRules[signature] = inferenceRule;
        				}			
        				//cout<<"Inference accepted"<<endl;
    				}
				//cout << "\nPrints:" << $15;
			}
#line 1534 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 245 "inferencerule_parser.y" /* yacc.c:1646  */
    {
	    			bool variableNumberOfArguments = ((yyvsp[-12].int_value) != (yyvsp[-10].int_value)) ? true : false;
    				bool unlimitedNumberOfArguments = ((yyvsp[-10].int_value) == INFINITY_WIDTH) ? true : false;
    				for (int i = (yyvsp[-12].int_value); i <= (yyvsp[-10].int_value); i++) 
				{
				        string signature;

				        t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

				        inferenceRule->operatorSymbol = toString((yyvsp[-14].string_value));
	
				        inferenceRule->minargs = (yyvsp[-12].int_value);
				        inferenceRule->maxargs = i;
                                        inferenceRule->printExpressionFormat=toString((yyvsp[-1].string_value));
					inferenceRule->type_cast_operands = toString((yyvsp[0].string_value));
				        vector <TypeOfExpressionTuple *> operandTypes;
				        inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-8].string_value));
				        operandTypes.push_back(inferenceRule->operand1Type);
					
				        if (i >= 2) 
					{
				            inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-6].string_value));
				            operandTypes.push_back(inferenceRule->operand2Type);
				        }
				        if (i >= 3) 
					{
				            inferenceRule->operand3Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-4].string_value));
				            operandTypes.push_back(inferenceRule->operand3Type);
				        }
				        signature = inferenceRule->generateKey((yyvsp[-14].string_value), operandTypes);
				        inferenceRule->variableNumberOfArguments = variableNumberOfArguments;
				        inferenceRule->inferredType = (yyvsp[-2].string_value);
				        mapOfTypeInferenceRules[signature] = inferenceRule;
				        //cout << "signature\n" << signature << "\nInserting into the map\n";
				        //cout<<"3 Operands";
				    }
			}
#line 1576 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 283 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    bool variableNumberOfArguments = ((yyvsp[-10].int_value) != (yyvsp[-8].int_value)) ? true : false;
			    bool unlimitedNumberOfArguments = ((yyvsp[-8].int_value) == INFINITY_WIDTH) ? true : false;
			    for (int i = (yyvsp[-10].int_value); i <= (yyvsp[-8].int_value); i++) 
			    {
        			if ((yyvsp[-8].int_value) != 2) 
				{
			            yyerror("Error while parsing file (Minargs != Maxargs or Maxargs  not 2)");
			        }
				//cout<<"Inference rule of 2 operands:";
			        
			        t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();
				string signature;

			        inferenceRule->operatorSymbol = toString((yyvsp[-12].string_value));

			        inferenceRule->minargs = i;
			        inferenceRule->maxargs = i;

                                inferenceRule->printExpressionFormat=toString((yyvsp[-1].string_value));
				inferenceRule->type_cast_operands = toString((yyvsp[0].string_value));
			        vector <TypeOfExpressionTuple *> operandTypes;

			        inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-6].string_value));
			        operandTypes.push_back(inferenceRule->operand1Type);

			        if (i == 2) 
				{
			            inferenceRule->operand2Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-4].string_value));
			            operandTypes.push_back(inferenceRule->operand2Type);
			        }
			        signature = inferenceRule->generateKey((yyvsp[-12].string_value), operandTypes);
			        inferenceRule->inferredType = (yyvsp[-2].string_value);
			        inferenceRule->variableNumberOfArguments = variableNumberOfArguments;
			        mapOfTypeInferenceRules[signature] = inferenceRule;
			        //cout << "\nsignature " << signature << "\nInserted into the map\n";
			    }
			}
#line 1619 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 322 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string signature;
			    if ((yyvsp[-6].int_value) != (yyvsp[-8].int_value) || (yyvsp[-8].int_value) != 1) 
			    {
			        yyerror("Error while parsing file (Minargs != Maxargs or Maxargs  not 1)");
    			    }
			    t_InferTypeOfExpression *inferenceRule = new t_InferTypeOfExpression();

			    inferenceRule->operatorSymbol = toString((yyvsp[-10].string_value));

			    inferenceRule->minargs = (yyvsp[-8].int_value);
			    inferenceRule->maxargs = (yyvsp[-6].int_value);
                            inferenceRule->printExpressionFormat=toString((yyvsp[-1].string_value));
			    inferenceRule->type_cast_operands = toString((yyvsp[0].string_value));
			    vector <TypeOfExpressionTuple *> operandTypes;
			    inferenceRule->operand1Type = inferenceRule->stringToTypeOfExpressionTupleWithNoComputations((yyvsp[-4].string_value));
			    operandTypes.push_back(inferenceRule->operand1Type);
			    signature = inferenceRule->generateKey((yyvsp[-10].string_value), operandTypes);
			    inferenceRule->inferredType = (yyvsp[-2].string_value);
			    mapOfTypeInferenceRules[signature] = inferenceRule;

			    //cout << "\nsignature " << signature << "Inserted into map\n";
    			    //cout<<"Single Oprand type inferencing \n";
			}
#line 1648 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 347 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    //cout<<"Comments Accepted"<<endl;
			 }
#line 1656 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 352 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string passtype((yyvsp[-3].string_value));
			    string result = getTypeValue(passtype);
			    result += "@";
			    result += toString((yyvsp[-1].int_value));
			    char *res = new char[result.length() + 1];
			    strcpy(res, result.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Type tuple is being created \n";
			}
#line 1671 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 363 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string passtype((yyvsp[-4].string_value));
			    string result = getTypeValue(passtype);
			    result += "@#";
			    result += toString((yyvsp[-1].int_value));
			    char *res = new char[result.length() + 1];
			    strcpy(res, result.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Type tuple with some operand length is recieved \n";
			}
#line 1686 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 375 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    //cout << "Inferring the type directly according to operand\n";
			    //cout << $1;
			    (yyval.string_value) = (yyvsp[0].string_value);
			}
#line 1696 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 381 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string passtype((yyvsp[-3].string_value));
			    string result = getTypeValue(passtype);
			    result += "@";
			    result += toString((yyvsp[-1].string_value));
			    char *res = new char[result.length() + 1];
			    strcpy(res, result.c_str());
			    (yyval.string_value) = res;
			    //cout<<"\nInferred type:"<<res;
			    //cout<<"Inferring the tuple along with some computation\n";
			}
#line 1712 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 394 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string computation;
			    computation = "%max(" + toString((yyvsp[-3].string_value)) + "," + toString((yyvsp[-1].string_value)) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computation - Max\n"<<res;
			}
#line 1725 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 403 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string computation;
			    computation = "%min(" + toString((yyvsp[-3].string_value)) + "," + toString((yyvsp[-1].string_value)) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computataion - Min\n"<<res;
			}
#line 1738 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 412 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string computation;	
			    computation = "%add(" + toString((yyvsp[-3].string_value));
			    computation = computation + "," + toString((yyvsp[-1].string_value)) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computataion - Min\n"<<res;
			}
#line 1752 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 422 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string computation;
			    computation = "%addall(";
			    computation += (toString((yyvsp[-1].string_value)) + ")");
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computation - Add all\n"<<res;
			}
#line 1766 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 433 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string res = "#";
			    res.append(toString((yyvsp[0].int_value)));
			    char *resCopy = new char[res.length() + 1];
			    strcpy(resCopy, res.c_str());
			//cout<<"Hash Int:"<<$2;
			    (yyval.string_value) = resCopy;
			}
#line 1779 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 442 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string str(toString((yyvsp[0].int_value)));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
			    (yyval.string_value) = res;
			}
#line 1790 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 449 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    (yyval.string_value) = (yyvsp[0].string_value);
			}
#line 1798 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 454 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    //Nothing to do
			    //cout<<"Recieved Comments Nothing to do\n";
			}
#line 1807 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 461 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    (yyval.int_value) = (yyvsp[0].int_value);
			    //cout<<"Integer Number\n"<<$1;
			}
#line 1816 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 466 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    (yyval.int_value) = INFINITY_WIDTH;
			   // //cout << "Infinity Recieved\n";
			}
#line 1825 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 471 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    (yyval.int_value) = VARIABLE_WIDTH; //For bitvector SIGNED_BITVECTOR[$k]
			}
#line 1833 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 476 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    //cout<<"Operator in YACC:"<<$1;
    				(yyval.string_value) = (yyvsp[0].string_value);
			}
#line 1842 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 483 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    //cout<<endl<<"----";
			    //cout<<endl<<toString($3)<<endl;
			    string str((yyvsp[-1].string_value));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
    			    (yyval.string_value) = res;
                            //cout<<"\n"<<res;
			}
#line 1856 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 495 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    //cout<<$1<<endl<<$2<<endl;
				string res((yyvsp[-1].string_value));
				res.append((yyvsp[0].string_value));
				char *result=new char[res.length()+1];
				strcpy(result,res.c_str());
				(yyval.string_value)=result;	
			}
#line 1869 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 504 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    (yyval.string_value) = (yyvsp[0].string_value);
			    //cout<<$1<<endl;
			}
#line 1878 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 510 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			   // //cout<<$1<<endl<<$2<<endl;
			    string str((yyvsp[-2].string_value));
			    str += (toString((yyvsp[-1].string_value)) + ";");
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
                            (yyval.string_value) = res;
			}
#line 1891 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 519 "inferencerule_parser.y" /* yacc.c:1646  */
    {
                            string str((yyvsp[-3].string_value));
                            str+= ("$printall"+toString((yyvsp[-1].string_value))+";");
                            char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
                            (yyval.string_value) = res;
                        }
#line 1903 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 529 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    string str((yyvsp[-1].string_value));
			    str += (" " + toString((yyvsp[0].string_value)));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
			    (yyval.string_value) = res;
                           //cout<<res<<endl;
			}
#line 1916 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 538 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    //char *res= new char[str.length()+1];
			    //strcpy(res,str.c_str());
			    //$$=res;
			    (yyval.string_value) = (yyvsp[0].string_value);
			    //cout<<$1<<endl;
			}
#line 1928 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 547 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    (yyval.string_value) = (yyvsp[0].string_value);
			    //cout<<$1<<endl;
			}
#line 1937 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 552 "inferencerule_parser.y" /* yacc.c:1646  */
    {
    				(yyval.string_value) = (yyvsp[0].string_value);
			    //cout<<$1<<endl;
			}
#line 1946 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 557 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    (yyval.string_value) = "$once";
			    //cout<<"Once"<<endl;
			}
#line 1955 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 562 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			    (yyval.string_value) = "$every";
			    //cout<<"Every"<<endl;
			}
#line 1964 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 568 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			 //cout<<"Type cast:"<<$2<<endl;
			 (yyval.string_value) = (yyvsp[0].string_value);
		       }
#line 1973 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 574 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			  string str(toString((yyvsp[-2].int_value)));
			  str += ("," + toString((yyvsp[0].string_value)));
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  (yyval.string_value) = res; 
			}
#line 1985 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 581 "inferencerule_parser.y" /* yacc.c:1646  */
    {
			  string str(toString((yyvsp[0].int_value)));
			  if( (yyvsp[0].int_value) == 0 )
			    {
			      str = "";
			    }			  
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  (yyval.string_value) = res;
			}
#line 2000 "inferencerule_parser.tab.c" /* yacc.c:1646  */
    break;


#line 2004 "inferencerule_parser.tab.c" /* yacc.c:1646  */
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
      yyerror (YY_("syntax error"));
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
        yyerror (yymsgp);
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
                      yytoken, &yylval);
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
                  yystos[yystate], yyvsp);
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
  yyerror (YY_("memory exhausted"));
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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 591 "inferencerule_parser.y" /* yacc.c:1906  */



void yyerror(char *s) 
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

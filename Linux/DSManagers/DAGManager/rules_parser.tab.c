/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         rules_parserparse
#define yylex           rules_parserlex
#define yyerror         rules_parsererror
#define yydebug         rules_parserdebug
#define yynerrs         rules_parsernerrs
#define yylval          rules_parserlval
#define yychar          rules_parserchar

/* First part of user prologue.  */
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

#line 102 "rules_parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "rules_parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_t_String = 3,                   /* t_String  */
  YYSYMBOL_t_Arrow = 4,                    /* t_Arrow  */
  YYSYMBOL_t_True = 5,                     /* t_True  */
  YYSYMBOL_t_False = 6,                    /* t_False  */
  YYSYMBOL_t_Constant = 7,                 /* t_Constant  */
  YYSYMBOL_t_Integer = 8,                  /* t_Integer  */
  YYSYMBOL_t_Variable = 9,                 /* t_Variable  */
  YYSYMBOL_t_OBrack = 10,                  /* t_OBrack  */
  YYSYMBOL_t_CBrack = 11,                  /* t_CBrack  */
  YYSYMBOL_t_Comma = 12,                   /* t_Comma  */
  YYSYMBOL_t_Eq = 13,                      /* t_Eq  */
  YYSYMBOL_t_BoolVariable = 14,            /* t_BoolVariable  */
  YYSYMBOL_YYACCEPT = 15,                  /* $accept  */
  YYSYMBOL_RulesFile = 16,                 /* RulesFile  */
  YYSYMBOL_RulesList = 17,                 /* RulesList  */
  YYSYMBOL_Rule = 18,                      /* Rule  */
  YYSYMBOL_nt_LHSExpr = 19,                /* nt_LHSExpr  */
  YYSYMBOL_LHSVariable = 20,               /* LHSVariable  */
  YYSYMBOL_LHSBoolVariable = 21,           /* LHSBoolVariable  */
  YYSYMBOL_LHSConstant = 22,               /* LHSConstant  */
  YYSYMBOL_nt_RHSExpr = 23,                /* nt_RHSExpr  */
  YYSYMBOL_RHSVariable = 24,               /* RHSVariable  */
  YYSYMBOL_RHSBoolVariable = 25,           /* RHSBoolVariable  */
  YYSYMBOL_RHSConstant = 26,               /* RHSConstant  */
  YYSYMBOL_nt_LHS_Arguments_List = 27,     /* nt_LHS_Arguments_List  */
  YYSYMBOL_nt_RHS_Arguments_List = 28,     /* nt_RHS_Arguments_List  */
  YYSYMBOL_Operator = 29                   /* Operator  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

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
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  42

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   269


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    53,    53,    63,    69,    74,    89,   107,   113,   119,
     126,   137,   151,   165,   177,   188,   200,   217,   226,   232,
     238,   245,   255,   269,   283,   291,   299,   310,   323,   334,
     346,   357,   373
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "t_String", "t_Arrow",
  "t_True", "t_False", "t_Constant", "t_Integer", "t_Variable", "t_OBrack",
  "t_CBrack", "t_Comma", "t_Eq", "t_BoolVariable", "$accept", "RulesFile",
  "RulesList", "Rule", "nt_LHSExpr", "LHSVariable", "LHSBoolVariable",
  "LHSConstant", "nt_RHSExpr", "RHSVariable", "RHSBoolVariable",
  "RHSConstant", "nt_LHS_Arguments_List", "nt_RHS_Arguments_List",
  "Operator", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-8)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      -8,     9,    -7,    -8,    17,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    16,    -8,    24,    -8,    -8,    -8,    -8,    17,
      27,    -8,    -3,    -8,    -8,    -8,    -8,    -8,    -8,    16,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    27,    -8,     7,
      -8,    -8
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       4,     0,     2,     1,     0,     3,    10,    13,    14,    15,
      16,    11,     0,    12,     0,     7,     9,     8,    32,     0,
       0,    29,     0,    21,    24,    25,    26,    27,    22,     0,
      23,     5,    18,    20,    19,     6,    28,     0,    31,     0,
      17,    30
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
      -8,    -8,    -8,    -8,    20,    -8,    -8,    -8,     1,    -8,
      -8,    -8,    -8,    -8,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,     5,    14,    15,    16,    17,    31,    32,
      33,    34,    22,    39,    19
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
       6,     4,     7,     8,     9,    10,    11,    12,    35,     3,
      23,    13,    24,    25,    26,    27,    28,    29,    40,    18,
       6,    30,     7,     8,     9,    10,    11,    12,    20,    37,
      23,    13,    24,    25,    26,    27,    28,    29,    38,    21,
      41,    30,    36
};

static const yytype_int8 yycheck[] =
{
       3,     8,     5,     6,     7,     8,     9,    10,    11,     0,
       3,    14,     5,     6,     7,     8,     9,    10,    11,     3,
       3,    14,     5,     6,     7,     8,     9,    10,     4,    29,
       3,    14,     5,     6,     7,     8,     9,    10,    37,    19,
      39,    14,    22
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    16,    17,     0,     8,    18,     3,     5,     6,     7,
       8,     9,    10,    14,    19,    20,    21,    22,     3,    29,
       4,    19,    27,     3,     5,     6,     7,     8,     9,    10,
      14,    23,    24,    25,    26,    11,    19,    29,    23,    28,
      11,    23
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    15,    16,    17,    17,    18,    19,    19,    19,    19,
      19,    20,    21,    22,    22,    22,    22,    23,    23,    23,
      23,    23,    24,    25,    26,    26,    26,    26,    27,    27,
      28,    28,    29
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     4,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
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
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
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
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
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
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
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
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
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
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  case 2: /* RulesFile: RulesList  */
#line 54 "rules_parser.y"
         {
	   //cout<<"Read the RulesFile successfully\n";
	   const string sRewriteRulesFile = 
	       t_ConfigurationOptions::getConfig()->m_sTempInfoDir + "/DAGManager/RewriteRulesRead";
	   ofstream *outfile = new ofstream();
	   outfile->open(sRewriteRulesFile.c_str());
	   ds->printAllRewriteRules(outfile);
	   outfile->close();
	 }
#line 1419 "rules_parser.tab.c"
    break;

  case 3: /* RulesList: RulesList Rule  */
#line 64 "rules_parser.y"
          {
	    //cout<<"Matched RulesList \n";
	  }
#line 1427 "rules_parser.tab.c"
    break;

  case 4: /* RulesList: %empty  */
#line 69 "rules_parser.y"
          {
	    //cout<<"Matched RulesList with 0 rules\n";
	  }
#line 1435 "rules_parser.tab.c"
    break;

  case 5: /* Rule: t_Integer nt_LHSExpr t_Arrow nt_RHSExpr  */
#line 75 "rules_parser.y"
         {
	   //cout<<"----------------------------------------\n";
	   //cout<<"Read an rule successfully\n";
	   t_RewriteRule * rule = new t_RewriteRule((yyvsp[-2].expression_type), (yyvsp[0].expression_type), stringToInteger(toString((yyvsp[-3].string_value))));
	   ds->addARewriteRule(rule);

	   t_RewriteRuleExpression *lhs_expr = (yyvsp[-2].expression_type);
	   ds->addValidStringOfLHSOfARule(lhs_expr->getValidSubstringOfRewriteRuleExpression());
	   (yyval.rule_type) = rule;
         }
#line 1450 "rules_parser.tab.c"
    break;

  case 6: /* nt_LHSExpr: t_OBrack Operator nt_LHS_Arguments_List t_CBrack  */
#line 90 "rules_parser.y"
        {
	  //cout<<"Creating an nt_Expr with operator and arguments\n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions((yyvsp[-1].vector_of_exprs_type));
	  string valid_substring = (yyvsp[-2].string_value);
	  for(int i=0; i<arguments.size();i++)
	    {
	      valid_substring += ",";
	      valid_substring+=arguments[i]->getValidSubstringOfRewriteRuleExpression();
	    }
	  //cout<<"Adding the VSS "<<valid_substring<<endl;
	  ds->addValidSubstring(valid_substring);
	  
	  t_RewriteRuleExpression *expression = new t_RewriteRuleExpression((yyvsp[-2].string_value), other, arguments, 0,valid_substring);
	  //cout<<"Created an nt_Expr\n";
	  (yyval.expression_type) = expression;
	}
#line 1471 "rules_parser.tab.c"
    break;

  case 7: /* nt_LHSExpr: LHSVariable  */
#line 108 "rules_parser.y"
        {
	  //cout<<"Creating an nt_Expr with Variable\n";
	  (yyval.expression_type) = (yyvsp[0].expression_type);
	}
#line 1480 "rules_parser.tab.c"
    break;

  case 8: /* nt_LHSExpr: LHSConstant  */
#line 114 "rules_parser.y"
        {
	  //cout<<"Creating an nt_Expr with Constant\n";
	  (yyval.expression_type) = (yyvsp[0].expression_type);
	}
#line 1489 "rules_parser.tab.c"
    break;

  case 9: /* nt_LHSExpr: LHSBoolVariable  */
#line 120 "rules_parser.y"
        {
	  //cout<<"Creating an nt_Expr with Bool Variable\n";
	  //cout<<"Created and VariableRewriteRuleExpression\n";
	  (yyval.expression_type) = (yyvsp[0].expression_type);
	}
#line 1499 "rules_parser.tab.c"
    break;

  case 10: /* nt_LHSExpr: t_String  */
#line 127 "rules_parser.y"
        {
	string variable_name = (yyvsp[0].string_value);
	   int variable_index = 0;
	   ds->addValidSubstring((yyvsp[0].string_value));
	   //cout<<"Adding the VSS "<<"$1"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[0].string_value), placeholder,variable_index, (yyvsp[0].string_value));
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;

	}
#line 1514 "rules_parser.tab.c"
    break;

  case 11: /* LHSVariable: t_Variable  */
#line 138 "rules_parser.y"
         {
	   //these are basically e_1 e_2 ...
	   string variable_name = (yyvsp[0].string_value);
	   //string var_to_create = "e";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   ds->addValidSubstring("e");
	   //cout<<"Adding the VSS e"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[0].string_value), placeholder, variable_index, "e");
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;
	   
          }
#line 1531 "rules_parser.tab.c"
    break;

  case 12: /* LHSBoolVariable: t_BoolVariable  */
#line 152 "rules_parser.y"
         {
	   //These are basically b_0 b_1 b_2 ...
	   string variable_name = (yyvsp[0].string_value);
	   //string var_to_create = "b";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   ds->addValidSubstring("b");
	   //cout<<"Adding the VSS "<<"b"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[0].string_value), placeholder,variable_index, "b");
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;
         }
#line 1547 "rules_parser.tab.c"
    break;

  case 13: /* LHSConstant: t_True  */
#line 166 "rules_parser.y"
        {
	  //this is single constant true
	  t_RewriteRuleExpression *true_expr = new t_RewriteRuleExpression((yyvsp[0].string_value), fixed_string,0, "true");
	  string valid_substring = "true";
	  //cout<<"Adding the VSS "<<valid_substring<<endl;
	  ds->addValidSubstring(valid_substring);

	  //cout<<"Creating a constant true\n";
	  (yyval.expression_type) = true_expr;
	}
#line 1562 "rules_parser.tab.c"
    break;

  case 14: /* LHSConstant: t_False  */
#line 178 "rules_parser.y"
       {
	  //this is single constant false
	 t_RewriteRuleExpression *false_expr = new t_RewriteRuleExpression((yyvsp[0].string_value), fixed_string,0,"false");
	 string valid_substring = "false";
	 //cout<<"Adding the VSS "<<valid_substring<<endl;
	 ds->addValidSubstring(valid_substring);
	 //cout<<"Creating a constant false\n";
	 (yyval.expression_type) = false_expr;
       }
#line 1576 "rules_parser.tab.c"
    break;

  case 15: /* LHSConstant: t_Constant  */
#line 189 "rules_parser.y"
        {
	  //these are basically constants like c_1 c_2 c_3....
	  string variable_name = (yyvsp[0].string_value);
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   ds->addValidSubstring("c");
	   //cout<<"Adding the VSS c\n";
	   t_RewriteRuleExpression *const_expr = new t_RewriteRuleExpression((yyvsp[0].string_value), placeholder,variable_index,"c");
	   //cout<<"Creating a constant false\n";
	   (yyval.expression_type) = const_expr;
	}
#line 1591 "rules_parser.tab.c"
    break;

  case 16: /* LHSConstant: t_Integer  */
#line 201 "rules_parser.y"
       {
	 ds->addValidSubstring((yyvsp[0].string_value));
	 //cout<<"Adding the VSS  "<<$1<<endl;
	 t_RewriteRuleExpression *integer_expr = new t_RewriteRuleExpression((yyvsp[0].string_value), fixed_string,0,(yyvsp[0].string_value));
	 string valid_substring = (yyvsp[0].string_value);
	 //cout<<"Adding the VSS "<<valid_substring<<endl;
	 ds->addValidSubstring(valid_substring);
	 //cout<<"Creating an integer \n";
	 (yyval.expression_type) = integer_expr;
       }
#line 1606 "rules_parser.tab.c"
    break;

  case 17: /* nt_RHSExpr: t_OBrack Operator nt_RHS_Arguments_List t_CBrack  */
#line 218 "rules_parser.y"
        {
	  //cout<<"Creating an nt_Expr with operator and arguments\n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions((yyvsp[-1].vector_of_exprs_type));
	  t_RewriteRuleExpression *expression = new t_RewriteRuleExpression((yyvsp[-2].string_value), other, arguments, 0,"");
	  //cout<<"Created an nt_Expr\n";
	  (yyval.expression_type) = expression;
	}
#line 1618 "rules_parser.tab.c"
    break;

  case 18: /* nt_RHSExpr: RHSVariable  */
#line 227 "rules_parser.y"
        {
	  //cout<<"Creating an nt_Expr with Variable\n";
	  (yyval.expression_type) = (yyvsp[0].expression_type);
	}
#line 1627 "rules_parser.tab.c"
    break;

  case 19: /* nt_RHSExpr: RHSConstant  */
#line 233 "rules_parser.y"
        {
	  //cout<<"Creating an nt_Expr with Constant\n";
	  (yyval.expression_type) = (yyvsp[0].expression_type);
	}
#line 1636 "rules_parser.tab.c"
    break;

  case 20: /* nt_RHSExpr: RHSBoolVariable  */
#line 239 "rules_parser.y"
        {
	  //cout<<"Creating an nt_Expr with Bool Variable\n";
	  //cout<<"Created and VariableRewriteRuleExpression\n";
	  (yyval.expression_type) = (yyvsp[0].expression_type);
	}
#line 1646 "rules_parser.tab.c"
    break;

  case 21: /* nt_RHSExpr: t_String  */
#line 246 "rules_parser.y"
        {
	string variable_name = (yyvsp[0].string_value);
	   int variable_index = 0;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[0].string_value), placeholder,variable_index, (yyvsp[0].string_value));
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;

	}
#line 1659 "rules_parser.tab.c"
    break;

  case 22: /* RHSVariable: t_Variable  */
#line 256 "rules_parser.y"
         {
	   //these are basically e_1 e_2 ...
	   string variable_name = (yyvsp[0].string_value);
	   //string var_to_create = "e";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   //ds->addValidSubstring("e");
	   ////cout<<"Adding the VSS e"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[0].string_value), placeholder, variable_index, "e");
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;
	   
          }
#line 1676 "rules_parser.tab.c"
    break;

  case 23: /* RHSBoolVariable: t_BoolVariable  */
#line 270 "rules_parser.y"
         {
	   //These are basically b_0 b_1 b_2 ...
	   string variable_name = (yyvsp[0].string_value);
	   //string var_to_create = "b";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   //ds->addValidSubstring("b");
	   ////cout<<"Adding the VSS "<<"b"<<endl;
	   t_RewriteRuleExpression *expr = new t_RewriteRuleExpression((yyvsp[0].string_value), placeholder,variable_index, "b");
	   //cout<<"Creating a expression\n";
	   (yyval.expression_type) = expr;
         }
#line 1692 "rules_parser.tab.c"
    break;

  case 24: /* RHSConstant: t_True  */
#line 284 "rules_parser.y"
        {
	  //this is single constant true
	  t_RewriteRuleExpression *true_expr = new t_RewriteRuleExpression((yyvsp[0].string_value), fixed_string,0, "true");
	  //cout<<"Creating a constant true\n";
	  (yyval.expression_type) = true_expr;
	}
#line 1703 "rules_parser.tab.c"
    break;

  case 25: /* RHSConstant: t_False  */
#line 292 "rules_parser.y"
       {
	  //this is single constant false
	 t_RewriteRuleExpression *false_expr = new t_RewriteRuleExpression((yyvsp[0].string_value), placeholder,0,"false");
	 //cout<<"Creating a constant false\n";
	 (yyval.expression_type) = false_expr;
       }
#line 1714 "rules_parser.tab.c"
    break;

  case 26: /* RHSConstant: t_Constant  */
#line 300 "rules_parser.y"
        {
	  //these are basically constants like c_1 c_2 c_3....
	  string variable_name = (yyvsp[0].string_value);
	   //string var_to_create = "c";
	   int variable_index = stringToInteger(variable_name.substr(2,variable_name.size()-2));
	   t_RewriteRuleExpression *const_expr = new t_RewriteRuleExpression((yyvsp[0].string_value), placeholder,variable_index,"c");
	   //cout<<"Creating a constant false\n";
	   (yyval.expression_type) = const_expr;
	}
#line 1728 "rules_parser.tab.c"
    break;

  case 27: /* RHSConstant: t_Integer  */
#line 311 "rules_parser.y"
       {
	 t_RewriteRuleExpression * integer_expr;
	 integer_expr = new t_RewriteRuleExpression((yyvsp[0].string_value), fixed_string,0,(yyvsp[0].string_value));
	 //cout<<"Creating an integer \n";
	 (yyval.expression_type) = integer_expr;
       /*Operand_RewriteRuleExpressions_List is the list of (comma seperated) optional operands of an operator. Do not confuse with Expr_Vector*/
       }
#line 1740 "rules_parser.tab.c"
    break;

  case 28: /* nt_LHS_Arguments_List: nt_LHS_Arguments_List nt_LHSExpr  */
#line 324 "rules_parser.y"
        {
	  //cout<<"Creating vector of arguments \n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions((yyvsp[-1].vector_of_exprs_type));
	  t_RewriteRuleExpression *arg_to_add = (yyvsp[0].expression_type);
	  arguments.push_back(arg_to_add);
	  t_VectorOfRewriteRuleExpressions *vector_of_expressions = new t_VectorOfRewriteRuleExpressions(arguments);
	  //cout<<"Created vector of arguments\n";
	  (yyval.vector_of_exprs_type) = vector_of_expressions;
	}
#line 1754 "rules_parser.tab.c"
    break;

  case 29: /* nt_LHS_Arguments_List: nt_LHSExpr  */
#line 335 "rules_parser.y"
          {
	    //cout<<"Creating vector of arguments with only one argument\n";
	    vector<t_RewriteRuleExpression *> arguments;
	    arguments.push_back((yyvsp[0].expression_type));
	    t_VectorOfRewriteRuleExpressions *vector_of_expressions
	      = new t_VectorOfRewriteRuleExpressions(arguments);
	    
	    //cout<<"Created vector of arguments with only one argument\n";
	    (yyval.vector_of_exprs_type) = vector_of_expressions;
	  }
#line 1769 "rules_parser.tab.c"
    break;

  case 30: /* nt_RHS_Arguments_List: nt_RHS_Arguments_List nt_RHSExpr  */
#line 347 "rules_parser.y"
        {
	  //cout<<"Creating vector of arguments \n";
	  vector<t_RewriteRuleExpression *> arguments = assign_arguments_of_simplification_expressions((yyvsp[-1].vector_of_exprs_type));
	  t_RewriteRuleExpression *arg_to_add = (yyvsp[0].expression_type);
	  arguments.push_back(arg_to_add);
	  t_VectorOfRewriteRuleExpressions *vector_of_expressions = new t_VectorOfRewriteRuleExpressions(arguments);
	  //cout<<"Created vector of arguments\n";
	  (yyval.vector_of_exprs_type) = vector_of_expressions;
	}
#line 1783 "rules_parser.tab.c"
    break;

  case 31: /* nt_RHS_Arguments_List: nt_RHSExpr  */
#line 358 "rules_parser.y"
          {
	    //cout<<"Creating vector of arguments with only one argument\n";
	    vector<t_RewriteRuleExpression *> arguments;
	    arguments.push_back((yyvsp[0].expression_type));
	    t_VectorOfRewriteRuleExpressions *vector_of_expressions = new t_VectorOfRewriteRuleExpressions(arguments);
	    //cout<<"Created vector of arguments with only one argument\n";
	    (yyval.vector_of_exprs_type) = vector_of_expressions;
	  }
#line 1796 "rules_parser.tab.c"
    break;

  case 32: /* Operator: t_String  */
#line 374 "rules_parser.y"
           { //cout<<"Matching an operator "<<$1<<endl;  
	     (yyval.string_value)=(yyvsp[0].string_value);
	   }
#line 1804 "rules_parser.tab.c"
    break;


#line 1808 "rules_parser.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 377 "rules_parser.y"







void yyerror(char const *s) 
{
  fprintf(stderr, "%s\n", s);
  fprintf(stderr, "ERROR!! In Parsing the simplification rules file \n");
  abort();
}



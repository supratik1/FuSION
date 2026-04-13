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
#define yyparse         inferencerule_parserparse
#define yylex           inferencerule_parserlex
#define yyerror         inferencerule_parsererror
#define yydebug         inferencerule_parserdebug
#define yynerrs         inferencerule_parsernerrs
#define yylval          inferencerule_parserlval
#define yychar          inferencerule_parserchar

/* First part of user prologue.  */
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
    

#line 133 "inferencerule_parser.tab.c"

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

#include "inferencerule_parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_t_String = 3,                   /* t_String  */
  YYSYMBOL_t_StringLiteral = 4,            /* t_StringLiteral  */
  YYSYMBOL_t_TypeValue = 5,                /* t_TypeValue  */
  YYSYMBOL_t_OBrack = 6,                   /* t_OBrack  */
  YYSYMBOL_t_CBrack = 7,                   /* t_CBrack  */
  YYSYMBOL_t_Comma = 8,                    /* t_Comma  */
  YYSYMBOL_t_Eq = 9,                       /* t_Eq  */
  YYSYMBOL_t_OSquareBrack = 10,            /* t_OSquareBrack  */
  YYSYMBOL_t_CSquareBrack = 11,            /* t_CSquareBrack  */
  YYSYMBOL_t_OperatorString = 12,          /* t_OperatorString  */
  YYSYMBOL_t_MinArgs = 13,                 /* t_MinArgs  */
  YYSYMBOL_t_MaxArgs = 14,                 /* t_MaxArgs  */
  YYSYMBOL_t_Operand1 = 15,                /* t_Operand1  */
  YYSYMBOL_t_Operand2 = 16,                /* t_Operand2  */
  YYSYMBOL_t_Operand3 = 17,                /* t_Operand3  */
  YYSYMBOL_t_OtherOperandsString = 18,     /* t_OtherOperandsString  */
  YYSYMBOL_t_InferString = 19,             /* t_InferString  */
  YYSYMBOL_t_InfinityString = 20,          /* t_InfinityString  */
  YYSYMBOL_t_Hash = 21,                    /* t_Hash  */
  YYSYMBOL_t_Comments = 22,                /* t_Comments  */
  YYSYMBOL_t_Add = 23,                     /* t_Add  */
  YYSYMBOL_t_Max = 24,                     /* t_Max  */
  YYSYMBOL_t_Min = 25,                     /* t_Min  */
  YYSYMBOL_t_Addall = 26,                  /* t_Addall  */
  YYSYMBOL_INT_NUMBER = 27,                /* INT_NUMBER  */
  YYSYMBOL_t_ArgumentNumber = 28,          /* t_ArgumentNumber  */
  YYSYMBOL_t_Variable = 29,                /* t_Variable  */
  YYSYMBOL_t_print = 30,                   /* t_print  */
  YYSYMBOL_t_FCBrack = 31,                 /* t_FCBrack  */
  YYSYMBOL_t_FOBrack = 32,                 /* t_FOBrack  */
  YYSYMBOL_t_Once = 33,                    /* t_Once  */
  YYSYMBOL_t_Every = 34,                   /* t_Every  */
  YYSYMBOL_t_SemiColon = 35,               /* t_SemiColon  */
  YYSYMBOL_t_KeyWordsOfPrint = 36,         /* t_KeyWordsOfPrint  */
  YYSYMBOL_t_printAll = 37,                /* t_printAll  */
  YYSYMBOL_t_CastString = 38,              /* t_CastString  */
  YYSYMBOL_YYACCEPT = 39,                  /* $accept  */
  YYSYMBOL_ListOfInferenceRules = 40,      /* ListOfInferenceRules  */
  YYSYMBOL_InferenceRule = 41,             /* InferenceRule  */
  YYSYMBOL_TypeTuple = 42,                 /* TypeTuple  */
  YYSYMBOL_InferredTypeTuple = 43,         /* InferredTypeTuple  */
  YYSYMBOL_Computation = 44,               /* Computation  */
  YYSYMBOL_ArgumentForComputation = 45,    /* ArgumentForComputation  */
  YYSYMBOL_Comments = 46,                  /* Comments  */
  YYSYMBOL_Integer = 47,                   /* Integer  */
  YYSYMBOL_Operator = 48,                  /* Operator  */
  YYSYMBOL_PrintString = 49,               /* PrintString  */
  YYSYMBOL_ParameterOfPrintBlock = 50,     /* ParameterOfPrintBlock  */
  YYSYMBOL_ParameterOfPrintStatement = 51, /* ParameterOfPrintStatement  */
  YYSYMBOL_ParameterOfPrintWordList = 52,  /* ParameterOfPrintWordList  */
  YYSYMBOL_ParameterOfPrintWord = 53,      /* ParameterOfPrintWord  */
  YYSYMBOL_OnceOrEvery = 54,               /* OnceOrEvery  */
  YYSYMBOL_TypeCastOperands = 55,          /* TypeCastOperands  */
  YYSYMBOL_OperandsNumbers = 56            /* OperandsNumbers  */
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

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

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

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   293


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
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    97,    97,   101,   106,   184,   244,   282,   325,   354,
     359,   370,   382,   388,   401,   410,   419,   429,   440,   449,
     456,   461,   466,   468,   473,   478,   483,   490,   502,   511,
     517,   526,   536,   545,   554,   559,   564,   573,   583,   589,
     596
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "t_String",
  "t_StringLiteral", "t_TypeValue", "t_OBrack", "t_CBrack", "t_Comma",
  "t_Eq", "t_OSquareBrack", "t_CSquareBrack", "t_OperatorString",
  "t_MinArgs", "t_MaxArgs", "t_Operand1", "t_Operand2", "t_Operand3",
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

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-50)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
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
static const yytype_int8 yydefact[] =
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
       0,     3,     4,    27,    28,    70,    71,     5,    72,     7,
      36,    60,    61,    81,    82,    62,    48,    64
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
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

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
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

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    39,    40,    40,    41,    41,    41,    41,    41,    41,
      42,    42,    43,    43,    44,    44,    44,    44,    45,    45,
      45,    46,    46,    47,    47,    47,    48,    49,    50,    50,
      51,    51,    52,    52,    53,    53,    54,    54,    55,    56,
      56
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,    18,    16,    16,    14,    12,     1,
       4,     5,     1,     4,     6,     6,     6,     4,     2,     1,
       1,     1,     0,     1,     1,     1,     1,     4,     2,     1,
       3,     4,     2,     1,     1,     1,     1,     1,     2,     3,
       1
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
  case 2: /* ListOfInferenceRules: InferenceRule  */
#line 98 "inferencerule_parser.y"
                        {
			  ////cout << "Inference rule is there\n";
			}
#line 1244 "inferencerule_parser.tab.c"
    break;

  case 3: /* ListOfInferenceRules: InferenceRule ListOfInferenceRules  */
#line 102 "inferencerule_parser.y"
                         {
			   ////cout << "Inference Rule\n";
			 }
#line 1252 "inferencerule_parser.tab.c"
    break;

  case 4: /* InferenceRule: t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_Operand2 TypeTuple t_Operand3 TypeTuple t_OtherOperandsString TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands  */
#line 107 "inferencerule_parser.y"
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
#line 1334 "inferencerule_parser.tab.c"
    break;

  case 5: /* InferenceRule: t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_Operand2 TypeTuple t_OtherOperandsString TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands  */
#line 185 "inferencerule_parser.y"
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
#line 1398 "inferencerule_parser.tab.c"
    break;

  case 6: /* InferenceRule: t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_Operand2 TypeTuple t_Operand3 TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands  */
#line 245 "inferencerule_parser.y"
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
#line 1440 "inferencerule_parser.tab.c"
    break;

  case 7: /* InferenceRule: t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_Operand2 TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands  */
#line 283 "inferencerule_parser.y"
                        {
			    bool variableNumberOfArguments = ((yyvsp[-10].int_value) != (yyvsp[-8].int_value)) ? true : false;
			    bool unlimitedNumberOfArguments = ((yyvsp[-8].int_value) == INFINITY_WIDTH) ? true : false;
			    for (int i = (yyvsp[-10].int_value); i <= (yyvsp[-8].int_value); i++) 
			    {
        			if ((yyvsp[-8].int_value) != 2) 
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
#line 1487 "inferencerule_parser.tab.c"
    break;

  case 8: /* InferenceRule: t_OperatorString Operator t_MinArgs Integer t_MaxArgs Integer t_Operand1 TypeTuple t_InferString InferredTypeTuple PrintString TypeCastOperands  */
#line 326 "inferencerule_parser.y"
                        {
			    string signature;
			    if ((yyvsp[-6].int_value) != (yyvsp[-8].int_value) || (yyvsp[-8].int_value) != 1) 
			    {
			      string str;
			      str = "Error while parsing file (Minargs != Maxargs or Maxargs  not 1)";
			      char * msg = new char[str.length() + 1];
			      strcpy(msg, str.c_str());
			      yyerror(msg);
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
#line 1520 "inferencerule_parser.tab.c"
    break;

  case 9: /* InferenceRule: Comments  */
#line 355 "inferencerule_parser.y"
                         {
			    //cout<<"Comments Accepted"<<endl;
			 }
#line 1528 "inferencerule_parser.tab.c"
    break;

  case 10: /* TypeTuple: t_TypeValue t_OSquareBrack Integer t_CSquareBrack  */
#line 360 "inferencerule_parser.y"
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
#line 1543 "inferencerule_parser.tab.c"
    break;

  case 11: /* TypeTuple: t_TypeValue t_OSquareBrack t_Hash Integer t_CSquareBrack  */
#line 371 "inferencerule_parser.y"
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
#line 1558 "inferencerule_parser.tab.c"
    break;

  case 12: /* InferredTypeTuple: TypeTuple  */
#line 383 "inferencerule_parser.y"
                        {
			    //cout << "Inferring the type directly according to operand\n";
			    //cout << $1;
			    (yyval.string_value) = (yyvsp[0].string_value);
			}
#line 1568 "inferencerule_parser.tab.c"
    break;

  case 13: /* InferredTypeTuple: t_TypeValue t_OSquareBrack Computation t_CSquareBrack  */
#line 389 "inferencerule_parser.y"
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
#line 1584 "inferencerule_parser.tab.c"
    break;

  case 14: /* Computation: t_Max t_OBrack ArgumentForComputation t_Comma ArgumentForComputation t_CBrack  */
#line 402 "inferencerule_parser.y"
                        {
			    string computation;
			    computation = "%max(" + toString((yyvsp[-3].string_value)) + "," + toString((yyvsp[-1].string_value)) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computation - Max\n"<<res;
			}
#line 1597 "inferencerule_parser.tab.c"
    break;

  case 15: /* Computation: t_Min t_OBrack ArgumentForComputation t_Comma ArgumentForComputation t_CBrack  */
#line 411 "inferencerule_parser.y"
                        {
			    string computation;
			    computation = "%min(" + toString((yyvsp[-3].string_value)) + "," + toString((yyvsp[-1].string_value)) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computataion - Min\n"<<res;
			}
#line 1610 "inferencerule_parser.tab.c"
    break;

  case 16: /* Computation: t_Add t_OBrack ArgumentForComputation t_Comma ArgumentForComputation t_CBrack  */
#line 420 "inferencerule_parser.y"
                        {
			    string computation;	
			    computation = "%add(" + toString((yyvsp[-3].string_value));
			    computation = computation + "," + toString((yyvsp[-1].string_value)) + ")";
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computataion - Min\n"<<res;
			}
#line 1624 "inferencerule_parser.tab.c"
    break;

  case 17: /* Computation: t_Addall t_OBrack ArgumentForComputation t_CBrack  */
#line 430 "inferencerule_parser.y"
                        {
			    string computation;
			    computation = "%addall(";
			    computation += (toString((yyvsp[-1].string_value)) + ")");
			    char *res = new char[computation.length() + 1];
			    strcpy(res, computation.c_str());
			    (yyval.string_value) = res;
			    //cout<<"Computation - Add all\n"<<res;
			}
#line 1638 "inferencerule_parser.tab.c"
    break;

  case 18: /* ArgumentForComputation: t_Hash Integer  */
#line 441 "inferencerule_parser.y"
                        {
			    string res = "#";
			    res.append(toString((yyvsp[0].int_value)));
			    char *resCopy = new char[res.length() + 1];
			    strcpy(resCopy, res.c_str());
			//cout<<"Hash Int:"<<$2;
			    (yyval.string_value) = resCopy;
			}
#line 1651 "inferencerule_parser.tab.c"
    break;

  case 19: /* ArgumentForComputation: Integer  */
#line 450 "inferencerule_parser.y"
                        {
			    string str(toString((yyvsp[0].int_value)));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
			    (yyval.string_value) = res;
			}
#line 1662 "inferencerule_parser.tab.c"
    break;

  case 20: /* ArgumentForComputation: Computation  */
#line 457 "inferencerule_parser.y"
                        {
			    (yyval.string_value) = (yyvsp[0].string_value);
			}
#line 1670 "inferencerule_parser.tab.c"
    break;

  case 21: /* Comments: t_Comments  */
#line 462 "inferencerule_parser.y"
                        {
			    //Nothing to do
			    //cout<<"Recieved Comments Nothing to do\n";
			}
#line 1679 "inferencerule_parser.tab.c"
    break;

  case 23: /* Integer: INT_NUMBER  */
#line 469 "inferencerule_parser.y"
                        {
			    (yyval.int_value) = (yyvsp[0].int_value);
			    //cout<<"Integer Number\n"<<$1;
			}
#line 1688 "inferencerule_parser.tab.c"
    break;

  case 24: /* Integer: t_InfinityString  */
#line 474 "inferencerule_parser.y"
                        {
			    (yyval.int_value) = INFINITY_WIDTH;
			   // //cout << "Infinity Recieved\n";
			}
#line 1697 "inferencerule_parser.tab.c"
    break;

  case 25: /* Integer: t_Variable  */
#line 479 "inferencerule_parser.y"
                        {
			    (yyval.int_value) = VARIABLE_WIDTH; //For bitvector SIGNED_BITVECTOR[$k]
			}
#line 1705 "inferencerule_parser.tab.c"
    break;

  case 26: /* Operator: t_String  */
#line 484 "inferencerule_parser.y"
                        {
			    //cout<<"Operator in YACC:"<<$1;
    				(yyval.string_value) = (yyvsp[0].string_value);
			}
#line 1714 "inferencerule_parser.tab.c"
    break;

  case 27: /* PrintString: t_print t_FOBrack ParameterOfPrintBlock t_FCBrack  */
#line 491 "inferencerule_parser.y"
                        {
			    //cout<<endl<<"----";
			    //cout<<endl<<toString($3)<<endl;
			    string str((yyvsp[-1].string_value));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
    			    (yyval.string_value) = res;
                            //cout<<"\n"<<res;
			}
#line 1728 "inferencerule_parser.tab.c"
    break;

  case 28: /* ParameterOfPrintBlock: ParameterOfPrintStatement ParameterOfPrintBlock  */
#line 503 "inferencerule_parser.y"
                        {
			    //cout<<$1<<endl<<$2<<endl;
				string res((yyvsp[-1].string_value));
				res.append((yyvsp[0].string_value));
				char *result=new char[res.length()+1];
				strcpy(result,res.c_str());
				(yyval.string_value)=result;	
			}
#line 1741 "inferencerule_parser.tab.c"
    break;

  case 29: /* ParameterOfPrintBlock: ParameterOfPrintStatement  */
#line 512 "inferencerule_parser.y"
                        {
			    (yyval.string_value) = (yyvsp[0].string_value);
			    //cout<<$1<<endl;
			}
#line 1750 "inferencerule_parser.tab.c"
    break;

  case 30: /* ParameterOfPrintStatement: OnceOrEvery ParameterOfPrintWordList t_SemiColon  */
#line 518 "inferencerule_parser.y"
                        {
			   // //cout<<$1<<endl<<$2<<endl;
			    string str((yyvsp[-2].string_value));
			    str += (toString((yyvsp[-1].string_value)) + ";");
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
                            (yyval.string_value) = res;
			}
#line 1763 "inferencerule_parser.tab.c"
    break;

  case 31: /* ParameterOfPrintStatement: OnceOrEvery t_printAll ParameterOfPrintWordList t_SemiColon  */
#line 527 "inferencerule_parser.y"
                        {
                            string str((yyvsp[-3].string_value));
                            str+= ("$printall"+toString((yyvsp[-1].string_value))+";");
                            char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
                            (yyval.string_value) = res;
                        }
#line 1775 "inferencerule_parser.tab.c"
    break;

  case 32: /* ParameterOfPrintWordList: ParameterOfPrintWord ParameterOfPrintWordList  */
#line 537 "inferencerule_parser.y"
                        {
			    string str((yyvsp[-1].string_value));
			    str += (" " + toString((yyvsp[0].string_value)));
			    char *res = new char[str.length() + 1];
			    strcpy(res, str.c_str());
			    (yyval.string_value) = res;
                           //cout<<res<<endl;
			}
#line 1788 "inferencerule_parser.tab.c"
    break;

  case 33: /* ParameterOfPrintWordList: ParameterOfPrintWord  */
#line 546 "inferencerule_parser.y"
                        {
			    //char *res= new char[str.length()+1];
			    //strcpy(res,str.c_str());
			    //$$=res;
			    (yyval.string_value) = (yyvsp[0].string_value);
			    //cout<<$1<<endl;
			}
#line 1800 "inferencerule_parser.tab.c"
    break;

  case 34: /* ParameterOfPrintWord: t_StringLiteral  */
#line 555 "inferencerule_parser.y"
                        {
			    (yyval.string_value) = (yyvsp[0].string_value);
			    //cout<<$1<<endl;
			}
#line 1809 "inferencerule_parser.tab.c"
    break;

  case 35: /* ParameterOfPrintWord: t_KeyWordsOfPrint  */
#line 560 "inferencerule_parser.y"
                        {
    				(yyval.string_value) = (yyvsp[0].string_value);
			    //cout<<$1<<endl;
			}
#line 1818 "inferencerule_parser.tab.c"
    break;

  case 36: /* OnceOrEvery: t_Once  */
#line 565 "inferencerule_parser.y"
                        {
			  string str("$once");
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  (yyval.string_value) = res;
			  // $$ = "$once";
			  //cout<<"Once"<<endl;
			}
#line 1831 "inferencerule_parser.tab.c"
    break;

  case 37: /* OnceOrEvery: t_Every  */
#line 574 "inferencerule_parser.y"
                        {
			  string str("$every");
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  (yyval.string_value) = res;
			  // $$ = "$every";
			  //cout<<"Every"<<endl;
			}
#line 1844 "inferencerule_parser.tab.c"
    break;

  case 38: /* TypeCastOperands: t_CastString OperandsNumbers  */
#line 584 "inferencerule_parser.y"
                       {
			 //cout<<"Type cast:"<<$2<<endl;
			 (yyval.string_value) = (yyvsp[0].string_value);
		       }
#line 1853 "inferencerule_parser.tab.c"
    break;

  case 39: /* OperandsNumbers: INT_NUMBER t_Comma OperandsNumbers  */
#line 590 "inferencerule_parser.y"
                        {
			  string str(toString((yyvsp[-2].int_value)));
			  str += ("," + toString((yyvsp[0].string_value)));
			  char *res = new char[str.length() + 1];
			  strcpy(res, str.c_str());
			  (yyval.string_value) = res; 
			}
#line 1865 "inferencerule_parser.tab.c"
    break;

  case 40: /* OperandsNumbers: INT_NUMBER  */
#line 597 "inferencerule_parser.y"
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
#line 1880 "inferencerule_parser.tab.c"
    break;


#line 1884 "inferencerule_parser.tab.c"

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
      yyerror (YY_("syntax error"));
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

  return yyresult;
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

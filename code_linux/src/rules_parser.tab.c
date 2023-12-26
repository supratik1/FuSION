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




/* First part of user prologue.  */
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
void yyerror(const char *);
//to be commented ends

list<rule_t*> list_of_rules;

#line 92 "src/rules_parser.tab.c"

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
  YYSYMBOL_BEGIN_RULE = 3,                 /* BEGIN_RULE  */
  YYSYMBOL_END_RULE = 4,                   /* END_RULE  */
  YYSYMBOL_COLON = 5,                      /* COLON  */
  YYSYMBOL_FOR_ALL = 6,                    /* FOR_ALL  */
  YYSYMBOL_THERE_EXISTS = 7,               /* THERE_EXISTS  */
  YYSYMBOL_IN = 8,                         /* IN  */
  YYSYMBOL_IS = 9,                         /* IS  */
  YYSYMBOL_AND = 10,                       /* AND  */
  YYSYMBOL_OR = 11,                        /* OR  */
  YYSYMBOL_NOT = 12,                       /* NOT  */
  YYSYMBOL_IMPLIES = 13,                   /* IMPLIES  */
  YYSYMBOL_ALL_NODES = 14,                 /* ALL_NODES  */
  YYSYMBOL_LEFT_PAREN = 15,                /* LEFT_PAREN  */
  YYSYMBOL_RIGHT_PAREN = 16,               /* RIGHT_PAREN  */
  YYSYMBOL_PRED = 17,                      /* PRED  */
  YYSYMBOL_SUCC = 18,                      /* SUCC  */
  YYSYMBOL_NODE_TYPE = 19,                 /* NODE_TYPE  */
  YYSYMBOL_EDGE_TYPE = 20,                 /* EDGE_TYPE  */
  YYSYMBOL_EDGE_SUBTYPE = 21,              /* EDGE_SUBTYPE  */
  YYSYMBOL_EDGE_SUBTYPE_DNF = 22,          /* EDGE_SUBTYPE_DNF  */
  YYSYMBOL_NODE_INPUT_EDGE_TYPE = 23,      /* NODE_INPUT_EDGE_TYPE  */
  YYSYMBOL_NODE_OUTPUT_EDGE_TYPE = 24,     /* NODE_OUTPUT_EDGE_TYPE  */
  YYSYMBOL_NODE_INPUT_EDGE_SUBTYPE = 25,   /* NODE_INPUT_EDGE_SUBTYPE  */
  YYSYMBOL_NODE_OUTPUT_EDGE_SUBTYPE = 26,  /* NODE_OUTPUT_EDGE_SUBTYPE  */
  YYSYMBOL_GENE = 27,                      /* GENE  */
  YYSYMBOL_GROUP = 28,                     /* GROUP  */
  YYSYMBOL_COMPOUND = 29,                  /* COMPOUND  */
  YYSYMBOL_PPREL = 30,                     /* PPREL  */
  YYSYMBOL_ECREL = 31,                     /* ECREL  */
  YYSYMBOL_GEREL = 32,                     /* GEREL  */
  YYSYMBOL_PCREL = 33,                     /* PCREL  */
  YYSYMBOL_ACTIVE = 34,                    /* ACTIVE  */
  YYSYMBOL_INHIBIT = 35,                   /* INHIBIT  */
  YYSYMBOL_EXPRESS = 36,                   /* EXPRESS  */
  YYSYMBOL_REPRESS = 37,                   /* REPRESS  */
  YYSYMBOL_INDIRECT = 38,                  /* INDIRECT  */
  YYSYMBOL_STATE_CHANGE = 39,              /* STATE_CHANGE  */
  YYSYMBOL_ASSOC = 40,                     /* ASSOC  */
  YYSYMBOL_DISSOC = 41,                    /* DISSOC  */
  YYSYMBOL_MISSING_INTERACTION = 42,       /* MISSING_INTERACTION  */
  YYSYMBOL_COMPONENT = 43,                 /* COMPONENT  */
  YYSYMBOL_COMMA = 44,                     /* COMMA  */
  YYSYMBOL_SEMICOLON = 45,                 /* SEMICOLON  */
  YYSYMBOL_EDGE = 46,                      /* EDGE  */
  YYSYMBOL_NODE = 47,                      /* NODE  */
  YYSYMBOL_ACTIVATED = 48,                 /* ACTIVATED  */
  YYSYMBOL_INHIBITED = 49,                 /* INHIBITED  */
  YYSYMBOL_OVEREXPRESSED = 50,             /* OVEREXPRESSED  */
  YYSYMBOL_REPRESSED = 51,                 /* REPRESSED  */
  YYSYMBOL_DIFFER_EXPRESSED = 52,          /* DIFFER_EXPRESSED  */
  YYSYMBOL_NOT_DIFFER_EXPRESSED = 53,      /* NOT_DIFFER_EXPRESSED  */
  YYSYMBOL_PRESENT = 54,                   /* PRESENT  */
  YYSYMBOL_ABSENT = 55,                    /* ABSENT  */
  YYSYMBOL_VARIABLE = 56,                  /* VARIABLE  */
  YYSYMBOL_YYACCEPT = 57,                  /* $accept  */
  YYSYMBOL_list_rules = 58,                /* list_rules  */
  YYSYMBOL_rule = 59,                      /* rule  */
  YYSYMBOL_antecedent = 60,                /* antecedent  */
  YYSYMBOL_set_nodes = 61,                 /* set_nodes  */
  YYSYMBOL_var = 62,                       /* var  */
  YYSYMBOL_node_t = 63,                    /* node_t  */
  YYSYMBOL_edge_t = 64,                    /* edge_t  */
  YYSYMBOL_edge_subt = 65,                 /* edge_subt  */
  YYSYMBOL_list_edge_subtypes = 66,        /* list_edge_subtypes  */
  YYSYMBOL_edge_subtype_literal = 67,      /* edge_subtype_literal  */
  YYSYMBOL_ANDed_edge_subtype_clause = 68, /* ANDed_edge_subtype_clause  */
  YYSYMBOL_DNF_edge_subtype = 69,          /* DNF_edge_subtype  */
  YYSYMBOL_consequent = 70,                /* consequent  */
  YYSYMBOL_logical_statement = 71,         /* logical_statement  */
  YYSYMBOL_node_state = 72,                /* node_state  */
  YYSYMBOL_node_state2 = 73,               /* node_state2  */
  YYSYMBOL_edge_state = 74                 /* edge_state  */
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
typedef yytype_uint8 yy_state_t;

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
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   161

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  64
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  163

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   311


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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   136,   136,   147,   159,   169,   184,   199,   214,   230,
     250,   272,   294,   314,   334,   352,   370,   388,   406,   424,
     446,   468,   514,   533,   542,   544,   546,   549,   551,   553,
     555,   558,   560,   562,   564,   566,   568,   570,   572,   574,
     576,   579,   588,   598,   607,   617,   626,   636,   645,   701,
     710,   732,   754,   778,   828,   850,   874,   941,   943,   945,
     947,   950,   952,   955,   957
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
  "\"end of file\"", "error", "\"invalid token\"", "BEGIN_RULE",
  "END_RULE", "COLON", "FOR_ALL", "THERE_EXISTS", "IN", "IS", "AND", "OR",
  "NOT", "IMPLIES", "ALL_NODES", "LEFT_PAREN", "RIGHT_PAREN", "PRED",
  "SUCC", "NODE_TYPE", "EDGE_TYPE", "EDGE_SUBTYPE", "EDGE_SUBTYPE_DNF",
  "NODE_INPUT_EDGE_TYPE", "NODE_OUTPUT_EDGE_TYPE",
  "NODE_INPUT_EDGE_SUBTYPE", "NODE_OUTPUT_EDGE_SUBTYPE", "GENE", "GROUP",
  "COMPOUND", "PPREL", "ECREL", "GEREL", "PCREL", "ACTIVE", "INHIBIT",
  "EXPRESS", "REPRESS", "INDIRECT", "STATE_CHANGE", "ASSOC", "DISSOC",
  "MISSING_INTERACTION", "COMPONENT", "COMMA", "SEMICOLON", "EDGE", "NODE",
  "ACTIVATED", "INHIBITED", "OVEREXPRESSED", "REPRESSED",
  "DIFFER_EXPRESSED", "NOT_DIFFER_EXPRESSED", "PRESENT", "ABSENT",
  "VARIABLE", "$accept", "list_rules", "rule", "antecedent", "set_nodes",
  "var", "node_t", "edge_t", "edge_subt", "list_edge_subtypes",
  "edge_subtype_literal", "ANDed_edge_subtype_clause", "DNF_edge_subtype",
  "consequent", "logical_statement", "node_state", "node_state2",
  "edge_state", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-57)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
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

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
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

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -57,   -57,   155,   -57,    -9,    -5,   -57,   -52,   -56,    11,
      18,    19,   -57,   -57,   -12,   -57,   -57,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     3,     6,    37,    10,    75,    82,   132,   152,
     133,   134,   135,    21,    22,   126,   127,   156
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
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

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
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

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    57,    58,    58,    59,    60,    60,    60,    60,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    62,    63,    63,    63,    64,    64,    64,
      64,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    66,    66,    67,    67,    68,    68,    69,    69,    70,
      71,    71,    71,    71,    71,    71,    71,    72,    72,    72,
      72,    73,    73,    74,    74
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     5,     4,     4,     5,     5,     2,
       3,     3,     4,     4,     4,     4,     4,     4,     4,     8,
       8,    10,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     2,     1,     3,     1,     3,     3,
       6,     6,    12,    10,     2,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1
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
  case 2: /* list_rules: rule  */
#line 137 "src/rules_parser.y"
                                {
					list_rules_t* temp = new list_rules_t;
					assert(temp != NULL);
					temp->rule = (yyvsp[0].rule);
					temp->next = NULL;
					if(list_of_rules.empty())
						list_of_rules.push_back(temp->rule);
					(yyval.list_rules) = temp;
				}
#line 1537 "src/rules_parser.tab.c"
    break;

  case 3: /* list_rules: list_rules rule  */
#line 148 "src/rules_parser.y"
                                {
					list_rules_t* temp = new list_rules_t;
					assert(temp != NULL);
					temp->rule = (yyvsp[0].rule);
					temp->next = (yyvsp[-1].list_rules);
					if(!list_of_rules.empty())
						list_of_rules.push_back(temp->rule);
					(yyval.list_rules) = temp;
				}
#line 1551 "src/rules_parser.tab.c"
    break;

  case 4: /* rule: BEGIN_RULE antecedent COLON consequent END_RULE  */
#line 160 "src/rules_parser.y"
                        {
				rule_t* temp = new rule_t;
				assert(temp != NULL);
				temp->antecedent = (yyvsp[-3].list_antecedent); 
				temp->consequent = (yyvsp[-1].consequent);
				(yyval.rule) = temp;
			}
#line 1563 "src/rules_parser.tab.c"
    break;

  case 5: /* antecedent: FOR_ALL var IN set_nodes  */
#line 170 "src/rules_parser.y"
                                {
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = FOR_ALL_QUANT;
					temp->var = new char[strlen((yyvsp[-2].str))+1];
					assert(temp->var != NULL);
					strcpy(temp->var, (yyvsp[-2].str));
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = (yyvsp[0].set_nodes); 
					temp->next = NULL;
					(yyval.list_antecedent) = temp;
				}
#line 1581 "src/rules_parser.tab.c"
    break;

  case 6: /* antecedent: THERE_EXISTS var IN set_nodes  */
#line 185 "src/rules_parser.y"
                                {
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = THERE_EXISTS_QUANT;
					temp->var = new char[strlen((yyvsp[-2].str))+1];
					assert(temp->var != NULL);
					strcpy(temp->var, (yyvsp[-2].str));
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = (yyvsp[0].set_nodes); 
					temp->next = NULL;
					(yyval.list_antecedent) = temp;
				}
#line 1599 "src/rules_parser.tab.c"
    break;

  case 7: /* antecedent: antecedent FOR_ALL var IN set_nodes  */
#line 200 "src/rules_parser.y"
                                {
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = FOR_ALL_QUANT;
					temp->var = new char[strlen((yyvsp[-2].str))+1];
					assert(temp->var != NULL);
					strcpy(temp->var, (yyvsp[-2].str));
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = (yyvsp[0].set_nodes); 
					temp->next = (yyvsp[-4].list_antecedent);
					(yyval.list_antecedent) = temp;
				}
#line 1617 "src/rules_parser.tab.c"
    break;

  case 8: /* antecedent: antecedent THERE_EXISTS var IN set_nodes  */
#line 215 "src/rules_parser.y"
                                {
					list_antecedent_t* temp = new list_antecedent_t;
					assert(temp != NULL);
					temp->quantifier = THERE_EXISTS_QUANT;
					temp->var = new char[strlen((yyvsp[-2].str))+1];
					assert(temp->var != NULL);
					strcpy(temp->var, (yyvsp[-2].str));
					temp->set_nodes = new set_nodes_t;
					assert(temp->set_nodes != NULL);
					temp->set_nodes = (yyvsp[0].set_nodes); 
					temp->next = (yyvsp[-4].list_antecedent);
					(yyval.list_antecedent) = temp;
				}
#line 1635 "src/rules_parser.tab.c"
    break;

  case 9: /* set_nodes: NOT set_nodes  */
#line 231 "src/rules_parser.y"
                                {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NOT_OPR;
					temp->left = new set_nodes_t;
					assert(temp->left != NULL);
					temp->left = (yyvsp[0].set_nodes);
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
				}
#line 1658 "src/rules_parser.tab.c"
    break;

  case 10: /* set_nodes: set_nodes AND set_nodes  */
#line 251 "src/rules_parser.y"
                                {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = AND_OPR;
					temp->left = new set_nodes_t;
					assert(temp->left != NULL);
					temp->left = (yyvsp[-2].set_nodes);
					temp->right = new set_nodes_t;
					assert(temp->right != NULL);
					temp->right = (yyvsp[0].set_nodes);
					temp->fun_name = NONE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1683 "src/rules_parser.tab.c"
    break;

  case 11: /* set_nodes: set_nodes OR set_nodes  */
#line 273 "src/rules_parser.y"
                                {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = OR_OPR;
					temp->left = new set_nodes_t;
					assert(temp->left != NULL);
					temp->left = (yyvsp[-2].set_nodes);
					temp->right = new set_nodes_t;
					assert(temp->right != NULL);
					temp->right = (yyvsp[0].set_nodes);
					temp->fun_name = NONE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1708 "src/rules_parser.tab.c"
    break;

  case 12: /* set_nodes: PRED LEFT_PAREN var RIGHT_PAREN  */
#line 295 "src/rules_parser.y"
                                {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = PRED_FUN;
					temp->arg1 = new char[strlen((yyvsp[-1].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[-1].str));
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1731 "src/rules_parser.tab.c"
    break;

  case 13: /* set_nodes: SUCC LEFT_PAREN var RIGHT_PAREN  */
#line 315 "src/rules_parser.y"
                                {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = SUCC_FUN;
					temp->arg1 = new char[strlen((yyvsp[-1].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[-1].str));
					temp->arg2 = NULL;
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1754 "src/rules_parser.tab.c"
    break;

  case 14: /* set_nodes: NODE_TYPE LEFT_PAREN node_t RIGHT_PAREN  */
#line 335 "src/rules_parser.y"
                                {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = NODE_TYPE_FUN;
					temp->arg1 = NULL;
					temp->arg2 = NULL;
					temp->arg3_node = (yyvsp[-1].node_type);
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1775 "src/rules_parser.tab.c"
    break;

  case 15: /* set_nodes: NODE_INPUT_EDGE_TYPE LEFT_PAREN edge_t RIGHT_PAREN  */
#line 353 "src/rules_parser.y"
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
					temp->arg3_edge = (yyvsp[-1].edge_type);
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1796 "src/rules_parser.tab.c"
    break;

  case 16: /* set_nodes: NODE_OUTPUT_EDGE_TYPE LEFT_PAREN edge_t RIGHT_PAREN  */
#line 371 "src/rules_parser.y"
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
					temp->arg3_edge = (yyvsp[-1].edge_type);
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1817 "src/rules_parser.tab.c"
    break;

  case 17: /* set_nodes: NODE_INPUT_EDGE_SUBTYPE LEFT_PAREN edge_subt RIGHT_PAREN  */
#line 389 "src/rules_parser.y"
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
					temp->arg3_edgesub = (yyvsp[-1].edge_subtype);
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1838 "src/rules_parser.tab.c"
    break;

  case 18: /* set_nodes: NODE_OUTPUT_EDGE_SUBTYPE LEFT_PAREN edge_subt RIGHT_PAREN  */
#line 407 "src/rules_parser.y"
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
					temp->arg3_edgesub = (yyvsp[-1].edge_subtype);
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1859 "src/rules_parser.tab.c"
    break;

  case 19: /* set_nodes: EDGE_TYPE LEFT_PAREN var COMMA var COMMA edge_t RIGHT_PAREN  */
#line 425 "src/rules_parser.y"
                                {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_TYPE_FUN;
					temp->arg1 = new char[strlen((yyvsp[-5].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[-5].str));
					temp->arg2 = new char[strlen((yyvsp[-3].str))+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, (yyvsp[-3].str));
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = (yyvsp[-1].edge_type);
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1884 "src/rules_parser.tab.c"
    break;

  case 20: /* set_nodes: EDGE_SUBTYPE LEFT_PAREN var COMMA var COMMA edge_subt RIGHT_PAREN  */
#line 447 "src/rules_parser.y"
                                {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_SUBTYPE_FUN;
					temp->arg1 = new char[strlen((yyvsp[-5].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[-5].str));
					temp->arg2 = new char[strlen((yyvsp[-3].str))+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, (yyvsp[-3].str));
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = NONE_EDGE;
					temp->arg3_edgesub = (yyvsp[-1].edge_subtype);
					temp->arg4_edgesub_list = NULL;
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1909 "src/rules_parser.tab.c"
    break;

  case 21: /* set_nodes: EDGE LEFT_PAREN var COMMA var COMMA edge_t COMMA list_edge_subtypes RIGHT_PAREN  */
#line 469 "src/rules_parser.y"
                                {
					set_nodes_t* temp = new set_nodes_t;
					assert(temp != NULL);
					temp->opr = NONE_OPR;
					temp->left = NULL;
					temp->right = NULL;
					temp->fun_name = EDGE_FUN;
					temp->arg1 = new char[strlen((yyvsp[-7].str))+1];
					assert(temp->arg1 != NULL);
					strcpy(temp->arg1, (yyvsp[-7].str));
					temp->arg2 = new char[strlen((yyvsp[-5].str))+1];
					assert(temp->arg2 != NULL);
					strcpy(temp->arg2, (yyvsp[-5].str));
					temp->arg3_node = NONE_NODE;
					temp->arg3_edge = (yyvsp[-3].edge_type);
					temp->arg3_edgesub = NONE_EDGE_SUB;
					temp->arg4_edgesub_list = (yyvsp[-1].list_edge_subtypes);
					//temp->arg5_edgesub_logical = NULL;
					(yyval.set_nodes) = temp;
				}
#line 1934 "src/rules_parser.tab.c"
    break;

  case 22: /* set_nodes: ALL_NODES  */
#line 515 "src/rules_parser.y"
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
				}
#line 1955 "src/rules_parser.tab.c"
    break;

  case 23: /* var: VARIABLE  */
#line 534 "src/rules_parser.y"
                        {
				char* temp = new char[strlen((yyvsp[0].str))+1];
				assert(temp != NULL);
				strcpy(temp, (yyvsp[0].str));
				(yyval.str) = temp;
			}
#line 1966 "src/rules_parser.tab.c"
    break;

  case 24: /* node_t: GENE  */
#line 542 "src/rules_parser.y"
                     {(yyval.node_type) = GENE_NODE;}
#line 1972 "src/rules_parser.tab.c"
    break;

  case 25: /* node_t: GROUP  */
#line 544 "src/rules_parser.y"
                              {(yyval.node_type) = GROUP_NODE;}
#line 1978 "src/rules_parser.tab.c"
    break;

  case 26: /* node_t: COMPOUND  */
#line 546 "src/rules_parser.y"
                                 {(yyval.node_type) = COMPOUND_NODE;}
#line 1984 "src/rules_parser.tab.c"
    break;

  case 27: /* edge_t: PPREL  */
#line 549 "src/rules_parser.y"
                      {(yyval.edge_type) = PPREL_EDGE;}
#line 1990 "src/rules_parser.tab.c"
    break;

  case 28: /* edge_t: ECREL  */
#line 551 "src/rules_parser.y"
                              {(yyval.edge_type) = ECREL_EDGE;}
#line 1996 "src/rules_parser.tab.c"
    break;

  case 29: /* edge_t: GEREL  */
#line 553 "src/rules_parser.y"
                              {(yyval.edge_type) = GEREL_EDGE;}
#line 2002 "src/rules_parser.tab.c"
    break;

  case 30: /* edge_t: PCREL  */
#line 555 "src/rules_parser.y"
                              {(yyval.edge_type) = PCREL_EDGE;}
#line 2008 "src/rules_parser.tab.c"
    break;

  case 31: /* edge_subt: ACTIVE  */
#line 558 "src/rules_parser.y"
                               {(yyval.edge_subtype) = ACTIVE_EDGE;}
#line 2014 "src/rules_parser.tab.c"
    break;

  case 32: /* edge_subt: INHIBIT  */
#line 560 "src/rules_parser.y"
                                        {(yyval.edge_subtype) = INHIBIT_EDGE;}
#line 2020 "src/rules_parser.tab.c"
    break;

  case 33: /* edge_subt: EXPRESS  */
#line 562 "src/rules_parser.y"
                                        {(yyval.edge_subtype) = EXPRESS_EDGE;}
#line 2026 "src/rules_parser.tab.c"
    break;

  case 34: /* edge_subt: REPRESS  */
#line 564 "src/rules_parser.y"
                                        {(yyval.edge_subtype) = REPRESS_EDGE;}
#line 2032 "src/rules_parser.tab.c"
    break;

  case 35: /* edge_subt: INDIRECT  */
#line 566 "src/rules_parser.y"
                                         {(yyval.edge_subtype) = INDIRECT_EDGE;}
#line 2038 "src/rules_parser.tab.c"
    break;

  case 36: /* edge_subt: STATE_CHANGE  */
#line 568 "src/rules_parser.y"
                                             {(yyval.edge_subtype) = STATE_CHANGE_EDGE;}
#line 2044 "src/rules_parser.tab.c"
    break;

  case 37: /* edge_subt: ASSOC  */
#line 570 "src/rules_parser.y"
                                      {(yyval.edge_subtype) = ASSOC_EDGE;}
#line 2050 "src/rules_parser.tab.c"
    break;

  case 38: /* edge_subt: DISSOC  */
#line 572 "src/rules_parser.y"
                                       {(yyval.edge_subtype) = DISSOC_EDGE;}
#line 2056 "src/rules_parser.tab.c"
    break;

  case 39: /* edge_subt: MISSING_INTERACTION  */
#line 574 "src/rules_parser.y"
                                                    {(yyval.edge_subtype) = MISSING_INTERACTION_EDGE;}
#line 2062 "src/rules_parser.tab.c"
    break;

  case 40: /* edge_subt: COMPONENT  */
#line 576 "src/rules_parser.y"
                                          {(yyval.edge_subtype) = COMPONENT_EDGE;}
#line 2068 "src/rules_parser.tab.c"
    break;

  case 41: /* list_edge_subtypes: edge_subt  */
#line 580 "src/rules_parser.y"
                                {
					list_edge_subtypes_t* temp = new list_edge_subtypes_t;
					assert(temp != NULL);
					temp->edge_subtype_data = (yyvsp[0].edge_subtype);
					temp->next = NULL;
					(yyval.list_edge_subtypes) = temp;
				}
#line 2080 "src/rules_parser.tab.c"
    break;

  case 42: /* list_edge_subtypes: list_edge_subtypes SEMICOLON edge_subt  */
#line 589 "src/rules_parser.y"
                                {
					list_edge_subtypes_t* temp = new list_edge_subtypes_t;
					assert(temp != NULL);
					temp->edge_subtype_data = (yyvsp[0].edge_subtype);
					temp->next = (yyvsp[-2].list_edge_subtypes);
					(yyval.list_edge_subtypes) = temp;
				}
#line 2092 "src/rules_parser.tab.c"
    break;

  case 43: /* edge_subtype_literal: edge_subt  */
#line 599 "src/rules_parser.y"
                                                        {
								edge_subt_literal_t* temp = new edge_subt_literal_t;
								assert(temp != NULL);
								temp->opr = NONE_OPR;
								temp->edge_subtype_data = (yyvsp[0].edge_subtype);
								(yyval.edge_subtype_literal) = temp;
							}
#line 2104 "src/rules_parser.tab.c"
    break;

  case 44: /* edge_subtype_literal: NOT edge_subt  */
#line 608 "src/rules_parser.y"
                                                        {
								edge_subt_literal_t* temp = new edge_subt_literal_t;
								assert(temp != NULL);
								temp->opr = NOT_OPR;
								temp->edge_subtype_data = (yyvsp[0].edge_subtype);
								(yyval.edge_subtype_literal) = temp;
							}
#line 2116 "src/rules_parser.tab.c"
    break;

  case 45: /* ANDed_edge_subtype_clause: edge_subtype_literal  */
#line 618 "src/rules_parser.y"
                                                                {
									ANDed_edgesub_literals_t* temp = new ANDed_edgesub_literals_t;
									assert(temp != NULL);
									temp->curr_literal= (yyvsp[0].edge_subtype_literal);
									temp->next = NULL;									
									(yyval.ANDed_edge_subtype_clause) = temp;
								}
#line 2128 "src/rules_parser.tab.c"
    break;

  case 46: /* ANDed_edge_subtype_clause: ANDed_edge_subtype_clause AND edge_subtype_literal  */
#line 627 "src/rules_parser.y"
                                                                {
									ANDed_edgesub_literals_t* temp = new ANDed_edgesub_literals_t;
									assert(temp != NULL);
									temp->curr_literal= (yyvsp[0].edge_subtype_literal);
									temp->next = (yyvsp[-2].ANDed_edge_subtype_clause);									
									(yyval.ANDed_edge_subtype_clause) = temp;
								}
#line 2140 "src/rules_parser.tab.c"
    break;

  case 47: /* DNF_edge_subtype: ANDed_edge_subtype_clause  */
#line 637 "src/rules_parser.y"
                                                {
							DNF_edgesub_t* temp = new DNF_edgesub_t;
							assert(temp != NULL);
							temp->current_ANDed_clause = (yyvsp[0].ANDed_edge_subtype_clause);
							temp->next = NULL;
							(yyval.DNF_edge_subtype)= temp;
						}
#line 2152 "src/rules_parser.tab.c"
    break;

  case 48: /* DNF_edge_subtype: DNF_edge_subtype OR ANDed_edge_subtype_clause  */
#line 646 "src/rules_parser.y"
                                                {
							DNF_edgesub_t* temp = new DNF_edgesub_t;
							assert(temp != NULL);
							temp->current_ANDed_clause = (yyvsp[0].ANDed_edge_subtype_clause);
							temp->next = (yyvsp[-2].DNF_edge_subtype);
							(yyval.DNF_edge_subtype)= temp;
						}
#line 2164 "src/rules_parser.tab.c"
    break;

  case 49: /* consequent: logical_statement IMPLIES logical_statement  */
#line 702 "src/rules_parser.y"
                                {
					consequent_t* temp = new consequent_t;
					temp->left_of_implies = (yyvsp[-2].logical_stmt);
					temp->right_of_implies = (yyvsp[0].logical_stmt);
					(yyval.consequent) = temp;
				}
#line 2175 "src/rules_parser.tab.c"
    break;

  case 50: /* logical_statement: NODE LEFT_PAREN var RIGHT_PAREN IS node_state  */
#line 711 "src/rules_parser.y"
                                                {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = NODE_FUN2;
							temp->arg1 = new char[strlen((yyvsp[-3].str))+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, (yyvsp[-3].str));
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = (yyvsp[0].node_state);
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = EDGE_NONE_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							(yyval.logical_stmt) = temp;
						}
#line 2200 "src/rules_parser.tab.c"
    break;

  case 51: /* logical_statement: NODE LEFT_PAREN var RIGHT_PAREN IS node_state2  */
#line 733 "src/rules_parser.y"
                                                {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = NODE_FUN2;
							temp->arg1 = new char[strlen((yyvsp[-3].str))+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, (yyvsp[-3].str));
							temp->arg2 = NULL;
							temp->arg3 = NONE_EDGE;
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = (yyvsp[0].node_state2);
							temp->state_edge = EDGE_NONE_STATE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							(yyval.logical_stmt) = temp;
						}
#line 2225 "src/rules_parser.tab.c"
    break;

  case 52: /* logical_statement: EDGE LEFT_PAREN var COMMA var COMMA edge_t COMMA list_edge_subtypes RIGHT_PAREN IS edge_state  */
#line 755 "src/rules_parser.y"
                                                {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = EDGE_FUN2;
							temp->arg1 = new char[strlen((yyvsp[-9].str))+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, (yyvsp[-9].str));
							temp->arg2 = new char[strlen((yyvsp[-7].str))+1];
							assert(temp->arg2 != NULL);
							strcpy(temp->arg2, (yyvsp[-7].str));
							temp->arg3 = (yyvsp[-5].edge_type);
							temp->arg4_list = (yyvsp[-3].list_edge_subtypes);
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = NULL;
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = (yyvsp[0].edge_state);
							(yyval.logical_stmt) = temp;
						}
#line 2252 "src/rules_parser.tab.c"
    break;

  case 53: /* logical_statement: EDGE_SUBTYPE_DNF LEFT_PAREN var COMMA var COMMA DNF_edge_subtype RIGHT_PAREN IS edge_state  */
#line 779 "src/rules_parser.y"
                                                {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NONE_OPR;
							temp->left = NULL;
							temp->right = NULL;
							temp->fun_name2 = EDGE_SUBTYPE_DNF_FUN2;
							temp->arg1 = new char[strlen((yyvsp[-7].str))+1];
							assert(temp->arg1 != NULL);
							strcpy(temp->arg1, (yyvsp[-7].str));
							temp->arg2 = new char[strlen((yyvsp[-5].str))+1];
							assert(temp->arg2 != NULL);
							strcpy(temp->arg2, (yyvsp[-5].str));
							temp->arg3 = NONE_EDGE;
							temp->arg4_list = NULL;
							//temp->arg5_edgesub_logical = NULL;
							temp->arg_DNF_edgesub = (yyvsp[-3].DNF_edge_subtype);
							temp->state_node = NODE_NONE_STATE;
							temp->state_node2 = NODE_NONE2_STATE;
							temp->state_edge = (yyvsp[0].edge_state);
							(yyval.logical_stmt) = temp;
						}
#line 2279 "src/rules_parser.tab.c"
    break;

  case 54: /* logical_statement: NOT logical_statement  */
#line 829 "src/rules_parser.y"
                                                {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = NOT_OPR;
							temp->left = new logical_stmt_t;
							assert(temp->left != NULL);
							temp->left = (yyvsp[0].logical_stmt);
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
						}
#line 2304 "src/rules_parser.tab.c"
    break;

  case 55: /* logical_statement: logical_statement OR logical_statement  */
#line 851 "src/rules_parser.y"
                                                {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = OR_OPR;
							temp->left = new logical_stmt_t;
							assert(temp->left != NULL);
							temp->left = (yyvsp[-2].logical_stmt);
							temp->right = new logical_stmt_t;
							assert(temp->right != NULL);
							temp->right = (yyvsp[0].logical_stmt);
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
						}
#line 2331 "src/rules_parser.tab.c"
    break;

  case 56: /* logical_statement: logical_statement AND logical_statement  */
#line 875 "src/rules_parser.y"
                                                {
							logical_stmt_t* temp = new logical_stmt_t;
							assert(temp != NULL);
							temp->opr = AND_OPR;
							temp->left = new logical_stmt_t;
							assert(temp->left != NULL);
							temp->left = (yyvsp[-2].logical_stmt);
							temp->right = new logical_stmt_t;
							assert(temp->right != NULL);
							temp->right = (yyvsp[0].logical_stmt);
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
						}
#line 2358 "src/rules_parser.tab.c"
    break;

  case 57: /* node_state: ACTIVATED  */
#line 941 "src/rules_parser.y"
                                  {(yyval.node_state) = ACTIVATED_STATE;}
#line 2364 "src/rules_parser.tab.c"
    break;

  case 58: /* node_state: INHIBITED  */
#line 943 "src/rules_parser.y"
                                          {(yyval.node_state) = INHIBITED_STATE;}
#line 2370 "src/rules_parser.tab.c"
    break;

  case 59: /* node_state: REPRESSED  */
#line 945 "src/rules_parser.y"
                                          {(yyval.node_state) = REPRESSED_STATE;}
#line 2376 "src/rules_parser.tab.c"
    break;

  case 60: /* node_state: OVEREXPRESSED  */
#line 947 "src/rules_parser.y"
                                              {(yyval.node_state) = OVEREXPRESSED_STATE;}
#line 2382 "src/rules_parser.tab.c"
    break;

  case 61: /* node_state2: DIFFER_EXPRESSED  */
#line 950 "src/rules_parser.y"
                                 {(yyval.node_state2) = DIFFER_EXPRESSED_STATE;}
#line 2388 "src/rules_parser.tab.c"
    break;

  case 62: /* node_state2: NOT_DIFFER_EXPRESSED  */
#line 952 "src/rules_parser.y"
                                                     {(yyval.node_state2) = NOT_DIFFER_EXPRESSED_STATE;}
#line 2394 "src/rules_parser.tab.c"
    break;

  case 63: /* edge_state: PRESENT  */
#line 955 "src/rules_parser.y"
                                {(yyval.edge_state) = PRESENT_STATE;}
#line 2400 "src/rules_parser.tab.c"
    break;

  case 64: /* edge_state: ABSENT  */
#line 957 "src/rules_parser.y"
                                       {(yyval.edge_state) = ABSENT_STATE;}
#line 2406 "src/rules_parser.tab.c"
    break;


#line 2410 "src/rules_parser.tab.c"

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

#line 960 "src/rules_parser.y"


void yyerror(char const* message)
{
    cerr << message;
}

int init (void) {
    //yyparse();
    return (1);
}

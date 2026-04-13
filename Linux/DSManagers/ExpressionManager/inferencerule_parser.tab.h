/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_INFERENCERULE_PARSER_INFERENCERULE_PARSER_TAB_H_INCLUDED
# define YY_INFERENCERULE_PARSER_INFERENCERULE_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int inferencerule_parserdebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    t_String = 258,                /* t_String  */
    t_StringLiteral = 259,         /* t_StringLiteral  */
    t_TypeValue = 260,             /* t_TypeValue  */
    t_OBrack = 261,                /* t_OBrack  */
    t_CBrack = 262,                /* t_CBrack  */
    t_Comma = 263,                 /* t_Comma  */
    t_Eq = 264,                    /* t_Eq  */
    t_OSquareBrack = 265,          /* t_OSquareBrack  */
    t_CSquareBrack = 266,          /* t_CSquareBrack  */
    t_OperatorString = 267,        /* t_OperatorString  */
    t_MinArgs = 268,               /* t_MinArgs  */
    t_MaxArgs = 269,               /* t_MaxArgs  */
    t_Operand1 = 270,              /* t_Operand1  */
    t_Operand2 = 271,              /* t_Operand2  */
    t_Operand3 = 272,              /* t_Operand3  */
    t_OtherOperandsString = 273,   /* t_OtherOperandsString  */
    t_InferString = 274,           /* t_InferString  */
    t_InfinityString = 275,        /* t_InfinityString  */
    t_Hash = 276,                  /* t_Hash  */
    t_Comments = 277,              /* t_Comments  */
    t_Add = 278,                   /* t_Add  */
    t_Max = 279,                   /* t_Max  */
    t_Min = 280,                   /* t_Min  */
    t_Addall = 281,                /* t_Addall  */
    INT_NUMBER = 282,              /* INT_NUMBER  */
    t_ArgumentNumber = 283,        /* t_ArgumentNumber  */
    t_Variable = 284,              /* t_Variable  */
    t_print = 285,                 /* t_print  */
    t_FCBrack = 286,               /* t_FCBrack  */
    t_FOBrack = 287,               /* t_FOBrack  */
    t_Once = 288,                  /* t_Once  */
    t_Every = 289,                 /* t_Every  */
    t_SemiColon = 290,             /* t_SemiColon  */
    t_KeyWordsOfPrint = 291,       /* t_KeyWordsOfPrint  */
    t_printAll = 292,              /* t_printAll  */
    t_CastString = 293             /* t_CastString  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 57 "inferencerule_parser.y"

  char *string_value;
  int int_value;

#line 107 "inferencerule_parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE inferencerule_parserlval;


int inferencerule_parserparse (void);


#endif /* !YY_INFERENCERULE_PARSER_INFERENCERULE_PARSER_TAB_H_INCLUDED  */

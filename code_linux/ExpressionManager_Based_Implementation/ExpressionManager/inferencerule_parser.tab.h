/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 57 "inferencerule_parser.y" /* yacc.c:1909  */

  char *string_value;
  int int_value;

#line 98 "inferencerule_parser.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE inferencerule_parserlval;

int inferencerule_parserparse (void);

#endif /* !YY_INFERENCERULE_PARSER_INFERENCERULE_PARSER_TAB_H_INCLUDED  */

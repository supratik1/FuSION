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

#ifndef YY_RULES_PARSER_RULES_PARSER_TAB_H_INCLUDED
# define YY_RULES_PARSER_RULES_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int rules_parserdebug;
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
    t_Arrow = 259,                 /* t_Arrow  */
    t_True = 260,                  /* t_True  */
    t_False = 261,                 /* t_False  */
    t_Constant = 262,              /* t_Constant  */
    t_Integer = 263,               /* t_Integer  */
    t_Variable = 264,              /* t_Variable  */
    t_OBrack = 265,                /* t_OBrack  */
    t_CBrack = 266,                /* t_CBrack  */
    t_Comma = 267,                 /* t_Comma  */
    t_Eq = 268,                    /* t_Eq  */
    t_BoolVariable = 269           /* t_BoolVariable  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 31 "rules_parser.y"

  char  *string_value;
  int   int_value;
  class t_RewriteRuleExpression *bool_expression_type;
  class t_RewriteRuleExpression *expression_type;
  class t_RewriteRule *rule_type;
  
  class t_VectorOfRewriteRuleExpressions *vector_of_exprs_type;

#line 88 "rules_parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE rules_parserlval;


int rules_parserparse (void);


#endif /* !YY_RULES_PARSER_RULES_PARSER_TAB_H_INCLUDED  */

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

#ifndef YY_RULES_PARSER_RULES_PARSER_TAB_H_INCLUDED
# define YY_RULES_PARSER_RULES_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int rules_parserdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 30 "rules_parser.y" /* yacc.c:1909  */

  char  *string_value;
  int   int_value;
  class t_RewriteRuleExpression *bool_expression_type;
  class t_RewriteRuleExpression *expression_type;
  class t_RewriteRule *rule_type;
  
  class t_VectorOfRewriteRuleExpressions *vector_of_exprs_type;

#line 79 "rules_parser.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE rules_parserlval;

int rules_parserparse (void);

#endif /* !YY_RULES_PARSER_RULES_PARSER_TAB_H_INCLUDED  */

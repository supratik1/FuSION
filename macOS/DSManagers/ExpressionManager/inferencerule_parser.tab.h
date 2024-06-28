/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 57 "inferencerule_parser.y"
{
  char *string_value;
  int int_value;
}
/* Line 1529 of yacc.c.  */
#line 130 "inferencerule_parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE inferencerule_parserlval;


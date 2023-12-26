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

#ifndef YY_YY_SRC_RULES_PARSER_TAB_H_INCLUDED
# define YY_YY_SRC_RULES_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
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
    BEGIN_RULE = 258,              /* BEGIN_RULE  */
    END_RULE = 259,                /* END_RULE  */
    COLON = 260,                   /* COLON  */
    FOR_ALL = 261,                 /* FOR_ALL  */
    THERE_EXISTS = 262,            /* THERE_EXISTS  */
    IN = 263,                      /* IN  */
    IS = 264,                      /* IS  */
    AND = 265,                     /* AND  */
    OR = 266,                      /* OR  */
    NOT = 267,                     /* NOT  */
    IMPLIES = 268,                 /* IMPLIES  */
    ALL_NODES = 269,               /* ALL_NODES  */
    LEFT_PAREN = 270,              /* LEFT_PAREN  */
    RIGHT_PAREN = 271,             /* RIGHT_PAREN  */
    PRED = 272,                    /* PRED  */
    SUCC = 273,                    /* SUCC  */
    NODE_TYPE = 274,               /* NODE_TYPE  */
    EDGE_TYPE = 275,               /* EDGE_TYPE  */
    EDGE_SUBTYPE = 276,            /* EDGE_SUBTYPE  */
    EDGE_SUBTYPE_DNF = 277,        /* EDGE_SUBTYPE_DNF  */
    NODE_INPUT_EDGE_TYPE = 278,    /* NODE_INPUT_EDGE_TYPE  */
    NODE_OUTPUT_EDGE_TYPE = 279,   /* NODE_OUTPUT_EDGE_TYPE  */
    NODE_INPUT_EDGE_SUBTYPE = 280, /* NODE_INPUT_EDGE_SUBTYPE  */
    NODE_OUTPUT_EDGE_SUBTYPE = 281, /* NODE_OUTPUT_EDGE_SUBTYPE  */
    GENE = 282,                    /* GENE  */
    GROUP = 283,                   /* GROUP  */
    COMPOUND = 284,                /* COMPOUND  */
    PPREL = 285,                   /* PPREL  */
    ECREL = 286,                   /* ECREL  */
    GEREL = 287,                   /* GEREL  */
    PCREL = 288,                   /* PCREL  */
    ACTIVE = 289,                  /* ACTIVE  */
    INHIBIT = 290,                 /* INHIBIT  */
    EXPRESS = 291,                 /* EXPRESS  */
    REPRESS = 292,                 /* REPRESS  */
    INDIRECT = 293,                /* INDIRECT  */
    STATE_CHANGE = 294,            /* STATE_CHANGE  */
    ASSOC = 295,                   /* ASSOC  */
    DISSOC = 296,                  /* DISSOC  */
    MISSING_INTERACTION = 297,     /* MISSING_INTERACTION  */
    COMPONENT = 298,               /* COMPONENT  */
    COMMA = 299,                   /* COMMA  */
    SEMICOLON = 300,               /* SEMICOLON  */
    EDGE = 301,                    /* EDGE  */
    NODE = 302,                    /* NODE  */
    ACTIVATED = 303,               /* ACTIVATED  */
    INHIBITED = 304,               /* INHIBITED  */
    OVEREXPRESSED = 305,           /* OVEREXPRESSED  */
    REPRESSED = 306,               /* REPRESSED  */
    DIFFER_EXPRESSED = 307,        /* DIFFER_EXPRESSED  */
    NOT_DIFFER_EXPRESSED = 308,    /* NOT_DIFFER_EXPRESSED  */
    PRESENT = 309,                 /* PRESENT  */
    ABSENT = 310,                  /* ABSENT  */
    VARIABLE = 311                 /* VARIABLE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 27 "src/rules_parser.y"

  char* str;
  opr_t opr;
  fun_name_t fun_name;
  fun_name2_t fun_name2;
  quantifier_t quantifier;
  node_t node_type;
  edge_t edge_type;
  edge_subt edge_subtype;
  node_state_t node_state;
  node_state2_t node_state2;
  edge_state_t edge_state;
  set_nodes_t* set_nodes;
  list_edge_subtypes_t* list_edge_subtypes;
  //logical_edge_subt_t* logical_edge_subtypes;
  edge_subt_literal_t* edge_subtype_literal;
  ANDed_edgesub_literals_t* ANDed_edge_subtype_clause;
  DNF_edgesub_t* DNF_edge_subtype;
  list_antecedent_t* list_antecedent;
  logical_stmt_t* logical_stmt;
  consequent_t* consequent;
  rule_t* rule;
  list_rules_t* list_rules;

#line 145 "src/rules_parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_RULES_PARSER_TAB_H_INCLUDED  */

#!/bin/bash

bison -d rules_parser.y #-d to get separate header file generated
lex rules_parser.l 
g++ lex.yy.c rules_parser.tab.c -o rules_parser.out
#cc -o template_scanner template_scanner.o -ll


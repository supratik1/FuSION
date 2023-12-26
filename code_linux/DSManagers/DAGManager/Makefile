SS = ..
include $(SS)/Makefile.include
#include $(LIRA)/ConfigFiles/lira.mak
RULES_LEXER = rules_parser
RULES_PARSER =rules_parser
DAG_LEXER = dag_parser
DAG_PARSER = dag_parser
YACC_UTILS = yacc_utils

build : $(RULES_PARSER).tab.o $(RULES_LEXER).yy.o $(DAG_PARSER).tab.o $(DAG_LEXER).yy.o $(DAG_MANAGER).o $(YACC_UTILS).o $(ADVANCED_DAG_MAN).o $(DAG_SIMPLIFIER).o $(DAGNODE).o $(STRATEGIES).o $(MATCHING_FNS).o $(DAG_SIMPLIFIER).o $(GC).o

$(DAG_MANAGER).o : $(DAG_MANAGER).cc $(DAG_MANAGER).h $(DAG_PARSER).tab.o
	$(CC) $(CFLAGS) $(DAG_MANAGER).cc $(DAG_MANAGER).h -I $(HASHTABLE_MANAGER_DIR) -I $(COMMON_DIR) -I $(LOGMANAGER_DIR) $(INCLUDE)

$(ADVANCED_DAG_MAN).o : $(ADVANCED_DAG_MAN).cc $(DAG_MANAGER).h
	$(CC) $(CFLAGS) $(ADVANCED_DAG_MAN).cc $(DAG_MANAGER).h -I $(HASHTABLE_MANAGER_DIR) -I $(COMMON_DIR) -I $(LOGMANAGER_DIR) $(INCLUDE)

$(DAGNODE).o : $(DAGNODE).cc $(DAG_MANAGER).h
	$(CC) $(CFLAGS) $(DAGNODE).cc $(DAG_MANAGER).h -I $(HASHTABLE_MANAGER_DIR) -I $(COMMON_DIR) -I $(LOGMANAGER_DIR) $(INCLUDE)

$(DAG_SIMPLIFIER).o : $(DAG_SIMPLIFIER).cc $(DAG_SIMPLIFIER).h
	$(CC) $(CFLAGS) $(DAG_SIMPLIFIER).cc $(DAG_SIMPLIFIER).h -I $(HASHTABLE_MANAGER_DIR) -I $(COMMON_DIR) -I $(LOGMANAGER_DIR) $(INCLUDE)


$(DAG_LEXER).yy.o : $(DAG_LEXER).yy.c
	$(CC) $(CFLAGS) $(DAG_LEXER).yy.c $(INCLUDE)


$(DAG_PARSER).tab.o : $(DAG_PARSER).tab.c
	$(CC) $(CFLAGS) $(DAG_PARSER).tab.c -I $(HASHTABLE_MANAGER_DIR) -I $(COMMON_DIR) -I $(LOGMANAGER_DIR) $(INCLUDE)

$(DAG_LEXER).yy.c : $(DAG_LEXER).l
	$(LEX) -o$(DAG_LEXER).yy.c -P$(DAG_LEXER) $(DAG_LEXER).l

#$(LEX) -o $(DAG_LEXER).yy.c --prefix=dag_parser $(DAG_LEXER).l


$(DAG_PARSER).tab.c : $(DAG_PARSER).y
	$(YACC) $(YFLAGS) -b dag_parser -p dag_parser $(DAG_PARSER).y

#$(YACC) $(YFLAGS) --name-prefix=dag_parser --file-prefix=dag_parser $(DAG_PARSER).y  



$(YACC_UTILS).o : $(YACC_UTILS).cc $(YACC_UTILS).h
	$(CC) $(CFLAGS) $(YACC_UTILS).cc -I $(HASHTABLE_MANAGER_DIR) -I $(LOGMANAGER_DIR) -I $(COMMON_DIR) $(INCLUDE)


$(RULES_LEXER).yy.o : $(RULES_LEXER).yy.c
	$(CC) $(CFLAGS) $(RULES_LEXER).yy.c $(INCLUDE)


$(RULES_PARSER).tab.o : $(RULES_PARSER).tab.c
	$(CC) $(CFLAGS) $(RULES_PARSER).tab.c -I $(HASHTABLE_MANAGER_DIR) -I $(COMMON_DIR) -I $(LOGMANAGER_DIR) $(INCLUDE)


$(RULES_LEXER).yy.c : $(RULES_LEXER).l
	$(LEX) -o$(RULES_LEXER).yy.c -P$(RULES_LEXER) $(RULES_LEXER).l

#$(LEX) -o $(RULES_LEXER).yy.c --prefix=rules_parser $(RULES_LEXER).l


$(RULES_PARSER).tab.c : $(RULES_PARSER).y
	$(YACC) $(YFLAGS) -b rules_parser -p rules_parser $(RULES_PARSER).y
#$(YACC) $(YFLAGS) --name-prefix=rules_parser --file-prefix=rules_parser $(RULES_PARSER).y  

$(STRATEGIES).o : $(STRATEGIES).cc
	$(CC) $(CFLAGS) $(STRATEGIES).cc -I $(HASHTABLE_MANAGER_DIR) -I $(COMMON_DIR) -I $(LOGMANAGER_DIR) $(INCLUDE)

$(MATCHING_FNS).o : $(MATCHING_FNS).cc
	$(CC) $(CFLAGS) $(MATCHING_FNS).cc -I $(HASHTABLE_MANAGER_DIR) -I $(COMMON_DIR) -I $(LOGMANAGER_DIR) $(INCLUDE)

$(GC).o : $(GC).cc $(GC).h $(DAG_MANAGER).h
	$(CC) $(CFLAGS) $(GC).cc -I $(HASHTABLE_MANAGER_DIR) -I $(COMMON_DIR) -I $(LOGMANAGER_DIR) $(INCLUDE)
clean:
	rm -f *.o *.gch *.*.gch lex.yy.c y.tab.c *.output y.tab.h *.c *.tab.* *.h.gch

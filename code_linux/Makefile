CC = g++
MAKE = make
SOURCE_DIR = src
DS_DIR = DSManagers
BUILD_DIR = build
INCLUDE_DIR = include
TARGET = fusionn 
SUPPORTING_FILES_DIR = supporting_files

CFLAGS =  -c -g -DQE -DDUP_CHECK_FLAG_SET -DDUMMY_EDGE_FLAG -DCOMPUTE_MINCUT_EDGES_FLAG -DONLY_INTERAC_EDGES_FOR_MINCUT -DOPT_GAP_HEUR_FLAG -DASSERT_FLAG -DDEBUG_FLAG -DUSE_FLOAT_EDGE_WEIGHTS_FOR_MINCUT -std=c++0x -Wall -Wno-deprecated -w
#-DASSOC_DISSOC_EDGE_COPY_FLAG_SET
#-DDUP_CHECK_FLAG_SET 
#-DDUMMY_EDGE_FLAG 
#-DCOMPUTE_MINCUT_EDGES_FLAG
#-DONLY_INTERAC_EDGES_FOR_MINCUT
#-DUSE_FLOAT_EDGE_WEIGHTS_FOR_MINCUT
#-DOPT_GAP_HEUR_FLAG
#-DOPT_MIN_DEGREE_FLAG
#-DOPT_SIMPLE_PATH_FLAG
#-DASSERT_FLAG
#-DDEBUG_FLAG


#GPROF = true
#VALGRIND = true
ifdef GPROF
  CFLAGS += -pg
  OPTFLAGS = -pg
endif

ifdef VALGRIND
  CFLAGS += -g 
  OPTFLAGS += -g 
endif

DS_SUBDIRS = $(shell find $(DS_DIR) -path '$(DS_DIR)/*' -prune -type d)
DS_SOURCE = $(foreach dsdir, $(DS_SUBDIRS), $(wildcard $(dsdir)/*.cc))
#DS_OBJ = $(patsubst %.cc, %.o, $(DS_SOURCE))
DSFLAGS= $(foreach dsdir, $(DS_SUBDIRS), -I $(dsdir))

DM_DIR = $(DS_DIR)/DAGManager
EM_DIR = $(DS_DIR)/ExpressionManager
HM_DIR = $(DS_DIR)/HashTableManager
LM_DIR = $(DS_DIR)/LogManager
CO_DIR = $(DS_DIR)/Common
CONFIG_DIR = $(DS_DIR)/ConfigurationOptions

DS_OBJ = $(HM_DIR)/HashTable_WithStandardKeys.o $(EM_DIR)/ExpressionManager.o $(EM_DIR)/Expression.o $(EM_DIR)/InferTypeOfExpression.o $(EM_DIR)/ExpressionAsDAG.o $(EM_DIR)/ExpressionValue.o $(EM_DIR)/InvalidBitManager.o $(EM_DIR)/ExpressionEvaluator.o $(EM_DIR)/inferencerule_parser.tab.o $(EM_DIR)/inferencerule_parser.yy.o  $(DM_DIR)/AdvancedDAGManipulation.o $(DM_DIR)/DagManager.o $(DM_DIR)/DAGNode.o $(DM_DIR)/DAGSimplifier.o $(DM_DIR)/MatchingFunctions.o $(DM_DIR)/rules_parser.tab.o $(DM_DIR)/rules_parser.yy.o $(DM_DIR)/Strategies.o $(DM_DIR)/yacc_utils.o $(CO_DIR)/helping_functions.o $(LM_DIR)/LogManager.o  $(DM_DIR)/GC.o $(CONFIG_DIR)/ConfigurationOptions.o 

XMLFLAGS=-I/usr/include/libxml++-2.6 -I/usr/lib/x86_64-linux-gnu/libxml++-2.6/include -I/usr/include/libxml2 -I/usr/include/glibmm-2.4 -I/usr/lib/x86_64-linux-gnu/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib/x86_64-linux-gnu/sigc++-2.0/include -lxml++-2.6 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lglib-2.0 -lsigc-2.0

Z3PP_FLAGS=-I/usr/include -L/usr/lib -lz3

.PHONY: all clean ds_build

all: ds_build $(TARGET) 

ds_build: $(DS_SRC) $(DS_DIR)/Makefile.include $(foreach dsdir, $(DS_SUBDIRS), $(dsdir)/Makefile)
	$(foreach dsdir, $(DS_SUBDIRS), $(MAKE) -C $(dsdir) &&)\
	echo "***************************************" &&\
	echo "Build of data structure libraries done!" &&\
	echo "***************************************"

$(TARGET) : $(BUILD_DIR)/main.o  $(BUILD_DIR)/NewBV.o  $(BUILD_DIR)/Graph.o  $(BUILD_DIR)/GraphManager.o  $(BUILD_DIR)/GraphManagerNew.o $(BUILD_DIR)/XMLParser.o $(BUILD_DIR)/Z3Solver.o $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/rules_parser.tab.o $(BUILD_DIR)/rule_tuple_store.o $(DS_OBJ)
	$(CC) -g $(OPTFLAGS) $(BUILD_DIR)/main.o  $(BUILD_DIR)/NewBV.o  $(BUILD_DIR)/Graph.o  $(BUILD_DIR)/GraphManager.o  $(BUILD_DIR)/GraphManagerNew.o $(BUILD_DIR)/XMLParser.o  $(BUILD_DIR)/Z3Solver.o $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/rules_parser.tab.o $(BUILD_DIR)/rule_tuple_store.o $(DS_OBJ) $(XMLFLAGS) $(DSFLAGS) $(Z3PP_FLAGS) -o $(TARGET) &&\
	rm -f $(SOURCE_DIR)/rules_parser.tab.h

$(BUILD_DIR)/main.o: $(SOURCE_DIR)/main.cc  $(INCLUDE_DIR)/GraphManager.h $(INCLUDE_DIR)/GraphManagerNew.h $(INCLUDE_DIR)/XMLParser.h $(INCLUDE_DIR)/Z3Solver.h $(INCLUDE_DIR)/macros.h $(INCLUDE_DIR)/rules_parser_datatypes.h $(INCLUDE_DIR)/rules_parser.tab.h $(SOURCE_DIR)/rule_tuple_store.cc $(INCLUDE_DIR)/rule_tuple_store.h
	$(CC) -g -c $(CFLAGS) $(OPTFLAGS) $(SOURCE_DIR)/main.cc $(XMLFLAGS) $(DSFLAGS) $(Z3PP_FLAGS) -o $(BUILD_DIR)/main.o 

$(BUILD_DIR)/NewBV.o : $(SOURCE_DIR)/NewBV.cc $(INCLUDE_DIR)/Oprs.h
	$(CC) -g -c $(OPTFLAGS) $(SOURCE_DIR)/NewBV.cc -o $(BUILD_DIR)/NewBV.o 

$(BUILD_DIR)/Graph.o : $(SOURCE_DIR)/Graph.cc $(INCLUDE_DIR)/GraphManager.h
	$(CC) -g -c $(OPTFLAGS) $(SOURCE_DIR)/Graph.cc $(DSFLAGS) -o $(BUILD_DIR)/Graph.o 

$(BUILD_DIR)/GraphManager.o : $(SOURCE_DIR)/GraphManager.cc $(INCLUDE_DIR)/GraphManager.h $(INCLUDE_DIR)/XMLParser.h $(INCLUDE_DIR)/macros.h $(INCLUDE_DIR)/Oprs.h $(INCLUDE_DIR)/rules_parser_datatypes.h $(INCLUDE_DIR)/rules_parser.tab.h $(SUPPORTING_FILES_DIR)/edge_type.txt $(SUPPORTING_FILES_DIR)/edge_subtype.txt $(SUPPORTING_FILES_DIR)/node_type.txt $(SUPPORTING_FILES_DIR)/path.txt
	$(CC) -g $(CFLAGS) $(SOURCE_DIR)/GraphManager.cc $(XMLFLAGS) $(DSFLAGS) -o $(BUILD_DIR)/GraphManager.o

$(BUILD_DIR)/GraphManagerNew.o : $(SOURCE_DIR)/GraphManagerNew.cc $(INCLUDE_DIR)/GraphManagerNew.h $(INCLUDE_DIR)/XMLParser.h $(INCLUDE_DIR)/macros.h $(INCLUDE_DIR)/Oprs.h $(INCLUDE_DIR)/rules_parser_datatypes.h $(INCLUDE_DIR)/rules_parser.tab.h $(SUPPORTING_FILES_DIR)/edge_type.txt $(SUPPORTING_FILES_DIR)/edge_subtype.txt $(SUPPORTING_FILES_DIR)/node_type.txt $(SUPPORTING_FILES_DIR)/path.txt
	$(CC) -g $(CFLAGS) $(SOURCE_DIR)/GraphManagerNew.cc $(XMLFLAGS)  $(DSFLAGS) $(Z3PP_FLAGS) -o $(BUILD_DIR)/GraphManagerNew.o

$(BUILD_DIR)/XMLParser.o : $(SOURCE_DIR)/XMLParser.cc $(INCLUDE_DIR)/XMLParser.h 
	$(CC) -c -g $(CFLAGS) $(OPTFLAGS) $(SOURCE_DIR)/XMLParser.cc $(XMLFLAGS) $(DSFLAGS) -o $(BUILD_DIR)/XMLParser.o 

$(BUILD_DIR)/Z3Solver.o : $(SOURCE_DIR)/Z3Solver.cc $(INCLUDE_DIR)/Z3Solver.h 
	$(CC) -c -g $(CFLAGS) $(OPTFLAGS) $(SOURCE_DIR)/Z3Solver.cc $(XMLFLAGS) $(DSFLAGS) $(Z3PP_FLAGS) -o $(BUILD_DIR)/Z3Solver.o 

$(BUILD_DIR)/rule_tuple_store.o : $(SOURCE_DIR)/rule_tuple_store.cc $(INCLUDE_DIR)/rule_tuple_store.h 
	$(CC) -c -g $(OPTFLAGS) $(SOURCE_DIR)/rule_tuple_store.cc $(XMLFLAGS) $(DSFLAGS) -o $(BUILD_DIR)/rule_tuple_store.o 

$(SOURCE_DIR)/main.cc : $(INCLUDE_DIR)/rules_parser.tab.h

$(SOURCE_DIR)/GraphManager.cc : $(INCLUDE_DIR)/rules_parser.tab.h

$(INCLUDE_DIR)/GraphManager.h : $(INCLUDE_DIR)/rules_parser.tab.h

$(INCLUDE_DIR)/rule_tuple_store.h : $(INCLUDE_DIR)/GraphManager.h $(SOURCE_DIR)/GraphManager.cc $(INCLUDE_DIR)/rules_parser.tab.h

$(BUILD_DIR)/lex.yy.o : $(SOURCE_DIR)/lex.yy.c $(INCLUDE_DIR)/rules_parser_datatypes.h
	$(CC) -c -g $(OPTFLAGS) $(SOURCE_DIR)/lex.yy.c -o $(BUILD_DIR)/lex.yy.o

$(BUILD_DIR)/rules_parser.tab.o : $(SOURCE_DIR)/rules_parser.tab.c $(INCLUDE_DIR)/rules_parser.tab.h
	$(CC) -c -g $(OPTFLAGS) $(SOURCE_DIR)/rules_parser.tab.c -o $(BUILD_DIR)/rules_parser.tab.o

$(INCLUDE_DIR)/rules_parser.tab.h $(SOURCE_DIR)/rules_parser.tab.c: $(SOURCE_DIR)/rules_parser.y
	bison -d $(SOURCE_DIR)/rules_parser.y -o $(SOURCE_DIR)/rules_parser.tab.c  && cp $(SOURCE_DIR)/rules_parser.tab.h $(INCLUDE_DIR)/

$(SOURCE_DIR)/lex.yy.c : $(SOURCE_DIR)/rules_parser.l $(INCLUDE_DIR)/rules_parser.tab.h
	flex $(SOURCE_DIR)/rules_parser.l && mv lex.yy.c $(SOURCE_DIR)

clean:
	rm -rf $(BUILD_DIR)/*.o $(SOURCE_DIR)/*gch $(TARGET) $(SOURCE_DIR)/*.tab.* $(SOURCE_DIR)/*.yy.c $(INCLUDE_DIR)/*.tab.* $(INCLUDE_DIR)/*.yy.c &&\
	for dsdir in $(DS_SUBDIRS); do $(MAKE) -C $$dsdir clean; done &&\
	echo "********************" &&\
	echo "Cleaned the build..." &&\
	echo "********************"


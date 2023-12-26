CC = g++
SOURCE_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
TARGET = ./bin/network_tool
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
#flag -std=c++0x required for unordered_map included from GraphManagaeNew.cc

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

AB = ExpressionManager_Based_Implementation

DM_DIR = $(AB)/DAGManager
EM_DIR = $(AB)/ExpressionManager
HM_DIR = $(AB)/HashTableManager
LM_DIR = $(AB)/LogManager
CO_DIR = $(AB)/Common
CONFIG_DIR = $(AB)/ConfigurationOptions


#XMLFLAGS=-I/usr/include/glibmm-2.4 -I/usr/lib/x86_64-linux-gnu/glibmm-2.4/include -I/usr/include/sigc++-2.0 -I/usr/lib/x86_64-linux-gnu/sigc++-2.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/libxml2 -I/usr/include/libxml++-2.6 -I/usr/lib/libxml++-2.6/include  -lxml++-2.6 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lsigc-2.0 -lglib-2.0 -I/usr/include/glibmm-2.4 -I/usr/lib/x86_64-linux-gnu/glibmm-2.4/include -I/usr/include/sigc++-2.0 -I/usr/lib/x86_64-linux-gnu/sigc++-2.0/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/libxml++-2.6 -I/usr/lib/libxml++-2.6/include  -lxml++-2.6 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lsigc-2.0 -std=c++0x
XMLFLAGS=-I/usr/include/libxml++-2.6 -I/usr/lib/x86_64-linux-gnu/libxml++-2.6/include -I/usr/include/libxml2 -I/usr/include/glibmm-2.4 -I/usr/lib/x86_64-linux-gnu/glibmm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/sigc++-2.0 -I/usr/lib/x86_64-linux-gnu/sigc++-2.0/include -lxml++-2.6 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lglib-2.0 -lsigc-2.0
#XMLFLAGS=-I"C:\Users\JAHNABI ROY\Desktop\code\libxml++-3.0" -I"C:\Users\JAHNABI ROY\Desktop\code\libxml++-3.0\include" -I"C:\Users\JAHNABI ROY\Desktop\code\libxml2-2.9.3-win32-x86_64\include" -I"C:\Users\JAHNABI ROY\Desktop\code\glibmm-2.4" -I"C:\Users\JAHNABI ROY\Desktop\code\include" -I"C:\Users\JAHNABI ROY\Desktop\code\glib-2.0" -I"C:\Users\JAHNABI ROY\Desktop\code\include" -I"C:\Users\JAHNABI ROY\Desktop\code\sigc++-2.0" -I"C:\Users\JAHNABI ROY\Desktop\code\include" -lxml++-3.0 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lglib-2.0 -lsigc-2.0 --verbose
#XMLFLAGS=-I"C:\msys64\mingw64\include\libxml++-3.0" -I"C:\msys64\mingw64\lib\libxml++-3.0\include" -I"C:\msys64\mingw64\include\libxml2" -I"C:\msys64\mingw64\include\glibmm-2.4" -I"C:\msys64\mingw64\lib\glibmm-2.4\include" -I"C:\msys64\mingw64\include\glib-2.0" -I"C:\msys64\mingw64\lib\glib-2.0\include" -I"C:\msys64\mingw64\include\sigc++-2.0" -I"C:\msys64\mingw64\lib\sigc++-2.0\include" -lxml++-3.0 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lglib-2.0 -lsigc-2.0


#SBMLFLAGS=-I/home/sysbio/include -L/home/sysbio/lib -lsbml -lstdc++ -lm 

#`pkg-config --cflags --libs glibmm-2.4 libxml++-2.6`
#`pkg-config --cflags --libs libxml++-2.6`
#`pkg-config --cflags --libs libsbml`
#-I/usr/local/include -I/usr/include/libxml2  -L/usr/local/lib -lz -lsbml -lstdc++ -lm -lxml2

#FLAGS related to Expression Manager
EMFLAGS=-I $(DM_DIR) -I $(EM_DIR) -I $(HM_DIR) -I $(LM_DIR) -I $(CO_DIR) -I $(CONFIG_DIR)

#Z3PP_FLAGS=-I/home/sysbio/Z3-master/src/api -I/home/sysbio/Z3-master/src/api/c++ -L/home/sysbio/Z3-master/build/libz3.so -lz3
# Z3PP_FLAGS=-I/usr/include -I/home/sukanya/Downloads/z3-z3-4.7.1/src -L/usr/lib -lz3
#Z3PP_FLAGS=-I/usr/include -I"C:\Users\JAHNABI ROY\Desktop\code\z3 -L/usr/lib -lz3

Z3PP_FLAGS=-I/usr/include -L/usr/lib -lz3
#Z3PP_FLAGS=-I"C:\Users\JAHNABI ROY\Desktop\code\z3" -lz3

#SBMLFLAGS1=-I/usr/local/include -I/usr/include/libxml2
#SBMLFLAGS2-L/usr/local/lib -lz -lsbml -lstdc++ -lm -lxml2

all: $(TARGET) 
		
$(TARGET) : $(BUILD_DIR)/main.o  $(BUILD_DIR)/NewBV.o  $(BUILD_DIR)/Graph.o  $(BUILD_DIR)/GraphManager.o  $(BUILD_DIR)/GraphManagerNew.o $(BUILD_DIR)/XMLParser.o $(BUILD_DIR)/Z3Solver.o $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/rules_parser.tab.o $(BUILD_DIR)/rule_tuple_store.o
	$(CC) -g $(OPTFLAGS) $(BUILD_DIR)/main.o  $(BUILD_DIR)/NewBV.o  $(BUILD_DIR)/Graph.o  $(BUILD_DIR)/GraphManager.o  $(BUILD_DIR)/GraphManagerNew.o $(BUILD_DIR)/XMLParser.o  $(BUILD_DIR)/Z3Solver.o $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/rules_parser.tab.o $(BUILD_DIR)/rule_tuple_store.o $(HM_DIR)/HashTable_WithStandardKeys.o $(EM_DIR)/ExpressionManager.o $(EM_DIR)/Expression.o $(EM_DIR)/InferTypeOfExpression.o $(EM_DIR)/ExpressionAsDAG.o $(EM_DIR)/ExpressionValue.o $(EM_DIR)/InvalidBitManager.o $(EM_DIR)/ExpressionEvaluator.o $(EM_DIR)/inferencerule_parser.tab.o $(EM_DIR)/inferencerule_parser.yy.o  $(DM_DIR)/AdvancedDAGManipulation.o $(DM_DIR)/DagManager.o $(DM_DIR)/DAGNode.o $(DM_DIR)/DAGSimplifier.o $(DM_DIR)/MatchingFunctions.o $(DM_DIR)/rules_parser.tab.o $(DM_DIR)/rules_parser.yy.o $(DM_DIR)/Strategies.o $(DM_DIR)/yacc_utils.o $(CO_DIR)/helping_functions.o $(LM_DIR)/LogManager.o  $(DM_DIR)/GC.o $(CONFIG_DIR)/ConfigurationOptions.o $(XMLFLAGS) $(EMFLAGS) $(Z3PP_FLAGS) -o $(TARGET) && rm $(SOURCE_DIR)/rules_parser.tab.h

#$(BUILD_DIR)/main.o: $(SOURCE_DIR)/main.cc  $(SOURCE_DIR)/Graph.cc  $(SOURCE_DIR)/GraphManager.cc $(SOURCE_DIR)/XMLParser.cc template_scanner.tab.c lex.yy.c
#	$(CC) -g -c  $(SOURCE_DIR)/main.cc template_scanner.tab.c lex.yy.c $(XMLFLAGS) #$(SBMLFLAGS) 

$(BUILD_DIR)/main.o: $(SOURCE_DIR)/main.cc  $(INCLUDE_DIR)/GraphManager.h $(INCLUDE_DIR)/GraphManagerNew.h $(INCLUDE_DIR)/XMLParser.h $(INCLUDE_DIR)/Z3Solver.h $(INCLUDE_DIR)/macros.h $(INCLUDE_DIR)/rules_parser_datatypes.h $(INCLUDE_DIR)/rules_parser.tab.h $(SOURCE_DIR)/rule_tuple_store.cc $(INCLUDE_DIR)/rule_tuple_store.h
	$(CC) -g -c $(CFLAGS) $(OPTFLAGS) $(SOURCE_DIR)/main.cc $(XMLFLAGS) $(EMFLAGS) $(Z3PP_FLAGS) -o $(BUILD_DIR)/main.o 

$(BUILD_DIR)/NewBV.o : $(SOURCE_DIR)/NewBV.cc $(INCLUDE_DIR)/Oprs.h
	$(CC) -g -c $(OPTFLAGS) $(SOURCE_DIR)/NewBV.cc -o $(BUILD_DIR)/NewBV.o 

$(BUILD_DIR)/Graph.o : $(SOURCE_DIR)/Graph.cc $(INCLUDE_DIR)/GraphManager.h
	$(CC) -g -c $(OPTFLAGS) $(SOURCE_DIR)/Graph.cc $(EMFLAGS) -o $(BUILD_DIR)/Graph.o 

$(BUILD_DIR)/GraphManager.o : $(SOURCE_DIR)/GraphManager.cc $(INCLUDE_DIR)/GraphManager.h $(INCLUDE_DIR)/XMLParser.h $(INCLUDE_DIR)/macros.h $(INCLUDE_DIR)/Oprs.h $(INCLUDE_DIR)/rules_parser_datatypes.h $(INCLUDE_DIR)/rules_parser.tab.h $(SUPPORTING_FILES_DIR)/edge_type.txt $(SUPPORTING_FILES_DIR)/edge_subtype.txt $(SUPPORTING_FILES_DIR)/node_type.txt $(SUPPORTING_FILES_DIR)/path.txt
	$(CC) -g $(CFLAGS) $(SOURCE_DIR)/GraphManager.cc $(XMLFLAGS) $(EMFLAGS) -o $(BUILD_DIR)/GraphManager.o

$(BUILD_DIR)/GraphManagerNew.o : $(SOURCE_DIR)/GraphManagerNew.cc $(INCLUDE_DIR)/GraphManagerNew.h $(INCLUDE_DIR)/XMLParser.h $(INCLUDE_DIR)/macros.h $(INCLUDE_DIR)/Oprs.h $(INCLUDE_DIR)/rules_parser_datatypes.h $(INCLUDE_DIR)/rules_parser.tab.h $(SUPPORTING_FILES_DIR)/edge_type.txt $(SUPPORTING_FILES_DIR)/edge_subtype.txt $(SUPPORTING_FILES_DIR)/node_type.txt $(SUPPORTING_FILES_DIR)/path.txt
	$(CC) -g $(CFLAGS) $(SOURCE_DIR)/GraphManagerNew.cc $(XMLFLAGS)  $(EMFLAGS) $(Z3PP_FLAGS) -o $(BUILD_DIR)/GraphManagerNew.o
 
$(BUILD_DIR)/XMLParser.o : $(SOURCE_DIR)/XMLParser.cc $(INCLUDE_DIR)/XMLParser.h 
	$(CC) -c -g $(CFLAGS) $(OPTFLAGS) $(SOURCE_DIR)/XMLParser.cc $(XMLFLAGS) $(EMFLAGS) -o $(BUILD_DIR)/XMLParser.o 

$(BUILD_DIR)/Z3Solver.o : $(SOURCE_DIR)/Z3Solver.cc $(INCLUDE_DIR)/Z3Solver.h 
	$(CC) -c -g $(CFLAGS) $(OPTFLAGS) $(SOURCE_DIR)/Z3Solver.cc $(XMLFLAGS) $(EMFLAGS) $(Z3PP_FLAGS) -o $(BUILD_DIR)/Z3Solver.o 
	
$(BUILD_DIR)/rule_tuple_store.o : $(SOURCE_DIR)/rule_tuple_store.cc $(INCLUDE_DIR)/rule_tuple_store.h 
	$(CC) -c -g $(OPTFLAGS) $(SOURCE_DIR)/rule_tuple_store.cc $(XMLFLAGS) $(EMFLAGS) -o $(BUILD_DIR)/rule_tuple_store.o 

#$(BUILD_DIR)/Rules.o : $(SOURCE_DIR)/Rules.cc $(INCLUDE_DIR)/Rules.h
#	$(CC) -c -g $(SOURCE_DIR)/Rules.cc -o $(BUILD_DIR)/Rules.o

#$(SOURCE_DIR)/main.cc : $(INCLUDE_DIR)/template_scanner.tab.h
$(SOURCE_DIR)/main.cc : $(INCLUDE_DIR)/rules_parser.tab.h

#$(SOURCE_DIR)/GraphManager.cc : $(INCLUDE_DIR)/template_scanner.tab.h
$(SOURCE_DIR)/GraphManager.cc : $(INCLUDE_DIR)/rules_parser.tab.h

#$(INCLUDE_DIR)/GraphManager.h : $(INCLUDE_DIR)/template_scanner.tab.h
$(INCLUDE_DIR)/GraphManager.h : $(INCLUDE_DIR)/rules_parser.tab.h

$(INCLUDE_DIR)/rule_tuple_store.h : $(INCLUDE_DIR)/GraphManager.h $(SOURCE_DIR)/GraphManager.cc $(INCLUDE_DIR)/rules_parser.tab.h

#$(BUILD_DIR)/lex.yy.o : $(SOURCE_DIR)/lex.yy.c $(INCLUDE_DIR)/template_datatypes.h
#	$(CC) -c $(SOURCE_DIR)/lex.yy.c -o $(BUILD_DIR)/lex.yy.o

$(BUILD_DIR)/lex.yy.o : $(SOURCE_DIR)/lex.yy.c $(INCLUDE_DIR)/rules_parser_datatypes.h
	$(CC) -c -g $(OPTFLAGS) $(SOURCE_DIR)/lex.yy.c -o $(BUILD_DIR)/lex.yy.o

#$(BUILD_DIR)/template_scanner.tab.o : $(SOURCE_DIR)/template_scanner.tab.c $(INCLUDE_DIR)/template_scanner.tab.h
#	$(CC) -c $(SOURCE_DIR)/template_scanner.tab.c -o $(BUILD_DIR)/template_scanner.tab.o

$(BUILD_DIR)/rules_parser.tab.o : $(SOURCE_DIR)/rules_parser.tab.c $(INCLUDE_DIR)/rules_parser.tab.h
	$(CC) -c -g $(OPTFLAGS) $(SOURCE_DIR)/rules_parser.tab.c -o $(BUILD_DIR)/rules_parser.tab.o

#$(INCLUDE_DIR)/template_scanner.tab.h $(SOURCE_DIR)/template_scanner.tab.c: $(SOURCE_DIR)/template_scanner.y
#	bison -d $(SOURCE_DIR)/template_scanner.y -o $(SOURCE_DIR)/template_scanner.tab.c && mv $(SOURCE_DIR)/template_scanner.tab.h $(INCLUDE_DIR)/

$(INCLUDE_DIR)/rules_parser.tab.h $(SOURCE_DIR)/rules_parser.tab.c: $(SOURCE_DIR)/rules_parser.y
	bison -d $(SOURCE_DIR)/rules_parser.y -o $(SOURCE_DIR)/rules_parser.tab.c  && cp $(SOURCE_DIR)/rules_parser.tab.h $(INCLUDE_DIR)/

#$(SOURCE_DIR)/lex.yy.c : $(SOURCE_DIR)/template_scanner.l $(INCLUDE_DIR)/template_scanner.tab.h
#	flex $(SOURCE_DIR)/template_scanner.l && mv lex.yy.c $(SOURCE_DIR)

$(SOURCE_DIR)/lex.yy.c : $(SOURCE_DIR)/rules_parser.l $(INCLUDE_DIR)/rules_parser.tab.h
	flex $(SOURCE_DIR)/rules_parser.l && mv lex.yy.c $(SOURCE_DIR)

clean:
	rm -rf $(BUILD_DIR)/*.o $(SOURCE_DIR)/*gch $(TARGET) $(SOURCE_DIR)/*.tab.* $(SOURCE_DIR)/*.yy.c $(INCLUDE_DIR)/*.tab.* $(INCLUDE_DIR)/*.yy.c

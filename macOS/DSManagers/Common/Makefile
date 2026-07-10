SS = ..
include $(SS)/Makefile.include 

ifndef QE #LIRA is required in this case 
	V64=1
	#include $(LIRA)/ConfigFiles/lira.mak
	#LOADLIBES += $(LIRA_DEPENDENT_LIBS)
endif


build: $(HELPING_FUNCTIONS)_LIRA.o $(HELPING_FUNCTIONS).o 

${HELPING_FUNCTIONS}.o : ${HELPING_FUNCTIONS}.cc
	${CC} ${CFLAGS} ${HELPING_FUNCTIONS}.cc ${HELPING_FUNCTIONS}.h -I $(COMMAND_OPTIONS_DIR)

${HELPING_FUNCTIONS}_LIRA.o : ${HELPING_FUNCTIONS}.cc
	${CC} $(CFLAGS) ${HELPING_FUNCTIONS}.cc ${HELPING_FUNCTIONS}.h  $(INCLUDE)
	mv $(HELPING_FUNCTIONS).o $(HELPING_FUNCTIONS)_LIRA.o
	rm -f $(HELPING_FUNCTIONS).h.gch $(HELPING_FUNCTIONS).h.pch



clean:
	rm -f *.o *.gch *.pch
distclean:
	make clean; 
	rm -f gmon.out *~ tags .gdb_history *.log *.LOG *.Log

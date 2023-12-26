SS = ..
include $(SS)/Makefile.include

build: $(HASHTABLE_MANAGER).o

$(HASHTABLE_MANAGER).o : $(HASHTABLE_MANAGER).cc
	$(CC) $(CFLAGS)  $(HASHTABLE_MANAGER).cc $(INCLUDE)

clean:
	rm -f *.o *.gch

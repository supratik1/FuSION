SS = ..
include $(SS)/Makefile.include

build: $(LOGMANAGER).o

$(LOGMANAGER).o : $(LOGMANAGER).cc
	$(CC) $(CFLAGS)  $(LOGMANAGER).cc -I $(COMMON_DIR)

clean:
	rm -f *.o *.gch

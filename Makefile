CC =clang20
#CC =cc
CFLAGS=-std=c11 -O3 -Wall -Werror -Wextra -pedantic
CFLAGS_DEBUG=-std=c11 -g3 -Wall -Werror -Wextra -pedantic

RM=rm -f
MK=mkdir -p $(OUTPUT_DIR)

PROJECT_O=torg.o
PROJECT_B=torg
OUTPUT_DIR=bin
OBJECTS=torg.o
SOURCES=torg.c

VERSION=0.0.0
OUTPUT=$(PROJECT_B)

-include Makefile.depend

all: build 

build: depend PROJECT_O PROJECT_B 

debug:
		$(MK)
		$(CC)  -c $(SOURCES) $(CFLAGS_DEBUG)
		$(CC) -o $(OUTPUT_DIR)/$(PROJECT_B) $(OBJECTS) $(LIBS)

PROJECT_O:
		$(CC) -c $(SOURCES) $(CFLAGS)  

PROJECT_B:
		$(MK)
		$(CC) -o $(OUTPUT_DIR)/$(PROJECT_B) $(OBJECTS) $(LIBS)


clean:
		$(RM)  *.o Makefile.depend *.core *.bin bin/*

depend:
		cc -E -MM $(SOURCES) $(INCLUDE_DIR) > Makefile.depend

install: all
		install $(OUTPUT_DIR)/$(PROJECT_B) /usr/local/bin


deinstall:
		$(RM) /usr/local/bin/$(OUTPUT)


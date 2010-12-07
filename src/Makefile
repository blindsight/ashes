.PHONY: clean

FLAGS = -Wall -pedantic -std=c99
OBJS = user.o ashes.o
BUILD_DIR = ../bin
INCLUDES = include
DEBUG_FLAGS =-O0 -g
DEBUG_BIN = debug_ashes
OBJ_DEBUG = 
BIN = ashes

all: ${OBJS}
	gcc ${OBJS} ${OBJ_DEBUG} -o ${BUILD_DIR}/${BIN}

debug: FLAGS +=${DEBUG_FLAGS}
debug: BIN=${DEBUG_BIN}
debug: OBJ_DEBUG=${DEBUG_FLAGS}
debug: all

ashes.o: ashes.c
	gcc ${FLAGS} -I${INCLUDES} -c ashes.c

user.o: user.c
	gcc ${FLAGS} -I${INCLUDES} -c user.c
clean:
	rm $(BUILD_DIR)/ashes
	rm *.o

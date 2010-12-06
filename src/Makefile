.PHONY: clean

FLAGS = -Wall -pedantic -std=c99
OBJS = user.o ashes.o
BUILD_DIR = ../bin
INCLUDES = include

all: ashes.o user.o
	gcc ashes.o user.o -o ${BUILD_DIR}/ashes
	rm *.o

ashes.o: ashes.c
	gcc ${FLAGS} -I${INCLUDES} -c ashes.c

user.o: user.c
	gcc ${FLAGS} -I${INCLUDES} -c user.c
clean:
	rm $(BUILD_DIR)/ashes

cleanobj:
	rm *.o

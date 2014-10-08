CC = g++
# CFLAGS = -O3 -ggdb -Wall -ansi
CFLAGS =


.SUFFIXS: .cpp

default: p2a

p2a: p2a.o
	${CC} p2a.o ${CFLAGS} -o p2a

p2b: p2b.o
	${CC} p2b.o ${CFLAGS} -o p2b

p2c: p2c.o
	${CC} p2c.o ${CFLAGS} -o p2c

%.o: %.cpp
	${CC} $< ${CFLAGS} -c

.PHONY: clean
clean:
	@rm -rf *.o

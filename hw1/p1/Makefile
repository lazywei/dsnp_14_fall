CC = g++
CFLAGS = -O3 -ggdb -Wall -ansi


.SUFFIXS: .cpp

default: p1a

p1a: p1a.main.o p1a.o
	${CC} p1a.main.o p1a.o ${CFLAGS} -o p1a.main

p1b: p1b.main.o p1b.o
	${CC} p1b.main.o p1b.o ${CFLAGS} -o p1b.main

bug: p1.bug.o p1a.o
	${CC} p1.bug.o p1a.o ${CFLAGS} -o p1.bug

%.o: %.cpp
	${CC} $< ${CFLAGS} -c

.PHONY: clean
clean:
	@rm -rf *.o

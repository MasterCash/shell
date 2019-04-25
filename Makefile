# Makefile

EXE = shell

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
HEADS = $(wildcard *.hpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

CPP = g++
CFLAGS = -Wall --pedantic-errors -g -std=c++11

program: ${OBJECTS}
	-@${CPP} ${CFLAGS} ${OBJECTS} -o ${EXE}
	-@echo Compliation Complete

%.o: %.cpp ${HEADERS} ${HEADS}
	-@${CPP} ${CFLAGS} -c $<

.PHONY: clean
clean:
	-@rm -f ${EXE}
	-@rm -f ${OBJECTS}
run:
	-@./${EXE}
val:
	-@valgrind ./${EXE}

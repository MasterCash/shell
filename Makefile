# Makefile

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
HEADS = $(wildcard *.hpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

CPP = g++
CFLAGS = -Wall --pedantic-errors -g -std=c++11

program: ${OBJECTS}
	-@${CPP} ${CFLAGS} ${OBJECTS} -o shell
	-@echo Compliation Complete

%.o: %.cpp ${HEADERS} ${HEADS}
	-@${CPP} ${CFLAGS} -c $<

.PHONY: clean
clean:
	-@rm -f program
	-@rm -f ${OBJECTS}
run:
	-@./shell
val:
	-@valgrind ./shell
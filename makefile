OBJS=newtest.o matrices.o utils.o glObject.o
CC=g++
CFLAGS=-std=c++0x -c
LFLAGS=-lGL -lGLEW -lSDL

newtest : $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -onewtest

newtest.o : newtest.cpp utils.h matrices.h glObject.h
	$(CC) $(CFLAGS) newtest.cpp

glObject.o : glObject.cpp glObject.h utils.h matrices.h
	$(CC) $(CFLAGS) glObject.cpp

utils.o : utils.cpp utils.h 
	$(CC) $(CFLAGS) utils.cpp

matrices.o : matrices.cpp matrices.h 
	$(CC) $(CFLAGS) matrices.cpp

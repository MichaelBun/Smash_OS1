# Makefile for the smash program
CC = g++
CFLAGS = -g -Wall -Wno-write-strings
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)
# Creating the object files
commands.o: commands.cpp commands.hpp
smash.o: smash.cpp commands.hpp
signals.o: signals.cpp signals.hpp
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*


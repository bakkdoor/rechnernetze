
CC		= gcc
CPPFLAGS	= -Wall -g -c

PROGNAME	= bin/person_list
OBJECTS		= src/mystrings.o src/person_list.o

default: $(PROGNAME)

$(PROGNAME): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

%.o: %.c %.h
	$(CC) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf src/*.o *~ $(PROGNAME) 2> /dev/null

run: $(PROGNAME)
	./$(PROGNAME)

vallgrind: $(PROGNAME)
	valgrind --tool=memcheck --leak-check=yes ./$(PROGNAME)

CC		= gcc
CPPFLAGS	= -Wall -g -c

PROGNAME	= bin/person_list
OBJECTS		= src/aufgabe1/list.o src/aufgabe1/aufgabe1.o src/aufgabe1/mystring.o

default: $(PROGNAME)

neu: $(OBJECTS_NEU)
	$(CC) -o bin/person_list_neu $(OBJECTS_NEU)

$(NEU): $(OBJECTS_NEU)
	$(CC) -o $@ $(OBJECTS_NEU)

$(PROGNAME): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

%.o: %.c %.h
	$(CC) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf src/**/*.o *~ $(PROGNAME) $(NEU) 2> /dev/null

run: $(PROGNAME)
	./$(PROGNAME)

vallgrind: $(PROGNAME)
	valgrind --tool=memcheck --leak-check=yes ./$(PROGNAME)


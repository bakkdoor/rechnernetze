
CC							= gcc
CPPFLAGS				= -Wall -g -c

CLIENT					= bin/client
SERVER					= bin/server
CLIENT_SRC			= src/client/client.o
SERVER_SRC			= src/server/server.o
COMMON					= src/common/unp_readline.o

default: $(CLIENT) $(SERVER)

client: $(CLIENT)

server: $(SERVER)

$(CLIENT): $(COMMON) $(CLIENT_SRC)
	$(CC) -o $@ $(COMMON) $(CLIENT_SRC)

$(SERVER): $(COMMON) $(SERVER_SRC)
	$(CC) -o $@ $(COMMON) $(SERVER_SRC)

%.o: %.c %.h
	$(CC) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf src/**/*.o *~ $(CLIENT) $(SERVER) 2> /dev/null

run_client: $(CLIENT)
	./$(CLIENT)

run_server: $(SERVER)
	./$(SERVER)

vallgrind_client: $(CLIENT)
	valgrind --tool=memcheck --leak-check=yes ./$(CLIENT)

vallgrind_server: $(SERVER)
	valgrind --tool=memcheck --leak-check=yes ./$(SERVER)


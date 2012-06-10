
CC							= gcc
CPPFLAGS				= -Wall -g -c -I src/

CLIENT					= bin/client
SERVER					= bin/server
CLIENT_SRC			= src/client/client.o src/client/connection.o
SERVER_SRC			= src/server/server.o src/server/connection.o src/server/client.o src/server/chat_room.o
COMMON					= src/common/unp_readline.o src/common/list.o src/common/messages.o src/common/output.o

PORT	= 1234
ADDR	= localhost
NAME	= user

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
	./$(CLIENT) $(ADDR) $(PORT) $(NAME)

run_client2: $(CLIENT)
	./$(CLIENT) $(ADDR) $(PORT) $(NAME)2

run_server: $(SERVER)
	./$(SERVER) $(PORT)

vallgrind_client: $(CLIENT)
	valgrind --tool=memcheck --leak-check=yes ./$(CLIENT) $(ADDR) $(PORT) $(NAME)

vallgrind_server: $(SERVER)
	valgrind --tool=memcheck --leak-check=yes ./$(SERVER) $(PORT)
	
	
SRC_DIR="pa_01"
TARGET_DIR="release_pa_01"

release:
	rm ../ReNe_SoSe_2012_PA1_933230_933151.tar
	cd .. && cp -rf ${SRC_DIR} ${TARGET_DIR} && cd ${TARGET_DIR} && \
	rm -rf `find . -name *git* ` && \
	rm -rf `find . -name nbproject* ` && \
	make && \
	tar -cvf ../ReNe_SoSe_2012_PA1_933230_933151.tar . -R && \
	cd .. && rm -rf ${TARGET_DIR}
	

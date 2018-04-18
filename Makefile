CC = gcc
CFLAGS = -I. -lpthread
DEPS = server.h
OBJ = server.o utilities.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o server server.o utilities.o $(CFLAGS)

clean:
	rm -f $(OBJ) server

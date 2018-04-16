CC = gcc
CFLAGS = -I. -Wall -lpthread
DEPS = server.h
OBJ = server.o
EXE = server


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) $(EXE)

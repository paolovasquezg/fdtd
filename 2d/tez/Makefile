CC = gcc
CFLAGS = -Wall -O2
LIBS = -lm

SRC = tezdemo.c gridtezpec.c updatetez.c ricker.c snapshot2d.c tfsftez.c abctez.c grid1dhz.c
OBJ = $(SRC:.c=.o)

tezdemo: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o tezdemo sim.*

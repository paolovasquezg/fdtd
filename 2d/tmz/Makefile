CC = gcc
CFLAGS = -Wall -O2
LIBS = -lm

SRC = tmzdemo2.c gridtmz.c updatetmz.c ricker.c snapshot2d.c tfsftmz.c abctmz.c grid1dez.c
OBJ = $(SRC:.c=.o)

tmzdemo2: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o tmzdemo2 sim.*

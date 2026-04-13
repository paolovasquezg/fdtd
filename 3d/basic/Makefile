CC     = gcc
CFLAGS = -Wall -O2
LIBS   = -lm

SRC = 3ddemo.c grid3dhomo.c update3d.c abc3dfirst.c ricker.c snapshot3d.c
OBJ = $(SRC:.c=.o)

fdtd3d: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o fdtd3d sim-x.* sim-y.* sim-z.*

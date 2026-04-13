CC     = gcc
CFLAGS = -Wall -O2
LIBS   = -lm

SRC = 3d-tfsf-demo.c grid3dsphere.c grid1dez.c \
      update3d.c tfsf-3d-ez.c abc3dfirst.c ricker.c snapshot3d.c
OBJ = $(SRC:.c=.o)

fdtd3d-tfsf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o fdtd3d-tfsf snap-x.* snap-y.*

OBJS = binfmt.o formats.o

LIBS = -lelf
CFLAGS = -g3 -O0

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $< 

binfmt: $(OBJS)
	gcc -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm -rf *.o binfmt

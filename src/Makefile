CFLAGS := -g3 -O0

create_patch_OBJS = create_patch.o generate_patch_data.o

%.o: %.c
	gcc $< -c $(CFLAGS) -o $@

create_patch: $(create_patch_OBJS)
	gcc $^ -o $@ $(CFLAGS)

clean:
	rm -rf *.o create_patch

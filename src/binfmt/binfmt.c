#include <stdio.h>
#include <malloc.h>
#include <libelf.h>
#include <gelf.h>
#include <fcntl.h>
#include "formats.h"

static void usage(char *argv0) {
	printf("Usage: %s <binary file>\n", argv0);
}

/*
 * @pre fd is a file descriptor pointing to beginning of file
 */
static execformat_t *find_file_type(int file_fd) {

	GElf_Ehdr elfheader;
	Elf *elf;

	elf = elf_begin(file_fd, ELF_C_READ, NULL);
	if ( gelf_getehdr(elf, &elfheader) ) {
		// ELF file
		int elf_class = (elfheader.e_ident[EI_CLASS] == ELFCLASS32) ? 
			BINFORMAT_FMT_ELF32 : BINFORMAT_FMT_ELF64;

		return find_format(elf_class);
	}

	// TODO: PE format

	return NULL; // XXX
}

int main(int argc, char **argv) {
	Elf *myelf;

	if ( argc < 2 ) {
		usage(argv[0]);
		return 0;
	}

	if ( elf_version(EV_CURRENT) == EV_NONE ) { // YOU ARE AWFUL FUCKING HUMAN BEINGS
		fprintf(stderr, "ELF Library too old\n");
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);
	if ( fd == -1 ) {
		perror("open");
		return 1;
	}

	execformat_t *mytype = find_file_type(fd);
	if ( mytype ) {
		printf("type: %s\n", mytype->desc);
	} else {
		printf("Unknown file format\n");
	}

	return 0;
}

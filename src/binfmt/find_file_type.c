/*
 * @pre fd is a file descriptor pointing to beginning of file
 */
execformat_t *find_file_type(int file_fd) {

	GElf_Ehdr elfheader;
	Elf *elf;

	// if this fails, elf_begin will fail right afterward anyway
	elf_version(EV_CURRENT);

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

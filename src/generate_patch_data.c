#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "binfmt/formats.h"
#include "generate_patch_data.h"

// cheesy-ass linked list setup
static diffstate_t *create_patch_point(diffstate_t *end,
	size_t diffstart, size_t diffsize,
	unsigned char *buf_orig, unsigned char *buf_ptch) {

	diffstate_t *newstate = (diffstate_t*)malloc(sizeof(diffstate_t));
	newstate->pos = diffstart;
	newstate->size = diffsize;

	newstate->before = (unsigned char*)malloc(sizeof(unsigned char)*diffsize);
	newstate->after = (unsigned char*)malloc(sizeof(unsigned char)*diffsize);
	memcpy(newstate->before, buf_orig+diffstart, diffsize);
	memcpy(newstate->after, buf_ptch+diffstart, diffsize);

	end->next = newstate;
	return newstate;
}

void free_patch_point(diffstate_t *diff) {
	free(diff->before);
	free(diff->after);
	free(diff);
}

int generate_patch_data(char *orig_file, 
	char *patched_file, 
	diffstate_t **diffs) 
{
	struct stat st_orig, st_ptch;

	FILE *fd_orig = fopen(orig_file, "r");
	if ( fd_orig == NULL ) {
		fprintf(stderr, "could not open orig file: %s\n", strerror(errno));
		return -1;
	}
	FILE *fd_ptch = fopen(patched_file, "r");
	if ( fd_ptch == NULL ) {
		fclose(fd_orig);
		fprintf(stderr, "could not open patched file: %s\n", strerror(errno));
		return -1;
	}

	if ( fstat(fileno(fd_orig), &st_orig) != 0 ) {
		fclose(fd_orig); fclose(fd_ptch);
		fprintf(stderr, "could not stat orig file: %s\n", strerror(errno));
		return -1;
	}
	if ( fstat(fileno(fd_ptch), &st_ptch) != 0 ) {
		fclose(fd_orig); fclose(fd_ptch);
		fprintf(stderr, "could not stat patched file: %s\n", strerror(errno));
		return -1;
	}

	if ( st_orig.st_size != st_ptch.st_size ) {
		fclose(fd_orig); fclose(fd_ptch);
		fprintf(stderr, "Orig and patched files are different size\n");
		return -1;
	}

	diffstate_t *states_head = (diffstate_t*)malloc(sizeof(diffstate_t)),
		*states_end = states_head;

	{
		char *buf_orig, *buf_ptch;
		int diff_state = 0; // 0 = normal, 1 = diff mode
		size_t diff_pos_start = 0, diff_size = 0;
		size_t total = 0;
		int i;

		buf_orig = (char*)malloc(sizeof(char)*st_orig.st_size);
		buf_ptch = (char*)malloc(sizeof(char)*st_orig.st_size);
		fread(buf_orig, 1, st_orig.st_size, fd_orig);
		fread(buf_ptch, 1, st_orig.st_size, fd_ptch);
		fclose(fd_orig);
		fclose(fd_ptch);

		for( total = 0; total < st_orig.st_size; total++ ) {
			if ( buf_orig[total] != buf_ptch[total] ) {
				if ( diff_pos_start == 0 ) {
					diff_pos_start = total;
					diff_size = 1;
				} else {
					++diff_size;
				}
			} else if ( diff_pos_start > 0 ) {
				states_end = create_patch_point(states_end, diff_pos_start, diff_size, buf_orig, buf_ptch);
				diff_pos_start = 0;
				diff_size = 0;
			}
		}

		free(buf_orig);
		free(buf_ptch);
	}
	
	*diffs = states_head;
	return 0;
}

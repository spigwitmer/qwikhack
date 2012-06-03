#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "binfmt/formats.h"

typedef struct diffstate_t
{
	unsigned char *before;
	unsigned char *after;
	size_t pos;
	size_t size;
	struct diffstate_t *next;
}
diffstate_t;

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

int generate_patch_data(char *orig_file, 
	char *patched_file, 
	char *out_file) 
{
	struct stat st_orig, st_ptch;

	int fd_orig = open(orig_file, O_RDONLY);
	if ( fd_orig < 0 ) {
		fprintf(stderr, "could not open orig file: %s\n", strerror(errno));
		return -1;
	}
	int fd_ptch = open(patched_file, O_RDONLY);
	if ( fd_ptch < 0 ) {
		fprintf(stderr, "could not open patched file: %s\n", strerror(errno));
		return -1;
	}

	if ( fstat(fd_orig, &st_orig) != 0 ) {
		fprintf(stderr, "could not stat orig file: %s\n", strerror(errno));
		return -1;
	}
	if ( fstat(fd_ptch, &st_ptch) != 0 ) {
		fprintf(stderr, "could not stat patched file: %s\n", strerror(errno));
		return -1;
	}

	if ( st_orig.st_size != st_ptch.st_size ) {
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

		read(fd_orig, buf_orig, st_orig.st_size);
		read(fd_ptch, buf_ptch, st_orig.st_size);

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
	
	FILE *out_f = fopen(out_file, "w");
	if ( out_f == NULL ) {
		fprintf(stderr, "Could not open destination file: %s\n", strerror(errno));
		return -1;
	}

	diffstate_t *iter;
	for ( iter = states_head; iter; iter = iter->next ) {
		if ( iter->size ) {
			int i = 0;
			fprintf(out_f, "hex %lx ", iter->pos);
			for(i; i < iter->size; ++i) {
				fprintf(out_f, "%02x", iter->after[i]);
			}
			fputc('\n', out_f);
		}
	}

	return 0;
}

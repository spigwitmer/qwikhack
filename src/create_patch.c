#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include "binfmt/formats.h"
#include "generate_patch_data.h"

void usage(char *argv0) {
	printf("Usage: %s <orig binary> <patched binary> <patch file>\n", argv0);
}

int main(int argc, char **argv) {
	char *orig_file, *patched_file;
	char *pdest_file, *patch_data;
	int c = 0, data_size = 0;

	struct option getopt_kids[] = {
		{ "help", no_argument, NULL, 'h'}
	};

	while ((c = getopt_long(argc, argv, "h", getopt_kids, NULL)) != -1) {
		switch(c) {
			case 0:
				break;

			case 'h':
				usage(argv[0]);
				return EXIT_SUCCESS;

			default:
				fprintf(stderr, "Unknown option %s\n", optarg);
				return EXIT_FAILURE;
		}
	}

	if ( argc - optind < 3 ) {
		fprintf(stderr, "Missing orig and patched files\n");
		usage(argv[0]);
		return EXIT_SUCCESS;
	}

	orig_file = argv[optind++];
	patched_file = argv[optind++];
	pdest_file = argv[optind];

	diffstate_t *diffs;

	c = generate_patch_data(orig_file, patched_file, &diffs);
	if ( c != 0 ) {
		fprintf(stderr, "Failed to generate patch\n");
		return EXIT_FAILURE;
	}

	FILE *out_f;
	if ( strlen(pdest_file) == 1 && pdest_file[0] == '-' )
		out_f = stdout;
	else
		out_f = fopen(pdest_file, "wb");

	if ( out_f == NULL ) {
		fprintf(stderr, "Could not open dest file: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}

	diffstate_t *iter, *next_iter;
	for( iter = diffs; iter; iter = next_iter ) {
		if ( iter->size ) {
			int i = 0;
			fprintf(out_f, "hex %lx ", iter->pos);
			while( i < iter->size )
				fprintf(out_f, "%02x", iter->after[i++]);
			fputc('\n', out_f);
		}
		next_iter = iter->next;
		free(iter);
	}
	fclose(out_f);

	return EXIT_SUCCESS;
}

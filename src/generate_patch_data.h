typedef struct diffstate_t
{
	unsigned char *before;
	unsigned char *after;
	size_t pos;
	size_t size;
	struct diffstate_t *next;
}
diffstate_t;

int generate_patch_data(char *orig_file, 
	char *patched_file, diffstate_t **diffs);

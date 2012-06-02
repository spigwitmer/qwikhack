#define BINFORMAT_FMT_ELF32 0
#define BINFORMAT_FMT_ELF64 1

#define BINFORMAT_ENDIAN_LITTLE 0
#define BINFORMAT_ENDIAN_BIG 1

typedef struct {
	int code;
	int addr_size;
	char *desc;
} execformat_t;

extern execformat_t g_formats[];
execformat_t *find_format(int format_type);

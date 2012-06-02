#include "formats.h"

execformat_t g_formats[] = {
	{
		BINFORMAT_FMT_ELF32,
		4,
		"ELF (32-bit)"
	},
	{
		BINFORMAT_FMT_ELF64,
		8,
		"ELF (64-bit)"
	}
};

execformat_t *find_format(int format_type) {
	execformat_t *ef;
	for(ef = g_formats; ef; ef++) {
		if (ef->code == format_type)
			return ef;
	}
	return (execformat_t*)0;
}

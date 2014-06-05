// From http://www.codeguru.com/forum/archive/index.php/t-378333.html
#include "jpeglib.h"
/*
This a custom destination manager for jpeglib that
enables the use of memory to memory compression.
*/
typedef struct {
	struct jpeg_destination_mgr pub; /* base class */
	JOCTET* buffer; /* buffer start address */
	int bufsize; /* size of buffer */
	size_t datasize; /* final size of compressed data */
	int* outsize; /* user pointer to datasize */
	int errcount; /* counts up write errors due to
				  buffer overruns */
} memory_destination_mgr;

typedef memory_destination_mgr* mem_dest_ptr;

METHODDEF(void)
init_destination (j_compress_ptr cinfo);

METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo);

METHODDEF(void)
term_destination (j_compress_ptr cinfo);

GLOBAL(void)
jpeg_memory_dest (j_compress_ptr cinfo, JOCTET* buffer, int bufsize, int* outsize);

METHODDEF(void)
init_source (j_decompress_ptr dinfo);

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr dinfo);

METHODDEF(void)
skip_input_data (j_decompress_ptr dinfo, INT32 num_bytes);

METHODDEF(void)
term_source (j_decompress_ptr dinfo);

GLOBAL(void)
jpeg_memory_src (j_decompress_ptr dinfo, unsigned char* buffer, size_t size);
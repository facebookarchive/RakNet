#include "jpeg_memory_dest.h"

/* ------------------------------------------------------------- */
/* MEMORY DESTINATION INTERFACE METHODS */
/* ------------------------------------------------------------- */


/* This function is called by the library before any data gets written */
METHODDEF(void)
init_destination (j_compress_ptr cinfo)
{
	mem_dest_ptr dest = (mem_dest_ptr)cinfo->dest;

	dest->pub.next_output_byte = dest->buffer; /* set destination buffer */
	dest->pub.free_in_buffer = dest->bufsize; /* input buffer size */
	dest->datasize = 0; /* reset output size */
	dest->errcount = 0; /* reset error count */
}

/* This function is called by the library if the buffer fills up

I just reset destination pointer and buffer size here.
Note that this behavior, while preventing seg faults
will lead to invalid output streams as data is over-
written.
*/
METHODDEF(boolean)
empty_output_buffer (j_compress_ptr cinfo)
{
	mem_dest_ptr dest = (mem_dest_ptr)cinfo->dest;
	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = dest->bufsize;
	++dest->errcount; /* need to increase error count */

	return TRUE;
}

/* Usually the library wants to flush output here.

I will calculate output buffer size here.
Note that results become incorrect, once
empty_output_buffer was called.
This situation is notified by errcount.
*/
METHODDEF(void)
term_destination (j_compress_ptr cinfo)
{
	mem_dest_ptr dest = (mem_dest_ptr)cinfo->dest;
	dest->datasize = dest->bufsize - dest->pub.free_in_buffer;
	if (dest->outsize) *dest->outsize += (int)dest->datasize;
}

/* Override the default destination manager initialization
provided by jpeglib. Since we want to use memory-to-memory
compression, we need to use our own destination manager.
*/
GLOBAL(void)
jpeg_memory_dest (j_compress_ptr cinfo, JOCTET* buffer, int bufsize, int* outsize)
{
	mem_dest_ptr dest;

	/* first call for this instance - need to setup */
	if (cinfo->dest == 0) {
		cinfo->dest = (struct jpeg_destination_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof (memory_destination_mgr));
	}

	dest = (mem_dest_ptr) cinfo->dest;
	dest->bufsize = bufsize;
	dest->buffer = buffer;
	dest->outsize = outsize;
	/* set method callbacks */
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
}

/* ------------------------------------------------------------- */
/* MEMORY SOURCE INTERFACE METHODS */
/* ------------------------------------------------------------- */

/* Called before data is read */
METHODDEF(void)
init_source (j_decompress_ptr dinfo)
{
	/* nothing to do here, really. I mean. I'm not lazy or something, but...
	we're actually through here. */
}

/* Called if the decoder wants some bytes that we cannot provide... */
METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr dinfo)
{
	/* we can't do anything about this. This might happen if the provided
	buffer is either invalid with regards to its content or just a to
	small bufsize has been given. */

	/* fail. */
	return FALSE;
}

/* From IJG docs: "it's not clear that being smart is worth much trouble"
So I save myself some trouble by ignoring this bit.
*/
METHODDEF(void)
skip_input_data (j_decompress_ptr dinfo, INT32 num_bytes)
{
	/* There might be more data to skip than available in buffer.
	This clearly is an error, so screw this mess. */
	if ((size_t)num_bytes > dinfo->src->bytes_in_buffer) {
		dinfo->src->next_input_byte = 0; /* no buffer byte */
		dinfo->src->bytes_in_buffer = 0; /* no input left */
	} else {
		dinfo->src->next_input_byte += num_bytes;
		dinfo->src->bytes_in_buffer -= num_bytes;
	}
}

/* Finished with decompression */
METHODDEF(void)
term_source (j_decompress_ptr dinfo)
{
	/* Again. Absolute laziness. Nothing to do here. Boring. */
}

GLOBAL(void)
jpeg_memory_src (j_decompress_ptr dinfo, unsigned char* buffer, size_t size)
{
	struct jpeg_source_mgr* src;

	/* first call for this instance - need to setup */
	if (dinfo->src == 0) {
		dinfo->src = (struct jpeg_source_mgr *)
			(*dinfo->mem->alloc_small) ((j_common_ptr) dinfo, JPOOL_PERMANENT,
			sizeof (struct jpeg_source_mgr));
	}

	src = dinfo->src;
	src->next_input_byte = buffer;
	src->bytes_in_buffer = size;
	src->init_source = init_source;
	src->fill_input_buffer = fill_input_buffer;
	src->skip_input_data = skip_input_data;
	src->term_source = term_source;
	/* IJG recommend to use their function - as I don't know ****
	about how to do better, I follow this recommendation */
	src->resync_to_restart = jpeg_resync_to_restart;
}
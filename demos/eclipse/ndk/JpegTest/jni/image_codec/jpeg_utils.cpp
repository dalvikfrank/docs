#include "jpeg_utils.h"

static void jpeg_utils_init_source(j_decompress_ptr cinfo)
{
    jpeg_utils_source_mgr*  src = (jpeg_utils_source_mgr*)cinfo->src;
    src->next_input_byte = (const JOCTET*)src->fBuffer;
    src->bytes_in_buffer = 0;
    src->current_offset = 0;
    if (!src->fStream->rewind()) {
        cinfo->err->error_exit((j_common_ptr)cinfo);
    }
}

static boolean jpeg_utils_seek_input_data(j_decompress_ptr cinfo, long byte_offset)
{
    jpeg_utils_source_mgr* src = (jpeg_utils_source_mgr*)cinfo->src;
    size_t bo = (size_t) byte_offset;

    if (bo > src->current_offset) {
        (void)src->fStream->skip(bo - src->current_offset);
    } else {
        if (!src->fStream->rewind()) {
            cinfo->err->error_exit((j_common_ptr)cinfo);
            return false;
        }
        (void)src->fStream->skip(bo);
    }
    src->current_offset = bo;
    src->next_input_byte = (const JOCTET*)src->fBuffer;
    src->bytes_in_buffer = 0;
    return true;
}

static boolean jpeg_utils_fill_input_buffer(j_decompress_ptr cinfo)
{
    jpeg_utils_source_mgr* src = (jpeg_utils_source_mgr*)cinfo->src;
    size_t bytes = src->fStream->read(src->fBuffer, jpeg_utils_source_mgr::kBufferSize);

    if (bytes == 0) {
        return FALSE;
    }
    src->current_offset += bytes;
    src->next_input_byte = (const JOCTET*)src->fBuffer;
    src->bytes_in_buffer = bytes;
    return TRUE;
}

static void jpeg_utils_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
    jpeg_utils_source_mgr*  src = (jpeg_utils_source_mgr*)cinfo->src;

    if (num_bytes > (long)src->bytes_in_buffer) {
        long bytesToSkip = num_bytes - src->bytes_in_buffer;
        while (bytesToSkip > 0) {
            long bytes = (long)src->fStream->skip(bytesToSkip);
            if (bytes <= 0 || bytes > bytesToSkip) {
                cinfo->err->error_exit((j_common_ptr)cinfo);
                return;
            }
            src->current_offset += bytes;
            bytesToSkip -= bytes;
        }
        src->next_input_byte = (const JOCTET*)src->fBuffer;
        src->bytes_in_buffer = 0;
    } else {
        src->next_input_byte += num_bytes;
        src->bytes_in_buffer -= num_bytes;
    }
}

static void jpeg_utils_term_source(j_decompress_ptr cinfo)
{
}

jpeg_utils_source_mgr::jpeg_utils_source_mgr(Stream* stream) : fStream(stream)
{
    init_source = jpeg_utils_init_source;
    fill_input_buffer = jpeg_utils_fill_input_buffer;
    skip_input_data = jpeg_utils_skip_input_data;
    resync_to_restart = jpeg_resync_to_restart;
    term_source = jpeg_utils_term_source;
    seek_input_data = jpeg_utils_seek_input_data;
}


static void jpeg_utils_init_destination(j_compress_ptr cinfo)
{
    jpeg_utils_destination_mgr* dest = (jpeg_utils_destination_mgr*)cinfo->dest;

    dest->next_output_byte = dest->fBuffer;
    dest->free_in_buffer = jpeg_utils_destination_mgr::kBufferSize;
}

static boolean jpeg_utils_empty_output_buffer(j_compress_ptr cinfo)
{
    jpeg_utils_destination_mgr* dest = (jpeg_utils_destination_mgr*)cinfo->dest;

    if (!dest->fStream->write(dest->fBuffer, jpeg_utils_destination_mgr::kBufferSize)) {
        ERREXIT(cinfo, JERR_FILE_WRITE);
        return false;
    }
    dest->next_output_byte = dest->fBuffer;
    dest->free_in_buffer = jpeg_utils_destination_mgr::kBufferSize;
    return TRUE;
}

static void jpeg_utils_term_destination (j_compress_ptr cinfo)
{
    jpeg_utils_destination_mgr* dest = (jpeg_utils_destination_mgr*)cinfo->dest;
    size_t size = jpeg_utils_destination_mgr::kBufferSize - dest->free_in_buffer;

    if (size > 0) {
        if (!dest->fStream->write(dest->fBuffer, size)) {
            ERREXIT(cinfo, JERR_FILE_WRITE);
            return;
        }
    }
    dest->fStream->flush();
}

jpeg_utils_destination_mgr::jpeg_utils_destination_mgr(Stream* stream)   : fStream(stream)
{
    this->init_destination = jpeg_utils_init_destination;
    this->empty_output_buffer = jpeg_utils_empty_output_buffer;
    this->term_destination = jpeg_utils_term_destination;
}

void jpeg_utils_error_exit(j_common_ptr cinfo)
{
    jpeg_utils_error_mgr* error = (jpeg_utils_error_mgr*)cinfo->err;
    char buffer[JMSG_LENGTH_MAX];

    /* Create the message */
    (*cinfo->err->format_message) (cinfo, buffer);

    LOGE("skia error skjpeg_error_exit,msgcode[%d]  msg:%s\n", (cinfo)->err->msg_code, buffer);

    /* Let the memory manager delete any temp files before we die */
    jpeg_destroy(cinfo);

    longjmp(error->fJmpBuf, -1);
}

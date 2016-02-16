#ifndef __IMAGE_CODEC_JPEG_UTILS_H__
#define __IMAGE_CODEC_JPEG_UTILS_H__

#include "stream.h"
extern "C" {
    #include "jpeglib.h"
    #include "jerror.h"
}
#include <setjmp.h>

struct jpeg_utils_source_mgr : jpeg_source_mgr {
    jpeg_utils_source_mgr(Stream* stream);

    Stream*       fStream;
    enum {
        kBufferSize = 4096
    };
    char    fBuffer[kBufferSize];
};

struct jpeg_utils_destination_mgr : jpeg_destination_mgr {
    jpeg_utils_destination_mgr(Stream* stream);

    Stream*  fStream;
    enum {
        kBufferSize = 1024
    };
    uint8_t fBuffer[kBufferSize];
};

struct jpeg_utils_error_mgr : jpeg_error_mgr {
    jmp_buf fJmpBuf;
};

void jpeg_utils_error_exit(j_common_ptr cinfo);

#endif /* __IMAGE_CODEC_JPEG_UTILS_H__ */

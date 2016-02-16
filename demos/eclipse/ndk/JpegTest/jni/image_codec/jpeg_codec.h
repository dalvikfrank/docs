#ifndef __IMAGE_CODEC_JPEG_CODEC_H__
#define __IMAGE_CODEC_JPEG_CODEC_H__

#include "stream.h"
#include "jpeg_utils.h"

class JpegEncoder
{
public:
    JpegEncoder() {}

    virtual ~JpegEncoder() {}

    bool onEncode(Stream* stream, int type, unsigned char* imageBuf, int width, int height, int stride, int depth, int quality);
};

class JpegDecoder
{
public:
    JpegDecoder() {}

    virtual ~JpegDecoder() {}

    bool onDecode(Stream* stream, int type, unsigned char** imageBuf, int* width, int* height, int* stride, int* depth);
};

#endif /* __IMAGE_CODEC_JPEG_CODEC_H__ */

#include "jpeg_codec.h"

static void set_error_mgr(jpeg_decompress_struct* cinfo, jpeg_utils_error_mgr* errorManager)
{
    cinfo->err = jpeg_std_error(errorManager);
    errorManager->error_exit = jpeg_utils_error_exit;
}

static void overwrite_mem_buffer_size(jpeg_decompress_struct* cinfo)
{
    cinfo->mem->max_memory_to_use = 30 * 1024 * 1024;
}

static void initialize_info(jpeg_decompress_struct* cinfo, jpeg_utils_source_mgr* src_mgr)
{
    jpeg_create_decompress(cinfo);
    overwrite_mem_buffer_size(cinfo);
    cinfo->src = src_mgr;
}

typedef void (*WriteScanline)(unsigned char* src, unsigned char* dst, int width);

static void Write_RGBA8888(unsigned char* src, unsigned char* dst, int width)
{
    while (width--) {
        *(dst++) = *(src++);
        *(dst++) = *(src++);
        *(dst++) = *(src++);
        src++;
    }
}

static void Write_YUV444(unsigned char* src, unsigned char* dst, int width)
{
    while (width--) {
        *(dst++) = *(src++);
        *(dst++) = *(src++);
        *(dst++) = *(src++);
    }
}

static WriteScanline ChooseWriteScanline(int type)
{
    switch (type) {
        case JCS_RGB:
        case JCS_YCbCr:
            return Write_YUV444;
        case JCS_RGBA_8888:
            return Write_RGBA8888;
        default:
            return NULL;
    }
}

bool JpegEncoder::onEncode(Stream* stream, int type, unsigned char* imageBuf, int width, int height, int stride, int depth, int quality)
{
    struct jpeg_compress_struct cinfo;
    jpeg_utils_error_mgr        sk_err;
    jpeg_utils_destination_mgr  srcManager(stream);
    JSAMPROW row_pointer[1];
    unsigned char* dstRow = (unsigned char*) malloc(width * 3);
    WriteScanline writer = ChooseWriteScanline(type);

    if (NULL == dstRow || NULL == writer) {
        return false;
    }
    LOGE("onEncode 1");
    cinfo.err = jpeg_std_error(&sk_err);
    LOGE("onEncode 2");
    sk_err.error_exit = jpeg_utils_error_exit;
    LOGE("onEncode 3");
    if (setjmp(sk_err.fJmpBuf)) {
        LOGE("onEncode 4");
        return false;
    }
    LOGE("onEncode 5");
    jpeg_create_compress(&cinfo);
    LOGE("onEncode 6");
    cinfo.dest = &srcManager;
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;//depth;
    cinfo.in_color_space = (J_COLOR_SPACE)(type == JCS_RGBA_8888 ? JCS_RGB : type);
    LOGE("input_components:%d, cinfo.in_color_space:%d", cinfo.input_components, cinfo.in_color_space);
    cinfo.input_gamma = 1;

    LOGE("onEncode 7");
    jpeg_set_defaults(&cinfo);
    LOGE("onEncode 8");
    jpeg_set_quality(&cinfo, quality, TRUE);

    LOGE("onEncode 9");
    jpeg_start_compress(&cinfo, TRUE);
    LOGE("onEncode 10");

    while (cinfo.next_scanline < cinfo.image_height) {
        //row_pointer[0] = &imageBuf[cinfo.next_scanline * stride];
        writer(&imageBuf[cinfo.next_scanline * stride], dstRow, width);
        row_pointer[0] = dstRow;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    LOGE("onEncode 11");
    jpeg_finish_compress(&cinfo);
    LOGE("onEncode 12");
    jpeg_destroy_compress(&cinfo);
    LOGE("onEncode 13");
    LOGE("onEncode 14");
    return true;
}

bool JpegDecoder::onDecode(Stream* stream, int type, unsigned char** imageBuf, int* width, int* height, int* stride, int* depth)
{
    struct jpeg_decompress_struct cinfo;
    jpeg_utils_error_mgr errorManager;
    JSAMPLE* buffer;
    int row_stride;

    LOGE("onDecode 1");
    jpeg_utils_source_mgr srcManager(stream);
    LOGE("onDecode 2");
    set_error_mgr(&cinfo, &errorManager);
    LOGE("onDecode 3");
    if (setjmp(errorManager.fJmpBuf)) {
        return false;
    }
    LOGE("onDecode 4");
    initialize_info(&cinfo, &srcManager);
    LOGE("onDecode 5");
    jpeg_read_header(&cinfo, TRUE);
    cinfo.out_color_space = (J_COLOR_SPACE) type;
    jpeg_start_decompress(&cinfo);

    LOGE("onDecode 6");
    *width = cinfo.output_width;
    *height = cinfo.output_height;
    *stride = row_stride  = cinfo.output_width * cinfo.output_components;
    *depth = cinfo.output_components;
    LOGE("onDecode 7");
    *imageBuf = (unsigned char*) malloc(cinfo.output_height * row_stride);
    if (NULL == *imageBuf) {
        LOGE("onDecode 8");
        jpeg_destroy_decompress(&cinfo);
        return false;
    }
    LOGE("onDecode 9");
    buffer = (JSAMPLE*)(*imageBuf);
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &buffer, 1);
        buffer += row_stride;
    }
    LOGE("onDecode 10");
    jpeg_finish_decompress(&cinfo);
    LOGE("onDecode 11");
    jpeg_destroy_decompress(&cinfo);
    LOGE("onDecode 12");
    return true;
}

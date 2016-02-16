#include "jpeg_codec.h"
#include "inputstream_wrapper.h"

void write_file(const char* path, const unsigned char* buffer, int bufferSize)
{
    if (NULL != path && NULL != buffer && bufferSize > 0) {
        FILE* fp = fopen(path, "wb+");
        if (NULL != fp) {
            fwrite(buffer, 1, bufferSize, fp);
            fflush(fp);
            fclose(fp);
        }
    }
}

extern "C" void Java_com_jpeg_test_JpegTest_nativeTest(JNIEnv* env, jobject object, jobject jinputStream, jbyteArray storage)
{
    JpegDecoder decoder;
    JavaInputStreamWrapper inputStream(env, jinputStream, storage);
    unsigned char* buffer = NULL;
    int width = 0, height = 0, stride = 0, depth = 0, type = JCS_RGBA_8888;

    decoder.onDecode(&inputStream, type, &buffer, &width, &height, &stride, &depth);
    LOGE("width:%d, height:%d\n", width, height);
    write_file("/sdcard/xxxx_abc.yuv", buffer, width * height * 4);
}

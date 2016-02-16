#ifndef __IMAGE_CODEC_STREAM_H__
#define __IMAGE_CODEC_STREAM_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>


#if 1//def LOG_DEBUG
        #define LOG_TAG                 "filter_engine"
        #define LOG(...)                 __android_log_print(ANDROID_LOG_ERROR,  LOG_TAG,  __VA_ARGS__)
        #define LOGV(...)                __android_log_print(ANDROID_LOG_VERBOSE,  LOG_TAG,  __VA_ARGS__)
        #define LOGD(...)                __android_log_print(ANDROID_LOG_DEBUG,  LOG_TAG,  __VA_ARGS__)
        #define LOGI(...)                  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG,  __VA_ARGS__)
        #define LOGW(...)               __android_log_print(ANDROID_LOG_WARN,  LOG_TAG,  __VA_ARGS__)
        #define LOGE(...)                __android_log_print(ANDROID_LOG_ERROR,  LOG_TAG,  __VA_ARGS__)
        #define LOGS(...)                __android_log_print(ANDROID_LOG_SILENT,  LOG_TAG,  __VA_ARGS__)
#else
        #define LOGV(...)
        #define LOGD(...)
        #define LOGI(...)
        #define LOGW(...)
        #define LOGE(...)
        #define LOGS(...)
#endif /* LOG_DEBUG */

class Stream
{
public:
    Stream() {}

    virtual ~Stream() {}

    virtual size_t read(void* buffer, size_t size) { return 0; }

    virtual bool write(void* buffer, size_t size) { return false; }

    virtual void flush() {}

    virtual size_t skip(size_t size) { return read(NULL, size); }

    virtual bool seek(size_t position) { return false; }

    virtual bool rewind() { return false; }

    virtual bool hasPosition() const { return false; }

    virtual size_t getPosition() const { return 0; }

    virtual bool hasLength() const { return false; }

    virtual size_t getLength() const { return false; }

    virtual bool isAtEnd() const { return false; }

    virtual void close() {}
};

#endif /* __PHOTO_RENDER_STREAM_H__ */

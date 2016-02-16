#ifndef __IMAGE_CODEC_OUTPUT_STREAM_WRAPPER_H__
#define __IMAGE_CODEC_OUTPUT_STREAM_WRAPPER_H__

#include "stream.h"

class JavaOutputStreamWrapper : public Stream
{
public:
    JavaOutputStreamWrapper(JNIEnv* env, jobject joutputStream, jbyteArray storage);

    virtual ~JavaOutputStreamWrapper();

    virtual bool write(void* buffer, size_t size);

    virtual void flush();
private:
    JNIEnv*     mEnv;
    jobject     mJavaOutputStream;
    jbyteArray  mJavaByteArray;
    size_t      mCapacity;
    jmethodID   mOutputStream_writeMethodID;
    jmethodID   mOutputStream_flushMethodID;
};

#endif /* __IMAGE_CODEC_OUTPUT_STREAM_WRAPPER_H__ */

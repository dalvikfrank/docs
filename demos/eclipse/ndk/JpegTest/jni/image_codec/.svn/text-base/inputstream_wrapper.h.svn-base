#ifndef __IMAGE_CODEC_INPUT_STREAM_WRAPPER_H__
#define __IMAGE_CODEC_INPUT_STREAM_WRAPPER_H__

#include "stream.h"

class JavaInputStreamWrapper : public Stream
{
public:
    JavaInputStreamWrapper(JNIEnv* env, jobject jinputStream, jbyteArray storage);

    virtual ~JavaInputStreamWrapper();

    virtual size_t read(void* buffer, size_t size);

    virtual bool rewind();

    virtual bool isAtEnd() const;
protected:
    size_t doRead(void* buffer, size_t size);

    size_t doSkip(size_t size);
private:
    JNIEnv*     mEnv;
    jobject     mJavaInputStream;
    jbyteArray  mJavaByteArray;
    size_t      mCapacity;
    size_t      mBytesRead;
    bool        mIsAtEnd;
    jmethodID   mInputStream_readMethodID;
    jmethodID   mInputStream_skipMethodID;
};

#endif /* __IMAGE_CODEC_INPUT_STREAM_WRAPPER_H__ */

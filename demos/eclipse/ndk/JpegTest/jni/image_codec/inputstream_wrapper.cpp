#include "inputstream_wrapper.h"

JavaInputStreamWrapper::JavaInputStreamWrapper(JNIEnv* env, jobject jinputStream, jbyteArray storage) : mEnv(env), mJavaInputStream(jinputStream), mJavaByteArray(storage)
{
    mCapacity = mEnv->GetArrayLength(storage);
    jclass inputStream_Clazz = mEnv->FindClass("java/io/InputStream");
    mInputStream_readMethodID = mEnv->GetMethodID(inputStream_Clazz, "read", "([BII)I");
    mInputStream_skipMethodID = mEnv->GetMethodID(inputStream_Clazz, "skip", "(J)J");
    mBytesRead = 0;
    mIsAtEnd = false;
}

JavaInputStreamWrapper::~JavaInputStreamWrapper()
{
}

size_t JavaInputStreamWrapper::read(void* buffer, size_t size)
{
    JNIEnv* env = mEnv;

    if (NULL == buffer) {
        if (0 == size) {
            return 0;
        } else {
            size_t amountSkipped = 0;
            do {
                size_t amount = this->doSkip(size - amountSkipped);
                if (0 == amount) {
                    char tmp;
                    amount = this->doRead(&tmp, 1);
                    if (0 == amount) {
                        mIsAtEnd = true;
                        break;
                    }
                }
                amountSkipped += amount;
            } while (amountSkipped < size);
            return amountSkipped;
        }
    }
    return this->doRead(buffer, size);
}

bool JavaInputStreamWrapper::rewind()
{
    return true;
}

bool JavaInputStreamWrapper::isAtEnd() const
{
    return mIsAtEnd;
}

size_t JavaInputStreamWrapper::doRead(void* buffer, size_t size)
{
    JNIEnv* env = mEnv;
    size_t bytesRead = 0;

    do {
        size_t requested = size;
        if (requested > mCapacity)
            requested = mCapacity;

        jint n = env->CallIntMethod(mJavaInputStream, mInputStream_readMethodID, mJavaByteArray, 0, requested);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            LOGE("---- read threw an exception\n");
            return 0;
        }

        if (n < 0) {
            mIsAtEnd = true;
            break;
        }

        env->GetByteArrayRegion(mJavaByteArray, 0, n, reinterpret_cast<jbyte*>(buffer));
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            LOGE("---- read:GetByteArrayRegion threw an exception\n");
            return 0;
        }

        buffer = (void*)((char*)buffer + n);
        bytesRead += n;
        size -= n;
        mBytesRead += n;
    } while (size != 0);

    return bytesRead;
}

size_t JavaInputStreamWrapper::doSkip(size_t size)
{
    JNIEnv* env = mEnv;

    jlong skipped = env->CallLongMethod(mJavaInputStream, mInputStream_skipMethodID, (jlong)size);
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        LOGE("------- skip threw an exception\n");
        return 0;
    }
    if (skipped < 0) {
        skipped = 0;
    }
    return (size_t)skipped;
}

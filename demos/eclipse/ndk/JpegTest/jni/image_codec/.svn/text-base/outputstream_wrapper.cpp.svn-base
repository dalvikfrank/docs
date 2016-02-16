#include "outputstream_wrapper.h"

JavaOutputStreamWrapper::JavaOutputStreamWrapper(JNIEnv* env, jobject joutputStream, jbyteArray storage) : mEnv(env), mJavaOutputStream(joutputStream), mJavaByteArray(storage)
{
    mCapacity = mEnv->GetArrayLength(storage);
    jclass outputStream_Clazz = mEnv->FindClass("java/io/OutputStream");
    mOutputStream_writeMethodID = mEnv->GetMethodID(outputStream_Clazz, "write", "([BII)V");
    mOutputStream_flushMethodID = mEnv->GetMethodID(outputStream_Clazz, "flush", "()V");
}

JavaOutputStreamWrapper::~JavaOutputStreamWrapper()
{
}

bool JavaOutputStreamWrapper::write(void* buffer, size_t size)
{
    JNIEnv* env = mEnv;
    jbyteArray storage = mJavaByteArray;

    while (size > 0) {
        size_t requested = size;
        if (requested > mCapacity) {
            requested = mCapacity;
        }

        env->SetByteArrayRegion(storage, 0, requested, reinterpret_cast<const jbyte*>(buffer));
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            LOGE("--- write:SetByteArrayElements threw an exception\n");
            return false;
        }

        env->CallVoidMethod(mJavaOutputStream, mOutputStream_writeMethodID, storage, 0, requested);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            LOGE("------- write threw an exception\n");
            return false;
        }

        buffer = (void*)((char*)buffer + requested);
        size -= requested;
    }
    return true;
}

void JavaOutputStreamWrapper::flush()
{
    mEnv->CallVoidMethod(mJavaOutputStream, mOutputStream_flushMethodID);
}

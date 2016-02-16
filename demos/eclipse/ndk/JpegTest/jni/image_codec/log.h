#ifndef __FILTER_ENGINE_COMMON_LOG_H__
#define __FILTER_ENGINE_COMMON_LOG_H__

#include <jni.h>
#include <android/log.h>

#ifdef LOG_DEBUG
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

#endif /* __FILTER_ENGINE_COMMON_LOG_H__ */

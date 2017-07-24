//
// Created by yw on 2017/7/24.
//

#ifndef OPENCV4ANDROIDWITHCMAKE_MASTER_LOGGER_H
#define OPENCV4ANDROIDWITHCMAKE_MASTER_LOGGER_H
#include <android/log.h>
#define  LOG_TAG    "appdebug"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)

#endif //OPENCV4ANDROIDWITHCMAKE_MASTER_LOGGER_H

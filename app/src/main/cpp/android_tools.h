//
// Created by michael on 01.05.17.
//

#ifndef ALG_TEST_ANDROID_ANDROID_UTILS_H
#define ALG_TEST_ANDROID_ANDROID_UTILS_H

#include <android/log.h>
#include <ctime>

#define LOGINFO(x...) __android_log_print(ANDROID_LOG_INFO,"CL_TEST",x)
#define LOGERR(x...) __android_log_print(ANDROID_LOG_ERROR,"CL_TEST",x)

#define BENCHMARK 1

unsigned char* imgDataPtrFromCvMatPtrAddr(long long cvMatPtrAddr,
                                          int *w = 0x00, int *h = 0x00, int *chan = 0x00);

void checkGLError(const char *msg);

#endif //ALG_TEST_ANDROID_ANDROID_UTILS_H

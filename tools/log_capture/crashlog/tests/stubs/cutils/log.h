#ifndef __CUTILS_LOG_H__
#define __CUTILS_LOG_H__

#include <stdio.h>

#define ALOGD(...) printf("D: " LOG_TAG ": " __VA_ARGS__)
#define ALOGE(...) printf("E: " LOG_TAG ": " __VA_ARGS__)
#define ALOGI(...) printf("I: " LOG_TAG ": " __VA_ARGS__)
#define ALOGW(...) printf("W: " LOG_TAG ": " __VA_ARGS__)
#define ALOGV(...) printf("V: " LOG_TAG ": " __VA_ARGS__)

#endif /* __CUTILS_LOG_H__ */

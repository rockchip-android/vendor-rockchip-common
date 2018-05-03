/*
 * Copyright (C) Intel 2014 - 2015
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LCTJNI"
#include <utils/Log.h>
#include <jni.h>
#include <lctclient.h>
#include <string.h>

static const char *classPathName = "com/intel/lct/Native";

#define METHOD_SIG "(ILjava/lang/String;Ljava/lang/String;I[Ljava/lang/Object;)I"
#define DATA_COUNT 6
#define FLIST_OFFSET 6
#define ADD_STEPS_OFFSET 7
#define MAX_VA_LIST 8

static jint lctLog(JNIEnv * env, jclass __attribute((unused)) thiz,
                   jint type, jstring subObj, jstring evObj, jint flags,
                   jobjectArray va)
{

    const char *data[6] = { NULL, };
    const char *sub = NULL, *event = NULL, *flist = NULL;
    unsigned int va_len = MAX_VA_LIST, i;
    jobject tmpObj;
    jint additionalSteps = 0;
    int ret = -1;
    jclass strClass;

    jclass intClass;

    jfieldID valInInteger;
    /*submitter */
    sub = (*env)->GetStringUTFChars(env, subObj, NULL);
    if (!sub)
        goto cleanup;
    /*event */
    event = (*env)->GetStringUTFChars(env, evObj, NULL);
    if (!event)
        goto cleanup;

    va_len = (*env)->GetArrayLength(env, va);
    if (va_len > MAX_VA_LIST) {
        ALOGE("Unexpected number of arguments");
        goto cleanup;
    }

    strClass = (*env)->FindClass(env, "java/lang/String");
    if (strClass == NULL) {
        ALOGE("Cannot get java/lang/String class");
        goto cleanup;
    }

    for (i = 0; i < DATA_COUNT && i < va_len; i++) {
        tmpObj = (*env)->GetObjectArrayElement(env, va, i);
        if ((*env)->IsInstanceOf(env, tmpObj, strClass)) {
            data[i] = (*env)->GetStringUTFChars(env, (jstring) tmpObj, NULL);
        } else {
            ALOGE("Array member %d is not string", i);
            goto cleanup;
        }
    }

    if (va_len > FLIST_OFFSET) {
        tmpObj = (*env)->GetObjectArrayElement(env, va, FLIST_OFFSET);
        if ((*env)->IsInstanceOf(env, tmpObj, strClass)) {
            flist = (*env)->GetStringUTFChars(env, (jstring) tmpObj, NULL);
        } else {
            ALOGE("Array member %d is not string", FLIST_OFFSET);
            goto cleanup;
        }
    }

    if (va_len > ADD_STEPS_OFFSET) {
        intClass = (*env)->FindClass(env, "java/lang/Integer");
        if (intClass == NULL) {
            ALOGE("Cannot get java/lang/Integer class");
            goto cleanup;
        }
        tmpObj = (*env)->GetObjectArrayElement(env, va, ADD_STEPS_OFFSET);
        if ((*env)->IsInstanceOf(env, tmpObj, intClass)) {
            valInInteger = (*env)->GetFieldID(env, intClass, "value", "I");
            if (!valInInteger) {
                ALOGI("Cannot get vealue field id in Integer");
                goto cleanup;
            }
            additionalSteps = (*env)->GetIntField(env, tmpObj, valInInteger);
        } else {
            ALOGE("Array member %d is not Integer", ADD_STEPS_OFFSET);
            goto cleanup;
        }
    }

    ret = lct_log(type, sub, event, flags, data[0], data[1], data[2], data[3],
                  data[4], data[5], flist, additionalSteps);

  cleanup:

    if (flist) {
        tmpObj = (*env)->GetObjectArrayElement(env, va, FLIST_OFFSET);
        (*env)->ReleaseStringUTFChars(env, (jstring) tmpObj, flist);
    }
    for (i = 0; i < DATA_COUNT; i++) {
        if (data[i]) {
            tmpObj = (*env)->GetObjectArrayElement(env, va, i);
            (*env)->ReleaseStringUTFChars(env, (jstring) tmpObj, data[i]);
        }
    }

    if (event)
        (*env)->ReleaseStringUTFChars(env, evObj, event);

    if (sub)
        (*env)->ReleaseStringUTFChars(env, subObj, sub);

    return ret;
}

static JNINativeMethod methods[] = {
    {"lctLog", METHOD_SIG, (void *)lctLog}
};

static int registerMethods(JNIEnv * env)
{
    jclass clazz;

    clazz = (*env)->FindClass(env, classPathName);
    if (clazz == NULL) {
        ALOGE("Native registration unable to find class '%s'", classPathName);
        return JNI_FALSE;
    }
    if ((*env)->RegisterNatives(env, clazz, methods,
                                sizeof(methods) / sizeof(methods[0])) < 0) {
        ALOGE("RegisterNatives failed for '%s'", classPathName);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM * vm, void __attribute((unused)) * reserved)
{
    JNIEnv *env = NULL;

    if ((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_6) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed");
        return -1;
    }

    if (registerMethods(env) != JNI_TRUE) {
        ALOGE("ERROR: registerNatives failed");
        return -1;
    }
    return JNI_VERSION_1_6;
}

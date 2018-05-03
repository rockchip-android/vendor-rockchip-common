/* Android Modem Traces and Logs
 *
 * Copyright (C) Intel 2015
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
 *
 */

#ifndef __RPCCALL_JNI_H__
#define __RPCCALL_JNI_H__

#include <jni.h>

/*
 * Class:     com_intel_amtl_modem_communication_RPCWrapper
 * Method:    SendRPCCommand
 * Signature: (Ljava/lang/String)I;
 */

JNIEXPORT jstring JNICALL Java_com_intel_amtl_modem_communication_RPCWrapper_SendRPCCommand(
        JNIEnv *, jobject, jstring);

/*
 * Class:     com_intel_amtl_common_modem_RPCWrapper
 * Method:    GenerateCoredump
 * Signature: ()I;
 */

JNIEXPORT void JNICALL Java_com_intel_amtl_modem_communication_RPCWrapper_GenerateCoredump(JNIEnv *,
        jobject);

#endif //__RPCCALL_JNI_H_

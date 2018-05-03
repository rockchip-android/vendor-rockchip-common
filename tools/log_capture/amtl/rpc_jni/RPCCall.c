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

#include <sys/types.h>
#include <strings.h>

#if defined (SOFIA_PLATFORM)
#include "uta_types.h"
#include "uta_common_types.h"
#include "app_rpc_interface.h"
#include "uta_trap_cfg.h"

#if !defined(HOST_TEST_SETUP)
#include <packer_unpacker.h>
#endif
#endif

#include "RPCCall.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "AMTL"
#define UNUSD __attribute__((__unused__))

#if defined (SOFIA_PLATFORM)
static UtaContextId rpcCtxId = 0;

void UtaTrcRPCNotifyIndCb(UtaUInt32 config_type)
{
    ALOGI("UtaTrcRPCNotifyIndCb - config_type: %d", config_type);
}

static INT32 amtl_register()
{
    UINT32 rpc_upper_bits = 0x0;
    INT32 rpc_id = 0;
    UINT32 RpcAppsID;

    rpc_upper_bits = 0x20000000;
    RpcAppsID = AppRPCIFHndlrInit();
    RpcAppsID = (RpcAppsID & 0x000000FF);
    rpc_upper_bits |=  (RpcAppsID << 8);
    rpc_id = rpc_upper_bits;
    RPCSetClientId(rpc_id);
    return rpc_id;
}

JNIEXPORT jstring JNICALL Java_com_intel_amtl_modem_communication_RPCWrapper_SendRPCCommand(
        JNIEnv *env, UNUSD jobject obj, jstring cmd)
{
    const UtaChar* result;
    jstring stringResult;

    UtaChar* nativeString = (UtaChar*) (*env)->GetStringUTFChars(env, cmd, NULL);

    if (rpcCtxId == 0) {
        rpcCtxId = amtl_register();
    }

    result = UTA_REMOTE_CALL(UtaTrcAtCmnd)(UTATRC_AT_CMND_GENERIC, nativeString);
    (*env)->ReleaseStringUTFChars(env, cmd, nativeString);

    if (NULL == result) {
        return (*env)->NewStringUTF(env, "OK");
    }
    else if ((UtaChar*)RPC_RESULT_SEM_ERROR == result)
    {
        return (*env)->NewStringUTF(env, "ERROR");
    }
    else
    {
        stringResult = (*env)->NewStringUTF(env, (const char*)result);
        return stringResult;
    }
}

JNIEXPORT void JNICALL Java_com_intel_amtl_modem_communication_RPCWrapper_GenerateCoredump(
        UNUSD JNIEnv *env, UNUSD jobject obj)
{
    if (rpcCtxId == 0) {
        rpcCtxId = amtl_register();
    }

    UTA_REMOTE_CALL(UtaTrapGenerateSwTrap)(UTA_TRAP_MODULE_ID_CUSTOMER, 0, NULL, __LINE__,
            __FILE__);
}

#else
JNIEXPORT jstring JNICALL Java_com_intel_amtl_modem_communication_RPCWrapper_SendRPCCommand(
        JNIEnv *env, UNUSD jobject obj, UNUSD jstring cmd)
{
    return (*env)->NewStringUTF(env, "ERROR");
}

JNIEXPORT void JNICALL Java_com_intel_amtl_modem_communication_RPCWrapper_GenerateCoredump(
        UNUSD JNIEnv *env, UNUSD jobject obj)
{
}
#endif


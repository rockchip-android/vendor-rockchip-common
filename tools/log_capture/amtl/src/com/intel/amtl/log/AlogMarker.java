/* Android AMTL
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
 * Author: Erwan Bracq <erwan.bracq@intel.com>
 * Author: Morgane Butscher <morganex.butscher@intel.com>
 */

package com.intel.amtl.log;

import android.os.Trace;

public class AlogMarker {

/*
 * Trace the beginning of an asynchronous event. Unlike ATRACE_BEGIN/ATRACE_END
 * contexts, asynchronous events do not need to be nested. The name describes
 * the event, and the cookie provides a unique identifier for distinguishing
 * simultaneous events. The name and cookie used to begin an event must be
 * used to end it.
 */
    public static void tAB(String usertag, String cookie) {
        Trace.asyncTraceBegin(Trace.TRACE_TAG_ALWAYS, usertag, 0);
    }

/*
 * Trace the end of an asynchronous event.
 * This should have a corresponding ATRACE_ASYNC_BEGIN.
 */
    public static void tAE(String usertag, String cookie) {
        Trace.asyncTraceEnd(Trace.TRACE_TAG_ALWAYS, usertag, 0);
    }
}

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
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 */

package com.intel.amtl.modem.output_config;

import android.util.Log;

import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.ModemConf;
import com.intel.amtl.modem.controller.ModemController;
import com.intel.amtl.modem.communication.IoctlWrapper;

public class IoctlConfig implements LogOutputConfig {

    private final String TAG = "AMTL";
    private final String MODULE = "IoctlConfig";
    private IoctlWrapper ioctlWrapper;

    public IoctlConfig() {
        AlogMarker.tAB("IoctlConfig.IoctlConfig", "0");
        ioctlWrapper = new IoctlWrapper();
        AlogMarker.tAE("IoctlConfig.IoctlConfig", "0");
    }

    public String confTraceAndModemInfo(ModemConf mdmConf, ModemController mdmCtrl)
            throws ModemControlException {
        AlogMarker.tAB("IoctlConfig.confTraceAndModemInfo", "0");
        AlogMarker.tAE("IoctlConfig.confTraceAndModemInfo", "0");
        return "OK";
    }

    public String checkAtXsioState(ModemController mdmCtrl) throws ModemControlException {
        AlogMarker.tAB("IoctlConfig.checkAtXsioState", "0");
        AlogMarker.tAE("IoctlConfig.checkAtXsioState", "0");
        return "";
    }

    public String getAtXsioState(ModemController mdmCtrl) throws ModemControlException {
        AlogMarker.tAB("ModemController.getAtXsioState", "0");
        AlogMarker.tAE("ModemController.getAtXsioState", "0");
        return null;
    }

    public String getOctDriverPath() {
        AlogMarker.tAB("IoctlConfig.getOctDriverPath", "0");
        String octDriverPath = ioctlWrapper.getOctDriverPath();
        Log.d(TAG, MODULE + ": Oct driver path " + octDriverPath);
        AlogMarker.tAE("IoctlConfig.getOctDriverPath", "0");
        return octDriverPath;
    }
}

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
 */

package com.intel.amtl.modem.controller;

import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.ModemConf;

public class OctController extends ModemController {

    public OctController() throws ModemControlException {
        super();
        AlogMarker.tAB("OctController.OctController", "0");
        AlogMarker.tAE("OctController.OctController", "0");
    }

    @Override
    public boolean queryTraceState() throws ModemControlException {
        AlogMarker.tAB("OctController.queryTraceState", "0");
        AlogMarker.tAE("OctController.queryTraceState", "0");
        return !checkOct().equals("0");
    }

    @Override
    public String switchOffTrace() throws ModemControlException {
        AlogMarker.tAB("OctController.switchOffTrace", "0");
        AlogMarker.tAE("OctController.switchOffTrace", "0");
        return sendCommand("AT+XSYSTRACE=0\r\n");
    }

    @Override
    public void switchTrace(ModemConf mdmConf) throws ModemControlException {
        AlogMarker.tAB("OctController.switchTrace", "0");
        sendCommand(mdmConf.getXsystrace());
        AlogMarker.tAE("OctController.switchTrace", "0");
    }

    @Override
    public String checkAtTraceState() throws ModemControlException {
        AlogMarker.tAB("OctController.checkAtTraceState", "0");
        AlogMarker.tAE("OctController.checkAtTraceState", "0");
        return "";
    }

    @Override
    public ModemConf getNoLoggingConf() {
        AlogMarker.tAB("OctController.getNoLoggingConf", "0");
        AlogMarker.tAE("OctController.getNoLoggingConf", "0");
        return ModemConf.getInstance("", "", "AT+XSYSTRACE=0\r\n", "", "");
    }
}

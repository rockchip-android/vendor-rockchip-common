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

public class AliasController extends ModemController {

    final String ALL_OFF = "AT+XSYSTRACE=pnall_off,,,\"oct=0\"\r\n";

    public AliasController() throws ModemControlException {
        super();
        AlogMarker.tAB("AliasController.AliasController", "0");
        AlogMarker.tAE("AliasController.AliasController", "0");
    }

    @Override
    public boolean queryTraceState() throws ModemControlException {
        AlogMarker.tAB("AliasController.queryTraceState", "0");
        AlogMarker.tAE("AliasController.queryTraceState", "0");
        return !checkProfileName().equals("all_off");
    }

    @Override
    public String switchOffTrace() throws ModemControlException {
        AlogMarker.tAB("AliasController.switchOffTrace", "0");
        AlogMarker.tAE("AliasController.switchOffTrace", "0");
        return sendCommand(ALL_OFF);
    }

    @Override
    public void switchTrace(ModemConf mdmConf) throws ModemControlException {
        AlogMarker.tAB("AliasController.switchTrace", "0");
        sendCommand(mdmConf.getXsystrace());
        AlogMarker.tAE("AliasController.switchTrace", "0");
    }

    @Override
    public String checkAtTraceState() throws ModemControlException {
        AlogMarker.tAB("AliasController.checkAtTraceState", "0");
        AlogMarker.tAE("AliasController.checkAtTraceState", "0");
        return "";
    }

    @Override
    public ModemConf getNoLoggingConf() {
        AlogMarker.tAB("AliasController.getNoLoggingConf", "0");
        AlogMarker.tAE("AliasController.getNoLoggingConf", "0");
        return ModemConf.getInstance("", "", ALL_OFF, "", "");
    }
}

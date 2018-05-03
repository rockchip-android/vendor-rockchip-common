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
import com.intel.amtl.modem.command.CommandParser;

public class TraceLegacyController extends ModemController {

    public TraceLegacyController() throws ModemControlException {
        super();
        AlogMarker.tAB("TraceLegacyController.TraceLegacyController", "0");
        AlogMarker.tAE("TraceLegacyController.TraceLegacyController", "0");
    }

    @Override
    public boolean queryTraceState() throws ModemControlException {
        AlogMarker.tAB("TraceLegacyController.queryTraceState", "0");
        AlogMarker.tAE("TraceLegacyController.queryTraceState", "0");
        return checkAtTraceState().equals("1");
    }

    @Override
    public String switchOffTrace() throws ModemControlException {
        AlogMarker.tAB("TraceLegacyController.switchOffTrace", "0");
        AlogMarker.tAE("TraceLegacyController.switchOffTrace", "0");
        return sendCommand("AT+TRACE=0\r\n");
    }

    @Override
    public void switchTrace(ModemConf mdmConf) throws ModemControlException {
        AlogMarker.tAB("TraceLegacyController.switchTrace", "0");
        sendCommand(mdmConf.getTrace());
        sendCommand(mdmConf.getXsystrace());
        AlogMarker.tAE("TraceLegacyController.switchTrace", "0");
    }

    @Override
    public String checkAtTraceState() throws ModemControlException {
        AlogMarker.tAB("TraceLegacyController.checkAtTraceState", "0");
        AlogMarker.tAE("TraceLegacyController.checkAtTraceState", "0");
        return CommandParser.parseTraceResponse(sendCommand("at+trace?\r\n"));
    }

    @Override
    public ModemConf getNoLoggingConf() {
        AlogMarker.tAB("TraceLegacyController.getNoLoggingConf", "0");
        AlogMarker.tAE("TraceLegacyController.getNoLoggingConf", "0");
        return ModemConf.getInstance("", "AT+TRACE=0\r\n", "AT+XSYSTRACE=0\r\n", "", "");
    }
}

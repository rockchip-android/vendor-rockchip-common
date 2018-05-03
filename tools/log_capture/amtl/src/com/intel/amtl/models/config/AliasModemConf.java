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

package com.intel.amtl.models.config;

import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.LogOutput;
import com.intel.amtl.models.config.ModemConf;

public class AliasModemConf extends ModemConf {

    public AliasModemConf(LogOutput config) {
        super(config);
        AlogMarker.tAB("AliasModemConf.AliasModemConf", "0");
        AlogMarker.tAE("AliasModemConf.AliasModemConf", "0");
    }

    public AliasModemConf(String xsio, String trace, String xsystrace, String flcmd,
            String octMode) {
        super(xsio, trace, xsystrace, flcmd, octMode);
        AlogMarker.tAB("AliasModemConf.AliasModemConf", "0");
        AlogMarker.tAE("AliasModemConf.AliasModemConf", "0");
    }

    @Override
    public boolean confTraceEnabled() {
        AlogMarker.tAB("AliasModemConf.confTraceEnabled", "0");
        AlogMarker.tAE("AliasModemConf.confTraceEnabled", "0");
        return !getProfileName().equals("all_off");
    }

    @Override
    public void activateConf(boolean activate) {
        AlogMarker.tAB("AliasModemConf.activateConf", "0");
        AlogMarker.tAE("AliasModemConf.activateConf", "0");
    }
}

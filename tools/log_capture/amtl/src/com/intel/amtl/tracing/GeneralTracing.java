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
 * Author: Nicolae Natea <nicolaex.natea@intel.com>
 */

package com.intel.amtl.tracing;

import android.view.View;

public interface GeneralTracing {
    public boolean start();
    public void stop();
    public void setState(boolean state);
    public boolean restart();
    public boolean isUpdated(View view);
    public void cleanTemp();
    public void saveTemp(String path);
    public boolean isRunning();
    public int getViewID();
    public void attachReferences(View view);
    public void attachListeners();
    public void updateConfiguration();
    public String getLastStatus();
    public String getTracerName();
}

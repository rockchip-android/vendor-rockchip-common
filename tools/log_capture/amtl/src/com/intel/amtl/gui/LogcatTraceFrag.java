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

package com.intel.amtl.gui;

import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.intel.amtl.R;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.tracing.LogcatTraces;

import java.io.FileOutputStream;

public class LogcatTraceFrag extends Fragment {

    private final String TAG = "AMTL";
    private final String MODULE = "LogcatTraceFrag";

    LogcatTraces lt;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        AlogMarker.tAB("LogcatTraceFrag.onCreate", "0");
        super.onCreate(savedInstanceState);
        AlogMarker.tAE("LogcatTraceFrag.onCreate", "0");
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
        AlogMarker.tAB("LogcatTraceFrag.onCreateView", "0");
        lt = new LogcatTraces();
        View view = inflater.inflate(lt.getViewID(), container, false);

        lt.attachReferences(view);
        AlogMarker.tAE("LogcatTraceFrag.onCreateView", "0");
        return view;
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        AlogMarker.tAB("LogcatTraceFrag.onViewCreated", "0");
        super.onViewCreated(view, savedInstanceState);

        lt.attachListeners();
        AlogMarker.tAE("LogcatTraceFrag.onViewCreated", "0");
    }

    @Override
    public void onResume() {
        AlogMarker.tAB("LogcatTraceFrag.onResume", "0");
        super.onResume();

        lt.restoreSelections();
        AlogMarker.tAE("LogcatTraceFrag.onResume", "0");
    }
}

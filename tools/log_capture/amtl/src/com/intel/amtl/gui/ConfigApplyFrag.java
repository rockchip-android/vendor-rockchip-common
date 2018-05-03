/* Android AMTL
 *
 * Copyright (C) Intel 2013
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
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 */
package com.intel.amtl.gui;


import android.app.Activity;
import android.app.Application;
import android.app.DialogFragment;
import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.SystemClock;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;

import com.intel.amtl.AMTLApplication;
import com.intel.amtl.R;
import com.intel.amtl.exceptions.ModemControlException;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.LogOutput;
import com.intel.amtl.models.config.ModemConf;
import com.intel.amtl.models.ConfigManager;
import com.intel.amtl.modem.controller.ModemController;


// Embedded class to handle delayed configuration setup (Dialog part).
public class ConfigApplyFrag extends DialogFragment {

    private final String TAG = "AMTL";
    private final String MODULE = "ConfigApplyFrag";

    private static final String EXTRA_TAG = "extra_tag";
    private static final String EXTRA_FRAG = "extra_frag";

    int targetFrag;
    String tag;

    ProgressBar confProgNot;
    // thread executed while Dialog Box is displayed.
    ApplyConfTask exeSetup;

    public static final ConfigApplyFrag newInstance(String tag, int targetFrag) {
        AlogMarker.tAB("ConfigApplyFrag.newInstance", "0");
        ConfigApplyFrag confApplyFrag = new ConfigApplyFrag();
        Bundle bdl = new Bundle(2);
        bdl.putString(EXTRA_TAG, tag);
        bdl.putInt(EXTRA_FRAG, targetFrag);
        confApplyFrag.setArguments(bdl);
        AlogMarker.tAE("ConfigApplyFrag.newInstance", "0");
        return confApplyFrag;
    }

    public void handlerConf(ApplyConfTask confTask) {
        // This allows to get ConfSetupTerminated on the specified Fragment.
        AlogMarker.tAB("ConfigApplyFrag.handlerConf", "0");
        this.exeSetup = confTask;
        this.exeSetup.setFragment(this);
        AlogMarker.tAE("ConfigApplyFrag.handlerConf", "0");
    }

    public void confSetupTerminated(String exceptReason) {
        // dismiss() is possible only if we are on the current Activity.
        // And will crash if we have switched to another one.
        AlogMarker.tAB("ConfigApplyFrag.confSetupTerminated", "0");
        if (isResumed()) {
            dismiss();
        }

        this.exeSetup = null;
        if (!exceptReason.equals("")) {
            Log.e(TAG, MODULE + ": modem conf application failed: " + exceptReason);
            UIHelper.okDialog(getActivity(),
                    "Error ", "Configuration not applied:\n" + exceptReason);
        }

        if (getTargetFragment() != null) {
            getTargetFragment().onActivityResult(targetFrag, Activity.RESULT_OK, null);
        }
        AlogMarker.tAE("ConfigApplyFrag.confSetupTerminated", "0");
    }

    public void launch(ModemConf modemConfToApply, boolean atProxyToStart, Fragment frag,
            FragmentManager gsfManager) {
        AlogMarker.tAB("ConfigApplyFrag.launch", "0");
        handlerConf(new ApplyConfTask(modemConfToApply, atProxyToStart));
        setTargetFragment(this, targetFrag);
        show(gsfManager, tag);
        AlogMarker.tAE("ConfigApplyFrag.launch", "0");
    }

    // Function overrides for the DialogFragment instance.
    @Override
    public void onCreate(Bundle savedInstanceState) {
        AlogMarker.tAB("ConfigApplyFrag.onCreate", "0");
        super.onCreate(savedInstanceState);
        setRetainInstance(true);
        this.tag = getArguments().getString(EXTRA_TAG);
        this.targetFrag = getArguments().getInt(EXTRA_FRAG);
        // Spawn the thread to execute modem configuration.
        if (this.exeSetup != null) {
            this.exeSetup.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        }
        AlogMarker.tAE("ConfigApplyFrag.onCreate", "0");
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        // Create dialog box.
        AlogMarker.tAB("ConfigApplyFrag.onCreateView", "0");
        View view = inflater.inflate(R.layout.fragment_task, container);
        confProgNot = (ProgressBar)view.findViewById(R.id.progressBar);
        getDialog().setTitle("Executing configuration");
        setCancelable(false);
        AlogMarker.tAE("ConfigApplyFrag.onCreateView", "0");

        return view;
    }

    @Override
    public void onDestroyView() {
        // This will allow dialog box to stay even if parent layout
        // configuration is changed (rotation)
        AlogMarker.tAB("ConfigApplyFrag.onDestroyView", "0");
        if (getDialog() != null && getRetainInstance()) {
            getDialog().setDismissMessage(null);
        }
        super.onDestroyView();
        AlogMarker.tAE("ConfigApplyFrag.onDestroyView", "0");
    }

    @Override
    public void onResume() {
        // This allows to close dialog box if the thread ends while
        // we are not focused on the activity.
        AlogMarker.tAB("ConfigApplyFrag.onResume", "0");
        super.onResume();
        if (this.exeSetup == null) {
            dismiss();
        }
        AlogMarker.tAE("ConfigApplyFrag.onResume", "0");
    }

    // embedded class to handle delayed configuration setup (thread part).
    public static class ApplyConfTask extends AsyncTask<Void, Void, Void> {
        private final String TAG = "AMTL";
        private final String MODULE = "ApplyConfTask";

        private ConfigApplyFrag confSetupFrag;
        private ModemController modemCtrl;
        private ModemConf modConfToApply;
        private boolean atProxyToStart = false;
        private String exceptReason = "";
        private AlogMarker m = new AlogMarker();

        public ApplyConfTask(ModemConf confToApply, boolean atProxyToStart) {
            AlogMarker.tAB("ConfigApplyFrag.ApplyConfTask.applyConf", "0");
            this.modConfToApply = confToApply;
            this.atProxyToStart = atProxyToStart;
            AlogMarker.tAE("ConfigApplyFrag.ApplyConfTask.applyConf", "0");
        }

        void setFragment(ConfigApplyFrag confAppFrag) {
            AlogMarker.tAB("ConfigApplyFrag.ApplyConfTask.setFragment", "0");
            confSetupFrag = confAppFrag;
            AlogMarker.tAE("ConfigApplyFrag.ApplyConfTask.setFragment", "0");
        }

        // Function overrides for Apply configuration thread.
        @Override
        protected Void doInBackground(Void... params) {
            AlogMarker.tAB("ConfigApplyFrag.ApplyConfTask.doInBackground", "0");
            SharedPreferences prefs =
                    AMTLApplication.getContext().getSharedPreferences("AMTLPrefsData",
                    Context.MODE_PRIVATE);
            if (null == prefs) {
                exceptReason = "cannot find preference";
                Log.e(TAG, MODULE + ": cannot update change modem configuration because "
                        + "preferences cannot be found");
                return null;
            }
            Editor editor = prefs.edit();
            int indexApplied = -1;
            try {
                modemCtrl = ModemController.getInstance();
                ConfigManager.applyAtProxyConfig(modConfToApply, atProxyToStart);
                indexApplied = ConfigManager.applyConfig(modConfToApply, modemCtrl);
            } catch (ModemControlException ex) {
                exceptReason = ex.getMessage();
                Log.e(TAG, MODULE + ": cannot change modem configuration " + ex);
                // if config change failed, apply modem default conf if defined or else deactivate
                // logging
                try {
                    LogOutput defaultOutput = AMTLApplication.getDefaultConf();
                    if (defaultOutput != null) {
                        modConfToApply = ModemConf.getInstance(AMTLApplication.getDefaultConf());
                        Log.d(TAG, MODULE + ": applying default conf");
                    } else {
                        modConfToApply = modemCtrl.getNoLoggingConf();
                        Log.d(TAG, MODULE + ": stopping logging");
                    }
                    indexApplied = ConfigManager.applyConfig(modConfToApply, modemCtrl);
                    // Everything went right, so let s commit trace configuration.
                } catch (ModemControlException mcex) {
                    Log.e(TAG, MODULE + ": failed to apply config " + mcex);
                }
            } finally {
                // store the configuration applied
                Log.d(TAG, MODULE + ": Configuration index to save: " + indexApplied);
                editor.putInt("index" + AMTLApplication.getCurrentModemName(), indexApplied);
                editor.commit();

                modemCtrl = null;
                modConfToApply = null;
            }
            AlogMarker.tAE("ConfigApplyFrag.ApplyConfTask.doInBackground", "0");
            return null;
        }

        @Override
        protected void onPostExecute(Void exception) {
            AlogMarker.tAB("ConfigApplyFrag.ApplyConfTask.onPostExecute", "0");
            if (confSetupFrag == null) {
                AlogMarker.tAE("ConfigApplyFrag.ApplyConfTask.onPostExecute", "0");
                return;
            }
            confSetupFrag.confSetupTerminated(exceptReason);
            AlogMarker.tAE("ConfigApplyFrag.ApplyConfTask.onPostExecute", "0");
        }
    }
}

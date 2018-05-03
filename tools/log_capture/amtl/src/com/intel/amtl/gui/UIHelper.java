/* Android Modem Traces and Logs
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
 * Author: Morgane Butscher <morganeX.butscher@intel.com>
 * Author: Erwan Bracq <erwan.bracq@intel.com>
 */

package com.intel.amtl.gui;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Color;
import android.preference.PreferenceManager;
import android.text.InputType;
import android.util.Log;
import android.view.inputmethod.InputMethodManager;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Switch;
import android.widget.TextView;

import com.intel.amtl.helper.LogManager;
import com.intel.amtl.helper.TelephonyStack;
import com.intel.amtl.log.AlogMarker;
import com.intel.amtl.models.config.ExpertConfig;
import com.intel.amtl.modem.controller.ModemController;
import com.intel.amtl.R;


public class UIHelper {

    static boolean isApChecked = false;
    static boolean isBpChecked = false;

    /* Print pop-up message with ok and cancel buttons */
    public static void warningDialog(final Activity A, String title, String message,
            DialogInterface.OnClickListener listener) {
        AlogMarker.tAB("UIHelper.warningDialog", "0");
        new AlertDialog.Builder(A)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", listener)
                .setNegativeButton("Cancel", listener)
                .show();
        AlogMarker.tAE("UIHelper.warningDialog", "0");
    }

    /* Print pop-up message with ok and cancel buttons */
    public static void warningExitDialog(final Activity A, String title, String message) {
        AlogMarker.tAB("UIHelper.warningExitDialog", "0");
        new AlertDialog.Builder(A)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        /* Nothing to do */
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        /* Exit application */
                        A.finish();
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.warningExitDialog", "0");
    }

    /* Print pop-up message with ok and cancel buttons to save */
    public static void chooseExpertFile(final Activity A, String title, String message,
            final Context context, final ExpertConfig expConf, final Switch expSwitch) {
        AlogMarker.tAB("UIHelper.chooseExpertFile", "0");
        final EditText saveInput = new EditText(context);
        saveInput.setText(expConf.getPath(), TextView.BufferType.EDITABLE);
        saveInput.setTextColor(Color.parseColor("#66ccff"));
        saveInput.setInputType(InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
        new AlertDialog.Builder(A)
                .setView(saveInput)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String confFilePath = saveInput.getText().toString();
                        InputMethodManager imm = (InputMethodManager)
                                A.getSystemService(Activity.INPUT_METHOD_SERVICE);
                        if (imm != null) {
                            imm.hideSoftInputFromWindow(saveInput.getWindowToken(), 0);
                        }
                        // if im is null, no specific issue, virtual keyboard will not be cleared
                        // parse file
                        expConf.setPath(confFilePath);
                        if (expConf.validateFile()) {
                            expConf.setConfigSet(true);
                        } else {
                            expConf.setConfigSet(false);
                            expSwitch.performClick();
                        }
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        ExpertConfig.setConfigSet(false);
                        expSwitch.performClick();
                    }
                })
                .setNeutralButton("Show", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String confFilePath = saveInput.getText().toString();
                        InputMethodManager imm = (InputMethodManager)
                                A.getSystemService(Activity.INPUT_METHOD_SERVICE);
                        if (imm != null) {
                            imm.hideSoftInputFromWindow(saveInput.getWindowToken(), 0);
                        }
                        // if im is null, no specific issue, virtual keyboard will not be cleared
                        // parse file
                        expConf.setPath(confFilePath);
                        if (expConf.validateFile()) {
                            expConf.setConfigSet(true);
                            if (!expConf.displayConf().equals("")) {
                                okCancelDialog(A, "Expert Config File", expConf, expSwitch);
                            }
                        } else {
                            expConf.setConfigSet(false);
                            expSwitch.performClick();
                        }
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.chooseExpertFile", "0");
    }

    /* Print pop-up message with ok and cancel buttons to save */
    public static void savePopupDialog(final Activity A, String title, String message,
            final Context context, final LogManager snaplog, final SaveLogFrag logProgressFrag) {
        final EditText saveInput = new EditText(context);
        saveInput.setText("snapshot", TextView.BufferType.EDITABLE);
        saveInput.setTextColor(Color.parseColor("#66ccff"));
        saveInput.setInputType(InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
        AlogMarker.tAB("UIHelper.savePopupDialog", "0");
        new AlertDialog.Builder(A)
                .setView(saveInput)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String snapshotTag = saveInput.getText().toString();
                        InputMethodManager imm = (InputMethodManager)
                            A.getSystemService(Activity.INPUT_METHOD_SERVICE);
                        if (imm != null) {
                            imm.hideSoftInputFromWindow(saveInput.getWindowToken(), 0);
                        }
                        // if im is null, no specific issue, virtual keyboard will not be cleared
                        snaplog.setTag(snapshotTag);
                        logProgressFrag.launch(snaplog);
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        /* Nothing to do */
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.savePopupDialog", "0");
    }

    /* Print pop-up message with ok and cancel buttons to save */
    public static void savePopupDialog(final Activity A, String title, String message,
            final String savepath, final Context context, final Runnable onOK,
            final Runnable onCancel) {
        AlogMarker.tAB("UIHelper.savePopupDialog", "0");
        final EditText saveInput = new EditText(context);
        saveInput.setText(savepath, TextView.BufferType.EDITABLE);
        saveInput.setTextColor(Color.parseColor("#66ccff"));
        saveInput.setInputType(InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
        new AlertDialog.Builder(A)
                .setView(saveInput)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String snapshotTag = saveInput.getText().toString();
                        InputMethodManager imm = (InputMethodManager)
                            A.getSystemService(context.INPUT_METHOD_SERVICE);
                        if (imm != null) {
                            imm.hideSoftInputFromWindow(saveInput.getWindowToken(), 0);
                        }

                        SharedPreferences sharedPrefs = PreferenceManager
                                .getDefaultSharedPreferences(context);
                        Editor sharedPrefsEdit = sharedPrefs.edit();
                        sharedPrefsEdit.putString(context.getString(
                                R.string.settings_user_save_path_key),
                                saveInput.getText().toString());
                        sharedPrefsEdit.commit();

                        onOK.run();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        SharedPreferences sharedPrefs = PreferenceManager
                                .getDefaultSharedPreferences(context);
                        Editor sharedPrefsEdit = sharedPrefs.edit();
                        sharedPrefsEdit.putString(context.getString(
                                R.string.settings_user_save_path_key), savepath);
                        sharedPrefsEdit.commit();

                        onCancel.run();
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.savePopupDialog", "0");
    }

    /* Print pop-up message with ok and cancel buttons to save */
    public static void cleanPopupDialog(final Activity A, String title, String message,
            final Runnable onOK, final Runnable onCancel) {
        AlogMarker.tAB("UIHelper.cleanPopupDialog", "0");
        new AlertDialog.Builder(A)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        onOK.run();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        onCancel.run();
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.cleanPopupDialog", "0");
    }

    /* Print pop-up message with ok and cancel buttons to save */
    public static void logTagDialog(final Activity A, String title, String message,
            final Context context) {
        AlogMarker.tAB("UIHelper.logTagDialog", "0");
        final EditText saveInput = new EditText(context);
        saveInput.setText("TAG_TO_SET_IN_LOGS", TextView.BufferType.EDITABLE);
        saveInput.setTextColor(Color.parseColor("#66ccff"));
        saveInput.setInputType(InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
        new AlertDialog.Builder(A)
                .setView(saveInput)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String logTag = saveInput.getText().toString();
                        InputMethodManager imm = (InputMethodManager)
                            A.getSystemService(Activity.INPUT_METHOD_SERVICE);
                        if (imm != null) {
                            imm.hideSoftInputFromWindow(saveInput.getWindowToken(), 0);
                        }
                        // if im is null, no specific issue, virtual keyboard will not be cleared
                        Log.d("AMTL", "UIHelper: " + logTag);
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        /* Nothing to do */
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.logTagDialog", "0");
    }

    /* Print pop-up message with ok button */
    public static void okDialog(Activity A, String title, String message) {
        AlogMarker.tAB("UIHelper.okDialog", "0");
        new AlertDialog.Builder(A)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        /* Nothing to do, waiting for user to press OK button */
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.okDialog", "0");
    }

    /* Print pop-up message with ok and cancel buttons */
    public static void okCancelDialog(Activity A, String title, final ExpertConfig conf,
            final Switch expSwitch) {
        AlogMarker.tAB("UIHelper.okCancelDialog", "0");
        String message = conf.displayConf();
        new AlertDialog.Builder(A)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        // validate conf
                        ExpertConfig.setConfigSet(true);
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        // uncheck expert switch
                        ExpertConfig.setConfigSet(false);
                        expSwitch.performClick();
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.okCancelDialog", "0");
    }

    /* Print a dialog before exiting application */
    public static void exitDialog(final Activity A, String title, String message) {
        AlogMarker.tAB("UIHelper.exitDialog", "0");
        new AlertDialog.Builder(A)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        A.finish();
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.exitDialog", "0");
    }

    /* Print pop-up message with ok and cancel buttons */
    public static void messageExitActivity(final Activity A, String title, String message) {
        AlogMarker.tAB("UIHelper.messageExitActivity", "0");
        new AlertDialog.Builder(A)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        A.finish();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        /* Nothing to do */
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.messageExitActivity", "0");
    }

    /* Print pop-up message with ok and cancel buttons, dedicated to telephony stack
     * property.
     */
    public static void messageSetupTelStack(final Activity A, String title, String message,
            final TelephonyStack telStackSetter) {
        AlogMarker.tAB("UIHelper.messageSetupTelStack", "0");
        new AlertDialog.Builder(A)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        /* Enable stack and exit. */
                        telStackSetter.enableStack();
                        exitDialog(A, "REBOOT REQUIRED.",
                            "As you enable telephony stack, AMTL will exit. "
                            + " Please reboot your device.");
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        /* We just exit. */
                        exitDialog(A, "Exit",
                            "AMTL will exit. Telephony stack is disabled.");
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.messageSetupTelStack", "0");
    }

    /* Print pop-up message with ok and cancel buttons to save */
    public static void evacLogDialog(final Activity A, String title, String message,
            final ModemController mdmCtrl, final Context context) {
        AlogMarker.tAB("UIHelper.evacLogDialog", "0");
        final String DEFAULT_TEXT = "Description of the issue";

        final EditText saveInput = new EditText(context);
        saveInput.setText(DEFAULT_TEXT, TextView.BufferType.EDITABLE);
        saveInput.setTextColor(Color.parseColor("#66ccff"));
        saveInput.setInputType(InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
        saveInput.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent motion) {
                if (saveInput.getText().toString().equals(DEFAULT_TEXT)) {
                    saveInput.setText("");
                }
                return false;
            }
        });

        final CheckBox apCheck = new CheckBox(context);
        apCheck.setText("AP");
        apCheck.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                isApChecked = buttonView.isChecked();
            }
        });

        final CheckBox bpCheck = new CheckBox(context);
        bpCheck.setText("BP");
        bpCheck.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                isBpChecked = buttonView.isChecked();
            }
        });

        LinearLayout linearLayout = new LinearLayout(context);
        linearLayout.setLayoutParams(new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.FILL_PARENT, LinearLayout.LayoutParams.FILL_PARENT));
        linearLayout.setOrientation(1);
        linearLayout.addView(apCheck);
        linearLayout.addView(bpCheck);
        linearLayout.addView(saveInput);

        new AlertDialog.Builder(A)
                .setView(linearLayout)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String issueCause = saveInput.getText().toString();
                        if (issueCause.equals(DEFAULT_TEXT)) {
                            issueCause = "";
                        }
                        mdmCtrl.notifyDebug(issueCause, isApChecked, isBpChecked);
                        isApChecked = false;
                        isBpChecked = false;
                        InputMethodManager imm = (InputMethodManager)
                            A.getSystemService(Activity.INPUT_METHOD_SERVICE);
                        if (imm != null) {
                            imm.hideSoftInputFromWindow(saveInput.getWindowToken(), 0);
                        }
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int whichButton) {
                        isApChecked = false;
                        isBpChecked = false;
                    }
                })
                .show();
        AlogMarker.tAE("UIHelper.savePopupDialog", "0");
    }
}

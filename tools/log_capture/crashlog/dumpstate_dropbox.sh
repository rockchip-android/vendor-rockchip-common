#!/system/bin/sh

# Do not allow bugreports on user builds unless USB debugging
# is enabled.
if [ "x$(getprop ro.build.type)" = "xuser" -a \
     "x$(getprop init.svc.adbd)" != "xrunning" ]; then
  exit 0
fi

storagePath=`getprop crashlogd.storage.path`
if [ ! -d "$storagePath" ] ; then
  exit 0
fi

# Wait 5 seconds, monkey may run procrank and librank after ANR crash
sleep 5
timestamp=`date +'%Y-%m-%d-%H-%M-%S'`
dumpstate=$storagePath/dumpstate-$timestamp.txt
dropbox=$storagePath/dropbox-$timestamp.txt

# run dumpstate and dropbox
/system/bin/bugreport > $dumpstate
/system/bin/dumpsys dropbox --print > $dropbox

# make files readable
if [[ $storagePath == /mnt/sdcard/logs/crashlog* ]] ; then
  chown root.sdcard_rw $dumpstate
  chown root.sdcard_rw $dropbox
else
  chown system.log $dumpstate
  chown system.log $dropbox
fi

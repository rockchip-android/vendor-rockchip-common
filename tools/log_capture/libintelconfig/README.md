Intel Config library {#mainpage}
=========
This library aims to provide an unified mode of handling complex configuration formats.

To get or refresh the full documentation run "doxygen doc/Doxyfile" and check doc/html/index.html

See \ref API for details.

Supported formats
----------------
### JSON ####
As described on http://www.json.org/

#### Limitations: ####
1. Unicode escaped sequences '\\uxxxx' are ignored
2. Only integer (decimal or hex) numerical values.
3. Booleans are integer values 1/0

#### Notes: ####
 Since the root of JSON can only be an "value" or "array of values" an root node named "iconfig-root" will hold this value.

Tests
-----
### AOSP ###
#### Host ####
run:

* host$ mmma vendor/intel/tools/log_capture/libintelconfig/tests/
* host$ iconfig_host_tests

#### Target ####
run:

 * host$ mmma vendor/intel/tools/log_capture/libintelconfig/tests/
 * host$ adb sync data
 * target$ /data/nativetest/iconfig_tests/iconfig_tests

### Host (no aosp) ###

 * run make in libintelconfig root
 * execute out/intelconfig_test


TODO:
----
* Add extra formats
* Improve reporting of syntax errors

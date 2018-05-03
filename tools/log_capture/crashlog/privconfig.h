/* Copyright (C) Intel 2013
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file privconfig.h
 * @brief File containing every constants definitions used in crashlogd source
 * code such as properties name, files name, directories name, various fields...
 */

#ifndef __CRASHLOGD_H__
#define __CRASHLOGD_H__

#include <sys/param.h>

#define LOG_TAG                 "CRASHLOG"
#include "log.h"

#ifndef MAX
#define MAX(a,b)                (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b)                (((a)<(b))?(a):(b))
#endif

/* CONSTRAINTS */
#define KB                      (1024)
#define MB                      (KB*KB)
#define MAX_RECORDS             5000
#define MAX_RECORDS_HIST_FILE   (MAX_RECORDS + 1000)
#define HIST_FILE_HEADER_SIZE   2
#define MAX_DIRS                1000
#define PATHMAX                 512
#define UPTIME_HOUR_FREQUENCY   6
#define TIMESTAMP_MAX_SIZE      10 /* Unix style timestamp on 10 characters max.*/
#define MAXFILESIZE             (10*MB)
/* find_str_in_file was implemented with 4KB*/
#define MAXLINESIZE             MAX((2 * PROPERTY_VALUE_MAX), (4 * KB))
#define CPBUFFERSIZE            (4*KB)
#define SIZE_FOOTPRINT_MAX      ((PROPERTY_VALUE_MAX + 1) * 11)
#define TIMEOUT_VALUE           (20*1000)
#define MAX_WAIT_MMGR_CONNECT_SECONDS  5
#define MMGR_CONNECT_RETRY_TIME_MS     200
#define UPTIME_FREQUENCY        (5 * 60)
#define MMGRMAXSTRING           (20)
#define MMGRMAXEXTRA            (512)
#define KCT_MAX_CONNECT_TRY      10
#define KCT_CONNECT_RETRY_TIME_S 2
#define UPTIME_MAX_LENGTH       11
#define STARTUP_REASON_LENGTH   48
/* crashlog wd timeout in seconds */
#define CRASHLOG_WD_TIMEOUT     120
#define CRASHLOG_WD_GRANULARITY 8
#define CRASHLOG_SELECT_TIMEOUT 60

/* Percentage of guaranteed available space checked by crashlogd
 * to determine whether to generate new crash logs
 */
#define LOGSDIR_MINIMUM_FREESPACE_PERCENT     5
#define SDCARD_MINIMUM_FREESPACE_PERCENT      5

/* FIELDS DEFINITIONS */
#define PERM_USER               "system"
#define PERM_GROUP              "log"
#define STARTUP_STR             "androidboot.wakesrc="
#define PER_UPTIME              "UPTIME"
#define USBBOGUS                "USBBOGUS"
#define ERROREVENT              "ERROR"
#define CRASHEVENT              "CRASH"
#define STATSEVENT              "STATS"
#define INFOEVENT               "INFO"
#define STATEEVENT              "STATE"
#define APLOGEVENT              "APLOG"
#define BZEVENT                 "BZ"
#define KERNEL_CRASH            "IPANIC"
#define KERNEL_SWWDT_CRASH      "IPANIC_SWWDT"
#define KERNEL_HWWDT_CRASH      "IPANIC_HWWDT"
#define KERNEL_FAKE_CRASH       "IPANIC_FAKE"
#define KERNEL_SWWDT_FAKE_CRASH "IPANIC_SWWDT_FAKE"
#define IPANIC_CORRUPTED        "IPANIC_CORRUPTED"
#define KDUMP_CRASH             "KDUMP"
#define RAMDUMP_EVENT           "RAMDUMP"
#define BZTRIGGER               "bz_trigger"
#define SCREENSHOT_PATTERN      "SCREENSHOT="
#define APLOG_DEPTH_DEF         "5"
#define APLOG_NB_PACKET_DEF     "1"
#define BZMANUAL                "MANUAL"
#define SYS_REBOOT              "REBOOT"
#define FABRIC_ERROR            "FABRICERR"
#define RECOVERY_ERROR          "RECOVERY_ERROR"
#define CRASHLOG_ERROR_DEAD     "CRASHLOG_DEAD"
#define CRASHLOG_ERROR_DECRYPT  "DECRYPT_ERROR"
#define CRASHLOG_ERROR_PATH     "CRASHLOG_PATH"
#define CRASHLOG_ERROR_FULL     "LOG_PARTITION_FULL"
#define CRASHLOG_ERROR_MODEMS   "MULTIPLE_MODEMS"
#define CRASHLOG_SWWDT_MISSING  "SWWDT_MISSING"
#define SYSSERVER_EVNAME        "UIWDT"
#define ANR_EVNAME              "ANR"
#define TOMBSTONE_EVNAME        "TOMBSTONE"
#define JAVACRASH_EVNAME        "JAVACRASH"
#define JAVA_TOMBSTONE_EVNAME   "JAVA_TOMBSTONE"
#define APCORE_EVNAME           "APCOREDUMP"
#define LOST_EVNAME             "LOST_DROPBOX"
#define HPROF_EVNAME            "HPROF"
#define STATSTRIG_EVNAME        "STTRIG"
#define APLOGTRIG_EVNAME        "APLOGTRIG"
#define CMDTRIG_EVNAME          "CMDTRIG"
#define UPTIME_EVNAME           "CURRENTUPTIME"
#define MDMCRASH_EVNAME         "MPANIC"
#define VMMTRAP_EVNAME          "VMMTRAP"
#define BOOTLOGS_EVNAME         "BOOT_LOGS"
#define EXTRA_NAME              "EXTRA"
#define NOTIFY_CONF_PATTERN     "INOTIFY"
#define GENERAL_CONF_PATTERN    "GENERAL"
#define VMMTRAP_CONF_PATTERN    "VMMTRAP"
#define MPANIC_ABORT            "COREDUMP_ABORTED_BY_PLATFORM_SHUTDOWN"
#define CRASHLOG_WATCHER_ERROR  "CRASHLOG_WATCHER"
#define RAMCONSOLE              "RAMCONSOLE"
#define DEVICE_ID_UNKNOWN       "UnknownId"
#define FAKE_EVENT_SUFFIX       "_FAKE"
#define LOST_EVENT_SUFFIX       "_LOST"

/*
 * Take care that this enum is ALWAYS aligned with
 * the array declared in inotify_handler.c
 * The type value is identical to the index in the table
 */
enum {
    LOST_TYPE = 0,
    SYSSERVER_TYPE,
    ANR_TYPE,
    TOMBSTONE_TYPE,
    JAVATOMBSTONE_TYPE,
    JAVACRASH_TYPE,
    JAVACRASH_TYPE2,
    APCORE_TYPE,
    HPROF_TYPE,
    STATTRIG_TYPE,
    INFOTRIG_TYPE,
    ERRORTRIG_TYPE,
    APLOGTRIG_TYPE,
    CMDTRIG_TYPE,
    UPTIME_TYPE,
    EVENT_TYPE_NUMBER, /* !!! Take care this enum item is always the last one */
};

/*
 * Array to print string value of enum defined here above for logging
 * purpose only Take care this array is aligned with the enum type
 */
static const char* print_eventtype[EVENT_TYPE_NUMBER] = {
    "LOST_TYPE",
    "SYSSERVER_TYPE",
    "ANR_TYPE",
    "TOMBSTONE_TYPE",
    "JAVATOMBSTONE_TYPE",
    "JAVACRASH_TYPE",
    "JAVACRASH_TYPE2",
    "APCORE_TYPE",
    "HPROF_TYPE",
    "STATTRIG_TYPE",
    "INFOTRIG_TYPE",
    "ERRORTRIG_TYPE",
    "APLOGTRIG_TYPE",
    "CMDTRIG_TYPE",
    "UPTIME_TYPE"
};

enum {
    APLOG_TYPE = 0,
    BPLOG_TYPE,
    APLOG_STATS_TYPE,
    BPLOG_STATS_TYPE,
    KDUMP_TYPE,
    BPLOG_TYPE_OLD,
};
/*Define uptime mode for raise_event function */
enum {
    NO_UPTIME = 0,
    UPTIME,
    UPTIME_BOOT,
};

typedef unsigned int bool;
enum { FALSE = 0, TRUE = 1, };

/* Define crashlogd running modes */
enum crashlog_mode {
    NOMINAL_MODE = 0, /*< boot mode is MAIN : standard behavior */
    RAMDUMP_MODE,     /*< boot mode is RAMCONSOLE : only checks some crashes, dump fw files and do a cold reset */
    MINIMAL_MODE,     /*< boot mode different from MAIN OS (COS, ROS, POS...) : minimal checks, no watcher */
};

/* Struct defining a crashlog mode configuration
 * New attributes to be added when needed
 */
struct mode_config {
    char *name;
    bool sdcard_storage:1;
    bool notifs_crashreport:1;
    bool monitor_crashenv:1;
    bool watched_event_types[EVENT_TYPE_NUMBER];
    bool mmgr_enabled:1;
};

#ifdef CONFIG_COREDUMP
#define COREDUMP_WATCHED TRUE
#else
#define COREDUMP_WATCHED FALSE
#endif

/* Array defining for each crashlog mode, the associated config */
static const struct mode_config get_mode_configs[] = {
    [ NOMINAL_MODE ] = {
        .name = "NOMINAL MODE",
        .sdcard_storage = CONFIG_USE_SD,
        .notifs_crashreport = TRUE,
        .monitor_crashenv = TRUE,
        .watched_event_types = {
            [ LOST_TYPE ... JAVACRASH_TYPE2 ] = TRUE,
            [ APCORE_TYPE ... HPROF_TYPE ] = COREDUMP_WATCHED,
            [ STATTRIG_TYPE ... UPTIME_TYPE ] = TRUE },
        .mmgr_enabled = TRUE,
    },
    [ RAMDUMP_MODE ] = {
        .name = "RAMDUMP MODE",
        .sdcard_storage = FALSE,
        .notifs_crashreport = FALSE,
        .monitor_crashenv = FALSE,
        .watched_event_types = { [ LOST_TYPE ... UPTIME_TYPE ] = FALSE, }, /* No directories are watched */
        .mmgr_enabled = FALSE,
    },
    [ MINIMAL_MODE ] = {
        .name = "MINIMAL MODE",
        .sdcard_storage = CONFIG_USE_SD,
        .notifs_crashreport = FALSE,
        .monitor_crashenv = FALSE,
        .watched_event_types = {
            [ LOST_TYPE ... HPROF_TYPE ] = FALSE, /* Watch only stat directory */
            [ STATTRIG_TYPE  ] = TRUE,
            [ INFOTRIG_TYPE ... UPTIME_TYPE ] = FALSE },
        .mmgr_enabled = FALSE,
    },
};

/* MACROS */
#define CRASHLOG_MODE_NAME(mode) \
    ((mode > MINIMAL_MODE) ? "" : get_mode_configs[mode].name)
#define CRASHLOG_MODE_SD_STORAGE(mode) \
    ((mode > MINIMAL_MODE) ? 0 : get_mode_configs[mode].sdcard_storage)
#define CRASHLOG_MODE_NOTIFS_ENABLED(mode) \
    ((mode > MINIMAL_MODE) ? 0 :get_mode_configs[mode].notifs_crashreport)
#define CRASHLOG_MODE_MONITOR_CRASHENV(mode) \
    ((mode > MINIMAL_MODE) ? 0 : get_mode_configs[mode].monitor_crashenv)
#define CRASHLOG_MODE_EVENT_TYPE_ENABLED(mode, type) \
    ((mode > MINIMAL_MODE || type > UPTIME_TYPE) ? \
     0 : get_mode_configs[mode].watched_event_types[type])
#define CRASHLOG_MODE_MMGR_ENABLED(mode) \
    ((mode > MINIMAL_MODE) ? 0 : get_mode_configs[mode].mmgr_enabled)

/* global flag indicating crashlogd mode (main.c) */
extern enum crashlog_mode g_crashlog_mode;

/* DIM returns the dimention of an array */
#define DIM(array)              ( sizeof(array) / sizeof(array[0]) )

/* PROPERTIES */
#define PROP_DATA_QUOTA         "persist.crashlogd.data_quota"
#define PROP_LOGS_ROOT          "persist.crashlogd.root"
#define PROP_CRASH_MODE         "persist.sys.crashlogd.mode"
#define PROP_PROFILE            "persist.service.profile.enable"
#define PROP_PROC_ONGOING       "crashlogd.processing.ongoing"
#define PROP_BOOTREASON         "ro.boot.bootreason"
#define PROP_BOOT_STATUS        "sys.boot_completed"
#define PROP_BUILD_FIELD        "ro.build.version.incremental"
#define PROP_BOARD_FIELD        "ro.product.model"
#define KERNEL_FIELD            "sys.kernel.version"
#define USER_FIELD              "ro.build.user"
#define HOST_FIELD              "ro.build.host"
#define IFWI_FIELD              "sys.ifwi.version"
#define SCUFW_VERSION           "sys.scu.version"
#define PUNIT_VERSION           "sys.punit.version"
#define IAFW_VERSION            "sys.ia32.version"
#define VALHOOKS_VERSION        "sys.valhooks.version"
#define FINGERPRINT_FIELD       "ro.build.fingerprint"
#define MODEM_FIELD             "gsm.version.baseband"
#define MODEM_FIELD2            "gsm.version.baseband1"
#define IMEI_FIELD              "persist.radio.device.imei"
#define MODEM_SCENARIO          "persist.radio.multisim.config"
#define OPERATOR_FIELD          "gsm.operator.alpha"
#define PROP_COREDUMP           "persist.core.enabled"
#define PROP_CRASH_MODE         "persist.sys.crashlogd.mode"
#define PROP_ANR_USERSTACK      "persist.anr.userstack.disabled"
#define PROP_IPANIC_PATTERN     "persist.crashlogd.panic.pattern"
#define PROP_APLOG_DEPTH        "persist.crashreport.aplogdepth"
#define PROP_APLOG_NB_PACKET    "persist.crashreport.packet"
#define PROP_LOGSYSTEMSTATE     "init.svc.logsystemstate"
#define PROP_RELEASE_VERSION    "ro.build.version.release"
#define PROP_SYS_BKC_VERSION    "ro.build.version.incremental"
#define PROP_SOC_VERSION        "ro.board.platform"
#define PROP_REPORT_FAKE        "crashreport.events.fake"
#define PROP_REPORT_COUNTDOWN   "crashreport.events.countdown"
#define PROP_REPORT_GENERIC_FAKE "persist.crashlogd.events.fake"
#define PROP_ISTP_CLEAN         "persist.crashlogd.istp_clean"
/** Control whether boot logs are stored. "1" disables boot logs. */
#define PROP_BOOT_LOG_DISABLED  "persist.crashlogd.no_bootlog"

/* DIRECTORIES */
#ifndef __LINUX__
#define RESDIR                  ""
#else
#define RESDIR                  "res"
#endif

#ifndef CONFIG_LOGS_PATH
#define LOGS_DIR                RESDIR "/logs"
#else
#define LOGS_DIR                RESDIR CONFIG_LOGS_PATH
#endif

#define EXTSDCARD_DIR           RESDIR "/mnt/media_rw"

#define SDCARD_DIR              RESDIR "/mnt/sdcard"
#define PROC_DIR                RESDIR "/proc"
#define DATA_DIR                RESDIR "/data"
#define SYSTEM_DIR              RESDIR "/system"
#define SYS_DIR                 RESDIR "/sys"
#define CACHE_DIR               RESDIR "/cache"
#define PSTORE_DIR              RESDIR "/sys/fs/pstore"
#define DEBUGFS_DIR             RESDIR "/d"
#define DEV_DIR                 RESDIR "/dev"
#define PANIC_DIR               DATA_DIR "/dontpanic"
#define SDCARD_LOGS_DIR         SDCARD_DIR "/logs"
#define SDCARD_CRASH_DIR        SDCARD_LOGS_DIR "/crashlog"
#define EMMC_CRASH_DIR          LOGS_DIR "/crashlog"
#define SDCARD_STATS_DIR        SDCARD_LOGS_DIR "/stats"
#define EMMC_STATS_DIR          LOGS_DIR "/stats"
#define SDCARD_APLOGS_DIR       SDCARD_LOGS_DIR "/aplogs"
#define EMMC_APLOGS_DIR         LOGS_DIR "/aplogs"
#define SDCARD_BZ_DIR           SDCARD_LOGS_DIR "/bz"
#define EMMC_BZ_DIR             LOGS_DIR "/bz"
#define LOGINFO_DIR             LOGS_DIR "/info"
#define STAT_DIR                LOGS_DIR "/stats"
#define APLOG_DIR               LOGS_DIR "/aplogs"
#define DROPBOX_DIR             DATA_DIR "/system/dropbox"
#define TOMBSTONE_DIR           DATA_DIR "/tombstones"
#define LOGS_MEDIA_DIR          DATA_DIR "/media/logs"
#define KDUMP_DIR               LOGS_MEDIA_DIR "/kdump"
#define SDSIZE_CURRENT_LOG      LOGS_DIR "/currentsdsize"
#define REBOOT_DIR              DEBUGFS_DIR "/intel_scu_osnib"
#define EVENTS_DIR              LOGS_DIR "/events"
#define EFI_DIR                 SYS_DIR "/firmware/efi"
#define EFI_EFIVARS_DIR         EFI_DIR "/efivars"
#define FACTORY_PARTITION_DIR   RESDIR "/factory"

/* FILES */
#define SYS_PROP                SYSTEM_DIR "/build.prop"
#define HISTORY_FILE            LOGS_DIR "/history_event"
#define UPTIME_FILE             LOGS_DIR "/uptime"
#define BZ_CURRENT_LOG          LOGS_DIR "/currentbzlog"
#define CRASH_CURRENT_LOG       LOGS_DIR "/currentcrashlog"
#define STATS_CURRENT_LOG       LOGS_DIR "/currentstatslog"
#define APLOGS_CURRENT_LOG      LOGS_DIR "/currentaplogslog"
#define LOG_UUID                LOGS_DIR "/uuid.txt"
#define LOG_BUILDID             LOGS_DIR "/buildid.txt"
#define LOG_MODEM_VERSION_BASE  LOGS_DIR "/modem_version"
#define APLOG_FILE_0            LOGS_DIR "/aplog"
#define APLOG_FILE_1            LOGS_DIR "/aplog.1"
#define BPLOG_FILE_0            LOGS_DIR "/bplog"
#define BPLOG_FILE_1            LOGS_DIR "/bplog.1"
#define BPLOG_FILE_1_OLD        LOGS_DIR "/bplog.1.istp"
#define BPLOG_FILE_2_OLD        LOGS_DIR "/bplog.2.istp"
#define BPLOG_FILE_0_EXT        ".istp"
#define BPLOG_FILE_1_EXT        ".1.istp"
#define BPLOG_FILE_1_OLD_EXT    ".1.istp"
#define BPLOG_FILE_2_OLD_EXT    ".2.istp"
#define LOGRESERVED             LOGS_DIR "/reserved"
#define HISTORY_UPTIME          LOGS_DIR "/uptime"
#define HISTORY_CORE_DIR        DATA_DIR "/core"
#define LOGS_GPS_DIR            LOGS_DIR "/gps"
#define APLOG_FILE_BOOT         LOGS_DIR "/aplog_boot"
#define LOG_SPID                LOGS_DIR "/spid.txt"
#define LOG_PANICTEMP           LOGS_DIR "/panic_temp"
#define LOG_FABRICTEMP           LOGS_DIR "/fabric_temp"
#define CONSOLE_NAME            "console"
#define LAST_KMSG_FILE          "last_kmsg"
#define CONSOLE_RAMOOPS_FILE    "console-ramoops"
#define FTRACE_RAMOOPS_FILE     "ftrace-ramoops"
#define EMMC_HEADER_NAME        "emmc_ipanic_header"
#define EMMC_CONSOLE_NAME       "emmc_ipanic_console"
#define CONSOLE_RAM             "ram_ipanic_console"
#define THREAD_NAME             "emmc_ipanic_threads"
#define LOGCAT_NAME             "emmc_ipanic_logcat"
#define FABRIC_ERROR_NAME       "ipanic_fabric_err"
#define FABRIC_RECOV_NAME       "ipanic_fabric_recv_err"
#define OFFLINE_SCU_LOG_NAME    "offline_scu_log"
#define ONLINE_SCU_LOG_NAME     "online_scu_log"
#define GBUFFER_NAME            "emmc_ipanic_gbuffer"
#define CMDLINE_NAME            "cmdline"
#define CRASHFILE_NAME          "crashfile"
#define LAST_KMSG               PROC_DIR "/" LAST_KMSG_FILE
#define CONSOLE_RAMOOPS         PSTORE_DIR "/" CONSOLE_RAMOOPS_FILE
#define FTRACE_RAMOOPS          PSTORE_DIR "/" FTRACE_RAMOOPS_FILE
#define PANIC_HEADER_NAME       PROC_DIR "/" EMMC_HEADER_NAME
#define PANIC_CONSOLE_NAME      PROC_DIR "/" EMMC_CONSOLE_NAME
#define PANIC_THREAD_NAME       PROC_DIR "/" THREAD_NAME
#define PANIC_GBUFFER_NAME      PROC_DIR "/" GBUFFER_NAME
#define PROC_FABRIC_ERROR_NAME  PROC_DIR "/" FABRIC_ERROR_NAME
#define PROC_FABRIC_RECOV_NAME  PROC_DIR "/" FABRIC_RECOV_NAME
#define PROC_MOUNTS             PROC_DIR "/mounts"
#define PROC_OFFLINE_SCU_LOG_NAME PROC_DIR "/" OFFLINE_SCU_LOG_NAME
#define PROC_ONLINE_SCU_LOG_NAME PROC_DIR "/" ONLINE_SCU_LOG_NAME
#define KERNEL_CMDLINE          PROC_DIR "/" CMDLINE_NAME
#define PROC_UUID               PROC_DIR "/emmc0_id_entry"
#define SYS_BLK_MMC0_CID        SYS_DIR "/block/mmcblk0/device/cid"
#define SAVED_HEADER_NAME       PANIC_DIR "/" EMMC_HEADER_NAME
#define SAVED_CONSOLE_NAME      PANIC_DIR "/" EMMC_CONSOLE_NAME
#define SAVED_THREAD_NAME       PANIC_DIR "/" THREAD_NAME
#define SAVED_GBUFFER_NAME      PANIC_DIR "/" GBUFFER_NAME
#define SAVED_LOGCAT_NAME       PANIC_DIR "/" LOGCAT_NAME
#define RECOVERY_ERROR_TRIGGER  CACHE_DIR "/recovery/recoveryfail"
#define RECOVERY_ERROR_LOG      CACHE_DIR "/recovery/last_log"
#define KDUMP_START_FLAG        KDUMP_DIR "/kdumphappened"
#define KDUMP_FILE_NAME         KDUMP_DIR "/kdumpfile.core"
#define KDUMP_FINISH_FLAG       KDUMP_DIR "/kdumpfinished"
#define KDUMP_CRASH_DIR         LOGS_MEDIA_DIR "/crashlog"
#define ANR_DUPLICATE_INFOERROR         "anr_duplicate_infoevent"
#define ANR_DUPLICATE_DATA              "anr_duplicate_data.txt"
#define UIWDT_DUPLICATE_DATA            "uiwdt_duplicate_data.txt"
#define JAVACRASH_DUPLICATE_DATA        "javacrash_duplicate_data.txt"
#define JAVACRASH_DUPLICATE_INFOERROR   "javacrash_duplicate_infoevent"
#define UIWDT_DUPLICATE_INFOERROR       "uiwdt_duplicate_infoevent"
#define CRASHLOG_WATCHER_INFOEVENT      "crashlog_watcher_infoevent"
#define CRASHLOG_CONF_PATH              SYSTEM_DIR "/vendor/etc/crashlog.conf"
#define MCD_PROCESSING          LOGS_DIR "/mcd_processing"
#define RESET_SOURCE_0          REBOOT_DIR "/RESETSRC0"
#define RESET_SOURCE_1          REBOOT_DIR "/RESETSRC1"
#define RESET_IRQ_1             REBOOT_DIR "/RESETIRQ1"
#define RESET_IRQ_2             REBOOT_DIR "/RESETIRQ2"
#define EVENTFILE_NAME          "eventfile"
#define IPTRAK_FILE             LOGS_DIR "/iptrak"
#define FW_UPDATE_STATUS_PATH   "/sys/firmware/osnib/fw_update_status"
#define INGREDIENTS_CONFIG      SYSTEM_DIR "/vendor/etc/ingredients.conf"
#define INGREDIENTS_FILE        LOGS_DIR "/ingredients.txt"
#define FACTORY_SUM_FILE        LOGS_DIR "/factory_sum"
#define PYTIMECHART_FILE        SYSTEM_DIR "/vendor/bin/pytimechart-record"
#define DUMPSTATE_DROPBOX_FILE  SYSTEM_DIR "/vendor/bin/dumpstate_dropbox.sh"
#define LOGCATEXT_PATH          SYSTEM_DIR "/vendor/bin/logcatext"
#define LM_DUMP_FILE            DEBUGFS_DIR "/lm_dump/lkm_buf1"
#define EARLY_LOGS_PATH         CACHE_DIR "/elogs"
#define EARLY_LOGS_FILENAME     "elog"
#define EARLY_LOGS_OLD_FILENAME "elog-old"
#define EARLY_LOGS_FILE         EARLY_LOGS_PATH "/" EARLY_LOGS_FILENAME
#define EARLY_LOGS_OLD_FILE     EARLY_LOGS_PATH "/" EARLY_LOGS_OLD_FILENAME
#define BOOT_LOGS_BACKUP_FILENAME     "log_boot"
#define MONITOR_CRASHENV_FILE   SYSTEM_DIR "/vendor/bin/monitor_crashenv"
#define NPKT_PATH               DEBUGFS_DIR"/npkt_dump"
#define NPKT_MTB                NPKT_PATH"/mtb"
#define NPKT_MTB_REC            NPKT_PATH"/mtb_recovery"
#define NPKT_CSR                NPKT_PATH"/csr"
#define NPKT_CSR_REC            NPKT_PATH"/csr_recovery"
#define DEV_MEM_FILE            DEV_DIR "/mem"
#define ACPI_BERT_TABLE_FILE    SYS_DIR "/firmware/acpi/tables/BERT"
#define TRAP_INFO               "trap_info"
#define TRAP_INFO_FILE          LOGS_DIR "/" TRAP_INFO

/* SYSTEM COMMANDS */
#define SDSIZE_SYSTEM_CMD "du -sk " SDCARD_LOGS_DIR "/ > " LOGS_DIR "/currentsdsize"

#ifndef CONFIG_NUM_MODEMS
#define MAX_MMGR_INST     (2)
#else
#define MAX_MMGR_INST     (CONFIG_NUM_MODEMS)
#endif

extern char *CRASH_DIR;
extern char *STATS_DIR;
extern char *APLOGS_DIR;
extern char *BZ_DIR;
extern char CURRENT_PANIC_CONSOLE_NAME[PATHMAX];
extern char CURRENT_PANIC_HEADER_NAME[PATHMAX];
extern char CURRENT_PROC_FABRIC_ERROR_NAME[PATHMAX];
extern char CURRENT_PROC_OFFLINE_SCU_LOG_NAME[PATHMAX];
extern char CURRENT_PROC_ONLINE_SCU_LOG_NAME[PATHMAX];
extern char CURRENT_KERNEL_CMDLINE[PATHMAX];
extern int gmaxfiles;
extern bool pytimechartrecord_filepresent;
extern bool logsystemstate_available;

#endif /* __CRASHLOGD_H__ */


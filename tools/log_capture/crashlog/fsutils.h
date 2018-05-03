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
 * @file fsutils.h
 * @brief File containing functions for basic operations on files.
 *
 * This file contains the functions for every basic operations on files such as
 * reading, writing, copying, pattern search, deletion, mode change...etc.
 */

#ifndef __FSUTILS_H__
#define __FSUTILS_H__

#include <cutils/properties.h>

#include <privconfig.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

/* Modes used for get_sdcard_paths */
typedef enum e_dir_mode {
    MODE_CRASH = 0,
    MODE_CRASH_NOSD,
    MODE_STATS,
    MODE_APLOGS,
    MODE_BZ,
    MODE_KDUMP,
} e_dir_mode_t;

typedef enum e_aplog_file {
    APLOG,
    APLOG_BOOT,
} e_aplog_file_t;

typedef enum e_match {
    MATCH_EXACT,
    MATCH_PATTERN,
} e_match;

/* Mode used to cache a file into a buffer*/
#define CACHE_TAIL      0
#define CACHE_START     1

/* returns a negative value on error or the number of lines read */
/*
* Name          : cache_file
* Description   : copy source file lines into buffer.
*                 returns a negative value on error or the number of lines read
*                 offset value is used to skip the file's first lines
*/
int cache_file(char *filename, char **records, int maxrecords, int cachemode, int offset);

static inline int file_exists(const char *filename) {
    struct stat info;

    return (stat(filename, &info) == 0);
}

static inline int directory_exists(char *path) {
    struct stat info;

    return (stat(path, &info) == 0 && S_ISDIR(info.st_mode));
}

static inline int dir_exists(const char *dirpath) {
    DIR * dir;

    dir = opendir(dirpath);
    if (dir != NULL)
        return 1;
    else
        return 0;
}

static inline int get_file_size(char *filename) {
    struct stat info;

    if (filename == NULL) return -ENOENT;

    if (stat(filename, &info) < 0) {
        return -errno;
    }

    return info.st_size;
}

int read_file_prop_uid(const char* propsource, const char *filename, char *uid, char* defaultvalue);
int find_new_crashlog_dir(e_dir_mode_t mode);
char *generate_crashlog_dir(e_dir_mode_t mode, char *unique);
int get_sdcard_paths(e_dir_mode_t mode);
void do_log_copy(char *mode, char *dir, const char* ts, int type);
void do_bplog_copy(char *mode, char *dir, const char* ts, int type, int instance);
long get_sd_size();
int sdcard_allowed();

int count_lines_in_file(const char *filename);
int find_matching_file(const char *dir_to_search, const char *pattern, char *filename_found);
int get_value_in_file(char *file, char *keyword, char *value, unsigned int sizemax);
int find_str_in_file(const char *filename, const char *keyword, const char *tail);
int find_str_in_standard_file(char *filename, char *keyword, char *tail);
int find_oneofstrings_in_file(const char *filename, const char *keywords[], int nbkeywords);
int find_oneofstrings_in_file_with_keyword(char *filename, char **keywords, char *common_keyword,int nbkeywords);
void flush_aplog(e_aplog_file_t file, const char *mode, char *dir, const char *ts);
void reset_file(const char *filename);
int readline(int fd, char buffer[MAXLINESIZE]);
int freadline(FILE *fd, char buffer[MAXLINESIZE]);
int append_file(char *filename, char *text);
int overwrite_file(char *filename, char *value);

mode_t get_mode(const char *s);
int do_chmod(char *path, char *mode);
int do_chown(const char *file, char *uid, char *gid);
int do_copy_eof(const char *src, const char *des);
int do_copy_eof_dir(const char *srcdir, const char *dstdir);
int do_copy_tail(char *src, char *dest, int limit);
int do_copy_utf16(const char *src, const char *des);
int do_copy(char *src, char *dest, int limit);
int do_mv(char *src, char *dest);
int do_mv_in_dir(char *src, char *dest_dir);
ssize_t do_read(int fd, void *buf, size_t len);
int rmfr(char *path);

/**
 * Removes matching files and folders given a partial path
 *
 * @param match indicates the partial path to a folder or file
 * @return returns the number of files or folders that matched
 *         the given pattern and a remove request was attempted.
 */
int rmfr_match(const char *match);

/**
 * Selectively removes files and folders within a given a path.
 *
 * @param path to the directory or file on which it should be acted upon
 * @param remove_entry indicates whether the path passed should be removed
 *        if it represents a directory
 * @param remove_subdirs indicates whether subdirectories in the given path,
 *        when if it is a directory, should be removed. Ignored if
 *        remove_entry is 1
 * @return 0 on success, -1 and errno set on failure
 */
int rmfr_specific(char *path, int remove_entry, int remove_subdirs);

int str_simple_replace(char *str, char *search, char *replace);
int get_parent_dir( char * dir, char *parent_dir );

char *compute_bp_log(const char* ext_file, int instance);
void copy_bplogs(const char *extra, char *dir, int limit, int instance);
void save_startuplogs(const char *reboot_id);
void activate_logcatext_collection();

/**
 * Read a one word string from file
 *
 * @param file path
 * @param string to be allocated before
 * @return string length, 0 if none, -errno code if error
 */
int file_read_string(const char *file, char *string);

/**
 * Search if a directory contains a file name, could be exact or partial
 *
 * @param dir to search in
 * @param filename to look at
 * @param exact macth of file name or partial
 * @return number of matched files, -errno on errors
 */
int dir_contains(const char *dir, const char *filename, bool exact);

/**
 * Reads the content of a file passed as parameter into a buffer
 *
 * @param path to the file from which we are supposed to read
 * @param buffer in which to write the read data
 * @param buffer_size indicates the maximum data to be read, in bytes
 * @return 0 on success, -errno on errors
 */
int read_binary_file(const char *path, unsigned char *buffer, unsigned int buffer_size);

/**
 * Writes the content of a buffer to file
 *
 * @param path to the file to write to
 * @param buffer from which it should try to write the data
 * @param buffer_size indicates how many bytes starting from 'buffer' are to be writen
 * @return number of written bytes, -errno on errors
 */
int write_binary_file(const char *path, const unsigned char *buffer, unsigned int buffer_size);

/**
 * @brief Generates an empty file
 *
 * @param filename indicates full file path to the new file
 * @return 0 on success, -errno on failure
 */
int create_empty_file(char* filename);

/**
 * @brief Copy log to a given folder if this log exists
 *
 * @param src       path to the source file
 * @param dst_dir   directory to copy @p src_file to
 *
 * @return 0 on success
 */
int copy_log_simple(const char *src, const char *dst_dir);

/**
 * @brief Copy log to a given folder if this log exists
 *
 * @param src_dir   directory to the source file
 * @param src_file  filename or pattern for the file to copy
 * @param match     whether @p src_file is an exact match or a pattern
 * @param dst_dir   directory to copy @p src_file to
 * @param dst_file  destination filename
 * @param to_utf8   convert @p src_file from UTF16 to UTF8 when set to true
 *
 * @return 0 on success
 */
int copy_log(const char *src_dir, const char *src_file, e_match match,
             const char *dst_dir, const char *dst_file, bool to_utf8);

/**
 * Read full content of a file passed as parameter into a buffer,
 * buffer is allocated by this function on success, must be freed by
 * caller, size of the buffer is passed in return.
 *
 * @param path to the file from which we are supposed to read
 * @param size of the allocated buffer
 * @param data pointer to allocated buffer which contains read data from file
 * @return 0 on success, -1 on error with errno set
 */
int read_full_binary_file(const char *path, unsigned long *size, void **data);

/**
 * Read a memory region from /dev/mem interface. Region is defined by
 * its address and size and is returned in data buffer.
 *
 * @param addr region's address
 * @param size region's size
 * @param data buffer which contains read data from region
 * @return 0 on success, -1 on error with errno set
 */
int read_dev_mem_region(unsigned long addr, unsigned long size, void *data);

#endif /* __FSUTILS_H__ */

/* Copyright (C) Intel 2015
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

#define LOG_TAG "DvC-Desc"

#include <dvcdesc.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#ifdef ANDROID
#include <cutils/log.h>

#define OPTIONS "alg:s:"

enum log_type {
	LOG_STDIO,
	LOG_ANDROID
};

static enum log_type __log_type = LOG_STDIO;

#else

#define OPTIONS "lg:s:"

#endif

#define MAX_LOG_SIZE 4096 /* 1 kernel page */

static void __log(const char *level, const char *fmt, ...)
{
	char tmp[MAX_LOG_SIZE];
	va_list al;

	va_start(al, fmt);
	vsnprintf(tmp, MAX_LOG_SIZE, fmt, al);
	va_end(al);

#ifdef ANDROID
	if (__log_type == LOG_ANDROID) {
		switch (level[0]) {
		case 'E':
			ALOGE("%s", tmp);
			break;
		case 'W':
			ALOGW("%s", tmp);
			break;
		case 'I':
			ALOGI("%s", tmp);
			break;
		default:
			ALOGD("%s", tmp);
		}
	} else {
#endif
		printf("%s: %s", level, tmp);
#ifdef ANDROID
	}
#endif
}

#define LOGD(...)   __log("D", __VA_ARGS__)
#define LOGI(...)   __log("I", __VA_ARGS__)
#define LOGW(...)   __log("W", __VA_ARGS__)
#define LOGE(...)   __log("E", __VA_ARGS__)

#ifndef DVC_FAKE_BUS
#define DVC_BUS_ROOT_READ "/sys/bus/dvctrace"
#define DVC_BUS_ROOT_WRITE "/sys/bus/dvctrace"
#else
#define DVC_BUS_ROOT_READ "test/bus"
#define DVC_BUS_ROOT_WRITE "test/busw"
#endif

#define MAX_CONTENT_SIZE 4096 /* 1 kernel page */

static void print_usage(const char *app)
{
	printf("Usage:\n");
	printf("%s -l: List available devices\n", app);
	printf("%s -g <dev>: Get the device descriptors\n", app);
	printf("%s -s <dev> <file>: Set the device descriptors\n", app);
#ifdef ANDROID
	printf("%s -a: Use android logging\n", app);
#endif
}

static void list_devs(void)
{
	DIR *dir;
	struct dirent *de;

	dir = opendir(DVC_BUS_ROOT_READ "/devices");
	if (!dir) {
		LOGE("Cannot open " DVC_BUS_ROOT_READ "/devices");
		return;
	}

	while ((de = readdir(dir))) {
		if (de->d_name[0] == '.')
			continue;
		if (de->d_type & (DT_DIR | DT_LNK))
			LOGI("%s\n", de->d_name);
	}
	closedir(dir);
}

static int get_file_content(const char *path, char *buf, size_t max_size)
{
	int ret, fd;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		LOGE("Cannot open %s", path);
		return fd;
	}

	ret = read(fd, buf, max_size);
	close(fd);

	return ret;
}

static int get_dev_node_content(const char *dev, const char *node, char *buf,
				size_t max_size)
{
	int ret;
	char *fname;

	ret = asprintf(&fname, DVC_BUS_ROOT_READ "/devices/%s/%s", dev, node);
	if (ret < 0) {
		LOGE("Cannot allocate name memory for %s", dev);
		return ret;
	}

	ret = get_file_content(fname, buf, max_size - 1);
	if (ret >= 0)
		buf[ret] = 0;

	free(fname);
	return ret;
}

static int set_file_content(const char *path, char *buf, size_t size)
{
	int  ret, fd;

	fd = open(path, O_WRONLY);
	if (fd < 0) {
		LOGE("Cannot open %s", path);
		return fd;
	}

	ret = write(fd, buf, size);
	close(fd);

	return ret;
}

static int set_dev_node_content(const char *dev, const char *node, char *buf,
				size_t size)
{
	int ret;
	char *fname;

	ret = asprintf(&fname, DVC_BUS_ROOT_WRITE "/devices/%s/%s", dev, node);
	if (ret < 0) {
		LOGE("Cannot allocate name memory for %s", dev);
		return ret;
	}

	ret = set_file_content(fname, buf, size);

	free(fname);
	return ret;
}

static struct dvc_descriptors *from_dev(const char *dev)
{
	char buf[MAX_CONTENT_SIZE];
	struct dvc_descriptors *drep;
	int ret;

	ret = get_dev_node_content(dev, "descriptors", buf, MAX_CONTENT_SIZE);

	if (ret <= 0) {
		LOGW("No descriptors for %s\n", dev);
		return NULL;
	}

	drep = dvc_alloc();
	if (!drep) {
		LOGE("Cannot allocate descriptors representation\n");
		return NULL;
	}

	ret = dvc_set_ascii_hex(drep, buf);
	if (ret < 0) {
		LOGE("Cannot set ascii hex (%d)\n", ret);
		dvc_free(drep);
		return NULL;
	}

	ret = get_dev_node_content(dev, "strings", buf, MAX_CONTENT_SIZE);
	if (ret <= 0) {
		LOGW("No strings for %s\n", dev);
		dvc_free(drep);
		return NULL;
	}

	ret = dvc_set_string_table(drep, buf);
	if (ret <= 0)
		LOGW("Cannot set string table (%d)\n", ret);

	return drep;
}

static struct dvc_descriptors *from_cfg_file(const char *fname)
{
	char buf[MAX_CONTENT_SIZE];
	struct dvc_descriptors *drep;
	int ret;

	ret = get_file_content(fname, buf, MAX_CONTENT_SIZE - 1);
	if (ret <= 0) {
		LOGW("No descriptors in %s", fname);
		return NULL;
	}
	buf[ret] = 0;

	drep = dvc_alloc();
	if (!drep) {
		LOGE("Cannot allocate descriptors representation\n");
		return NULL;
	}

	ret = dvc_set_config_string(drep, buf);
	if (ret < 0) {
		LOGE("Cannot load config from file %s (%d)\n", fname, ret);
		dvc_free(drep);
		return NULL;
	}

	return drep;
}

static void get(const char *dev)
{
	char buf[MAX_CONTENT_SIZE];
	struct dvc_descriptors *drep;
	int ret;

	drep = from_dev(dev);
	if (!drep) {
		LOGE("Cannot allocate descriptors representation\n");
		return;
	}

	ret = dvc_get_config_string(drep, buf, MAX_CONTENT_SIZE);
	if (ret > 0)
		LOGI("%s", buf);
	else
		LOGE("Cannot get config representation\n");

	dvc_free(drep);
}

static void set(const char *dev, const char *file)
{
	char buf[MAX_CONTENT_SIZE];
	struct dvc_descriptors *drep;
	int ret;

	drep = from_cfg_file(file);
	if (!drep) {
		LOGE("Cannot get descriptors from config\n");
		return;
	}

	ret = dvc_get_ascii_hex(drep, buf, MAX_CONTENT_SIZE);
	if (ret <= 0) {
		LOGE("Cannot get ascii hex (%d)", ret);
		goto err_desc;
	}

	ret = set_dev_node_content(dev, "descriptors", buf, strlen(buf));
	if (ret <= 0) {
		LOGE("Cannot write descriptors for %s (%d)", dev, ret);
		goto err_desc;
	}

	ret = dvc_get_string_table(drep, buf, MAX_CONTENT_SIZE);
	if (ret <= 0) {
		LOGE("Cannot get string table (%d)", ret);
		goto err_desc;
	}

	ret = set_dev_node_content(dev, "strings", buf, strlen(buf));
	if (ret <= 0) {
		LOGE("Cannot write strings for %s (%d)", dev, ret);
		goto err_desc;
	}

err_desc:
	dvc_free(drep);
}

int main(int argc, char **argv)
{
	char opt;

	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		switch (opt) {
#ifdef ANDROID
		case 'a':
			__log_type = LOG_ANDROID;
			break;
#endif
		case 'l':
			list_devs();
			return EXIT_SUCCESS;
		case 'g':
			LOGI("Get %s", optarg);
			get(optarg);
			return EXIT_SUCCESS;
		case 's':
			/*we need two args*/
			if (optind >= argc) {
				print_usage(argv[0]);
				return EXIT_FAILURE;
			}
			LOGI("Set %s, %s", optarg, argv[optind]);
			set(optarg, argv[optind]);
			return EXIT_SUCCESS;
		default: /* '?' */
			print_usage(argv[0]);
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>

#include "crashutils.h"
#include "fsutils.h"
#include "privconfig.h"

#define ACPI_NAME_SIZE 4
#define ACPI_SIG_BERT "BERT"
#define ACPI_BERT_REGION_STRUCT_SIZE (5 * sizeof(uint32_t))

/* BERT (Boot Error Record Table) as defined in ACPI spec, APEI chapter */
struct acpi_table_bert {
    char signature[ACPI_NAME_SIZE];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    char asl_compiler_id[ACPI_NAME_SIZE];
    uint32_t asl_compiler_revision;
    uint32_t region_length;
    uint64_t address;
} __attribute__((packed));

static int bert_check_table(struct acpi_table_bert *bert_tab) {
    if (strncmp(bert_tab->signature, ACPI_SIG_BERT, ACPI_NAME_SIZE))
        return -1;
    if (bert_tab->length != sizeof(struct acpi_table_bert))
        return -1;
    if (bert_tab->region_length != 0 &&
        bert_tab->region_length < ACPI_BERT_REGION_STRUCT_SIZE)
        return -1;

    return 0;
}

int do_bertdump_copy(const char *dir) {
    int ret = -1, fd;
    unsigned long size;
    struct acpi_table_bert *bert_tab;
    void *bert_region;
    char dstfile[PATHMAX];

    if (!file_exists(ACPI_BERT_TABLE_FILE)) {
        LOGE("%s: %s not available, abort.\n", __func__, ACPI_BERT_TABLE_FILE);
        return ret;
    }

    if (!file_exists(DEV_MEM_FILE)) {
        LOGE("%s: %s not available, abort.\n", __func__, DEV_MEM_FILE);
        return ret;
    }

    if (read_full_binary_file(ACPI_BERT_TABLE_FILE, &size,
                              (void **)&bert_tab)) {
        LOGE("%s: BERT file load failed : %s (%d)\n",
             __func__, strerror(errno), errno);
        return ret;
    }

    if (bert_check_table(bert_tab)) {
        LOGE("%s: bad data in BERT table\n", __func__);
        goto free_bert_tab;
    }

    if (bert_tab->region_length == 0) {
        LOGI("%s: no data in BERT region to dump\n", __func__);
        ret = 0;
        goto free_bert_tab;
    }

    bert_region = malloc(bert_tab->region_length);
    if (!bert_region) {
        LOGE("%s: BERT region malloc failed, requested size: %d\n",
             __func__, bert_tab->region_length);
        goto free_bert_tab;
    }

    if (read_dev_mem_region(bert_tab->address, bert_tab->region_length,
                            bert_region)) {
        LOGE("%s: BERT region dump failed : %s (%d)\n",
             __func__, strerror(errno), errno);
        goto free_bert_region;
    }

    snprintf(dstfile, sizeof(dstfile), "%s/fwerr_bert-dump_%s.bin",
             dir, get_current_time_short(0));

    fd = open(dstfile, O_WRONLY | O_CREAT, 0660);
    if (fd == -1) {
        LOGE("%s: Failed to open %s\n", __func__, dstfile);
        goto free_bert_region;
    }

    if (write(fd, bert_tab, sizeof(struct acpi_table_bert)) == -1) {
        LOGE("%s: Failed to write BERT table to %s : %s (%d)\n",
             __func__, dstfile, strerror(errno), errno);
        goto close;
    }

    if (write(fd, bert_region, bert_tab->region_length) == -1) {
        LOGE("%s: Failed to write BERT region to %s : %s (%d)\n",
             __func__, dstfile, strerror(errno), errno);
        goto close;
    }

    ret = 0;

  close:
    close(fd);
    do_chown(dstfile, PERM_USER, PERM_GROUP);
  free_bert_region:
    free(bert_region);
  free_bert_tab:
    free(bert_tab);
    return ret;
}

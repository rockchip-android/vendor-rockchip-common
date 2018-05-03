/* Copyright (C) Intel 2014
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
 * @file ingredients.c
 * @brief File containing functions used to handle operations on
 * ingredients.txt file.
 *
 */

#include "ingredients.h"
#include "privconfig.h"
#include "fsutils.h"
#include "config.h"
#include "getbulkprops.h"
#include <ctype.h>
#include <cutils/properties.h>
#include <stdlib.h>

#ifdef CONFIG_EFILINUX
#include <libdmi.h>
#endif

#define UNDEF_INGR "unknown"
#define INGREDIENT_VALUE_MAX_SIZE   PROPERTY_VALUE_MAX

struct {
    const char* match;
    const char* property;
    char value[INGREDIENT_VALUE_MAX_SIZE];
} modem_config[] = {
    {"Modem", MODEM_FIELD, UNDEF_INGR},
    {"ModemExt", MODEM_FIELD2, UNDEF_INGR}
};

static bool ingredients_disabled = FALSE;

static void get_modem_section_name(char *name, pconfig_handle handle);

static pconfig_handle parse_ingredients_file(const char *file_path) {
    pconfig_handle pc_handle;

    pc_handle = malloc(sizeof(struct config_handle));
    if (!pc_handle)
        return NULL;

    pc_handle->first = NULL;
    pc_handle->current = NULL;

    if (init_config_file(file_path, pc_handle) < 0) {
        free_config_file(pc_handle);
        free(pc_handle);
        LOGE("[INGR] Cannot load ingredients config, path = %s", file_path);
        return NULL;
    }

    return pc_handle;
}

static void check_config(pconfig_handle handle) {
    psection c_psection;
    pkv c_kv;

    c_psection = handle->first;
    while (c_psection) {
        c_kv = c_psection->kvlist;
        while (c_kv) {
            if (strncmp(c_kv->value, "true", strlen("true")) &&
                strncmp(c_kv->value, "false", strlen("false")))
                LOGW("[INGR] Invalid key value (%s) for %s",
                     c_kv->value, c_kv->key);
            c_kv = c_kv->next;
        }
        c_psection = c_psection->next;
    }
}

#ifdef CONFIG_EFILINUX
static int fetch_dmi_properties(psection section) {
    int status = 1;
    char *property;
    pkv kv = section->kvlist;
    while (kv) {
        property = libdmi_getfield(INTEL_SMBIOS, kv->key);

        free(kv->value);

        if (!property) {
            kv->value = strdup(UNDEF_INGR);
            status = 0;
        } else {
            kv->value = strdup(property);
        }
        kv = kv->next;
    }
    return status;
}
#else
static int fetch_dmi_properties(psection section __attribute__((__unused__))) {
    return 0;
}
#endif

static int fetch_android_properties(psection section) {
    int status = 1;
    char buffer[INGREDIENT_VALUE_MAX_SIZE];
    pkv kv = section->kvlist;
    while (kv) {
        if (property_get(kv->key, buffer, UNDEF_INGR) < 0) {
            status = 0;
        }
        free(kv->value);
        kv->value = strdup(buffer);

        kv = kv->next;
    }
    return status;
}

static int fetch_bulk_android_properties(psection section) {
    int status = 1;

    pkv kv = section->kvlist;
    while (kv) {
        char *props_json = retrieve_props_json(kv->key);

        if (props_json == NULL) {
            status = 0;
            props_json = strdup("null");
        }

        free(kv->value);
        kv->value = props_json;

        kv = kv->next;
    }
    return status;
}

void load_modem_config(pconfig_handle handle) {
    char modem_section_name[PROPERTY_VALUE_MAX];
    int instance = DIM(modem_config);

    if (!handle)
        return;

    get_modem_section_name(modem_section_name, handle);

    while(instance--) {
        char *value = get_value(modem_section_name, (char *)modem_config[instance].match, handle);
        if (!value)
            continue;

        snprintf(modem_config[instance].value, sizeof(modem_config[instance].value), "%s", value);
    }
}

static int update_modem_name(int instance) {
    char property[PROPERTY_VALUE_MAX];

    if (property_get(modem_config[instance].property, property, UNDEF_INGR) <= 0)
        LOGV("Property %s not readable\n", modem_config[instance].property);
    else if (strcmp(property, UNDEF_INGR) && strcmp(modem_config[instance].value, property)) {
        snprintf(modem_config[instance].value, sizeof(modem_config[instance].value),
                "%s", property);
        return 1;
    }

    return 0;
}

int conditional_ingredients_refresh() {
    int instance = DIM(modem_config);

    while(instance--) {
        if (update_modem_name(instance) > 0) {
            check_ingredients_file();
            return 1;
        }
    }

    return 0;
}

int fetch_modem_name(int instance, char **name) {
    int status = 0;
    *name = UNDEF_INGR;

    if (instance >= (int)DIM(modem_config) || instance < 0)
        return -EINVAL;

    *name = modem_config[instance].value;

    if (update_modem_name(instance)) {
        check_ingredients_file();
        status = 1;
    }

    if (!strcmp(*name, UNDEF_INGR))
        return -1;

    return status;
}

static int get_modem_property(char *property_name, char **value) {
    int status = 0;
    char property[PROPERTY_VALUE_MAX];
    int instance = DIM(modem_config);
    *value = UNDEF_INGR;

    while(instance--) {
        if (strncmp(property_name, modem_config[instance].match,
            strlen(modem_config[instance].match) + 1))
            continue;

        update_modem_name(instance);
        *value = modem_config[instance].value;
    }

    if (!strcmp(*value, UNDEF_INGR))
        status = -1;

    if (status < 0)
        LOGW("Cannot get modem prop - %s", property_name);

    return status;
}

static int fetch_modem_properties(psection section) {
    char *buffer;
    pkv kv = section->kvlist;
    LOGW("[INGR]: modem Props ");
    while (kv) {
        get_modem_property(kv->key, &buffer);
        free(kv->value);
        kv->value = strdup(buffer);

        kv = kv->next;
    }
    return 1;
}

static void get_modem_section_name(char *name, pconfig_handle handle) {
    char value[PROPERTY_VALUE_MAX];
    unsigned int index;

    property_get(MODEM_SCENARIO, value, "UNKNOWN");
    if (strcmp(value, "UNKNOWN") == 0) {
        sprintf(name, "MODEM");
        return;
    }

    for(index = 0; index < strlen(value); index++)
        value[index] = toupper(value[index]);

    snprintf(name, PROPERTY_VALUE_MAX, "MODEM.%s", value);
    if (find_section(name, handle) != NULL)
        return;

    sprintf(name, "MODEM");
}

static int fetch_ingredients(pconfig_handle handle) {
    psection c_psection;
    char modem_section_name[PROPERTY_VALUE_MAX];
    int fetch_result = 1;

    get_modem_section_name(modem_section_name, handle);
    c_psection = handle->first;
    while (c_psection) {
        if (!strncmp(c_psection->name, "LIBDMI", sizeof("LIBDMI"))
            && fetch_dmi_properties(c_psection))
            fetch_result = 0;

        if (!strncmp(c_psection->name, "GETPROP", sizeof("GETPROP"))
            && fetch_android_properties(c_psection) <= 0)
            fetch_result = 0;

        if (!strncmp(c_psection->name, "GETBULKPROPS", sizeof("GETBULKPROPS"))
            && fetch_bulk_android_properties(c_psection) <= 0)
            fetch_result = 0;

        if (!strncmp(c_psection->name, modem_section_name, strlen(modem_section_name)+1)
            && fetch_modem_properties(c_psection) <= 0)
            fetch_result = 0;

        c_psection = c_psection->next;
    }
    return fetch_result;
}

void check_ingredients_file() {
    static pconfig_handle old_values = NULL;
    pconfig_handle new_values = NULL;

    if (ingredients_disabled)
        return;

    if (!file_exists(INGREDIENTS_CONFIG)) {
        LOGE("[INGR]: File '%s' not found, disable 'ingredients' feature\n",
             INGREDIENTS_CONFIG);
        ingredients_disabled = TRUE;
        return;
    }

    if (!old_values) {
        /*first run, load last ingredients.txt */
        old_values = parse_ingredients_file(INGREDIENTS_FILE);
        load_modem_config(old_values);
    }

    new_values = parse_ingredients_file(INGREDIENTS_CONFIG);

    if (!new_values) {
        LOGW("[INGR]: Cannot load config file");
        return;
    }

    check_config(new_values);

    fetch_ingredients(new_values);

    /*old vs new */
    if (cmp_config(old_values, new_values)) {

        LOGI("[INGR]: Updating %s", INGREDIENTS_FILE);
        if (dump_config(INGREDIENTS_FILE, new_values)) {
            LOGI("[INGR]: Cannot update %s", INGREDIENTS_FILE);
            if (new_values) {
                free_config_file(new_values);
                free(new_values);
            }
        } else {
            if (old_values) {
                free_config_file(old_values);
                free(old_values);
            }
            old_values = new_values;
            do_chmod(INGREDIENTS_FILE, "644");
            do_chown(INGREDIENTS_FILE, PERM_USER, PERM_GROUP);
        }
    } else if (new_values) {
        LOGI("[INGR]: No diff between %p and %p", old_values, new_values);
        free_config_file(new_values);
        free(new_values);
    }
}

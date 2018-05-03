#include "log.h"
#include "privconfig.h"

#include <regex.h>
#include <cutils/properties.h>

struct list {
   char *value;
   struct list *next;
};
typedef struct list prop_item;

struct list_walker {
   prop_item *head;
   prop_item *last;
   char *filter;
   int space_allocated;
   int nb_elements;
};
typedef struct list_walker prop_walker;

static int is_match(const char *input, char *match) {
    regex_t re;
    if (regcomp(&re, match, 0) != 0) {
        LOGE("Could not compile regular expression: %s\n", match);
        return 0;
    }
    int ret = regexec(&re, input, 0, NULL, 0);
    regfree(&re);
    if (!ret) {
        return 1;
    }
    return 0;
}

static void filter_properties(const char* key, const char* name, void* storage)
{
    prop_walker* props_list = storage;
    if (is_match(key, props_list->filter) == 0) {
        return;
    }

    if (props_list->head == NULL) {
        props_list->head = props_list->last = malloc(sizeof(prop_item));
    } else {
        props_list->last->next = malloc(sizeof(prop_item));
        props_list->last = props_list->last->next;
    }

    if (props_list->last == NULL) {
        LOGE("Could not allocat memory for props!\n");
        return;
    }

    props_list->last->next = NULL;
    if (!asprintf(&props_list->last->value, "\"%s\":\"%s\"", key, name)) {
        props_list->last->value = "out of memory";
        LOGE("Could not insert property in list!\n");
    }
    props_list->space_allocated += strlen(props_list->last->value);
    props_list->nb_elements++;
}

static prop_walker *list_properties(char *filter) {
    prop_walker *list = malloc(sizeof(prop_walker));
    if (list == NULL) {
        LOGE("Could not set up prop walker!\n");
        return NULL;
    }
    list->head = NULL;
    list->filter = filter;
    list->space_allocated = 0;
    list->nb_elements = 0;

    (void)property_list(filter_properties, list);

    return list;
}

static void free_properties_list(prop_walker *list) {
    prop_item *head = list->head;
    while (head != NULL) {
        prop_item *temp = head;
        free(head->value);
        head = head->next;
        free(temp);
    }
    free(list);
}

char *retrieve_props_json(char* filter) {
    prop_walker *props;
    const char *separator = ",";
    const char *json_start = "{";
    const char *json_end = "}";
    char *ret;
    int size;

    if (filter == NULL)
        return NULL;

    props = list_properties(filter);
    if (props == NULL)
        return NULL;

    /* allocate space for creating a null terminated json from the found properties. */
    size = (((props->nb_elements > 0) ? props->nb_elements : 1) - 1) * strlen(separator) +
            strlen(json_start) + strlen(json_end) + props->space_allocated + 1;
    ret = malloc(size);
    if (ret == NULL) {
        LOGE("Could not allocate space for props json\n");
        free_properties_list(props);
        return NULL;
    }

    strncpy (ret, json_start, size - 1);
    prop_item *head = props->head;
    while (head != NULL) {
        strncat (ret, head->value, size - strlen(ret) - 1);
        head = head->next;
        if (head) strncat (ret, separator, size - strlen(ret) - 1);
    }
    strncat (ret, json_end, size - strlen(ret) - 1);
    free_properties_list(props);

    return ret;
}

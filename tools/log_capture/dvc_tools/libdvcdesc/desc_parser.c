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

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <linux/usb/ch9.h>
#include "dvcdesc_internal.h"
#include "parser.h"
#include "desc_parser.h"

struct pair *new_num_pair(char *name, unsigned long long number)
{
	struct pair *ret = malloc(sizeof(*ret));

	if (!ret)
		return NULL;

	ret->name = name;
	ret->next = NULL;
	ret->type = P_NUM;
	ret->v.num = number;
	return ret;
}

struct pair *new_str_pair(char *name, char *str)
{
	struct pair *ret = malloc(sizeof(*ret));

	if (!ret)
		return NULL;

	ret->name = name;
	ret->next = NULL;
	ret->type = P_STRING;
	ret->v.str = str;
	return ret;
}

struct pair *new_arr_pair(char *name, struct num_array *arr)
{
	struct pair *ret = malloc(sizeof(*ret));

	if (!ret)
		return NULL;

	ret->name = name;
	ret->next = NULL;
	ret->type = P_NUM_ARRAY;
	ret->v.num_array = arr;
	return ret;
}

struct pair_list *new_pair_list(struct pair *pair)
{
	struct pair_list *ret;

	if (!pair)
		return NULL;

	ret = malloc(sizeof(*ret));
	if (!ret)
		return NULL;
	ret->first = pair;
	ret->last = pair;
	return ret;
}

struct pair_list *pair_list_add(struct pair_list *pl, struct pair *pair)
{
	if (!pair || !pl)
		return NULL;
	pl->last->next = pair;
	pl->last = pair;
	return pl;
}

struct num_array *new_arr()
{
	struct num_array *ret = malloc(sizeof(*ret));

	if (!ret)
		return NULL;

	ret->len = 0;
	ret->elems = NULL;
	return ret;
}

struct num_array *arr_add(struct num_array *arr, unsigned long long number)
{
	unsigned long long *old;

	if (!arr)
		return NULL;

	old = arr->elems;
	arr->len++;
	arr->elems = calloc(arr->len, sizeof(*arr->elems));
	if (!arr->elems) {
		/*free anything, it will be lost in the parser othewise*/
		free(old);
		free(arr);
		return NULL;
	}
	if (old) {
		memcpy(arr->elems + 1, old,
		       (arr->len - 1) * sizeof(*arr->elems));
		free(old);
	}
	arr->elems[0] = number;
	return arr;
}

static struct pair *get_value(struct pair_list *pl, const char *name)
{
	struct pair *current;

	for (current = pl->first; current; current = current->next) {
		if (!strcmp(name, current->name))
			return current;
	}
	return NULL;
}

static void make_oconn_desc(struct dvc_descriptors *descs, struct pair_list *pl)
{
	struct pair *tval;
	struct dvc_oconn_desc *desc;

	desc = malloc(sizeof(*desc));
	if (!desc)
		return;

	memset(desc, 0, sizeof(*desc));

	desc->bLength = DC_OUTPUT_CONNECTION_SIZE;
	desc->bDescriptorType = USB_DT_CS_INTERFACE;
	desc->bDescriptorSubtype = DC_OUTPUT_CONNECTION;

	tval = get_value(pl, "bConnectionID");
	if (!tval)
		goto cleanup;
	desc->bConnectionID = tval->v.num & 0xff;

	tval = get_value(pl, "bConnectionType");
	if (!tval)
		goto cleanup;
	desc->bConnectionType = tval->v.num & 0xff;

	tval = get_value(pl, "bAssocConnection");
	if (!tval)
		goto cleanup;
	desc->bAssocConnection = tval->v.num & 0xff;

	tval = get_value(pl, "wSourceID");
	if (!tval)
		goto cleanup;
	desc->wSourceID = tval->v.num & 0xffff;

	/*iConnection*/
	tval = get_value(pl, "iConnection");
	if (tval)
		desc->sConnection = strdup(tval->v.str);

	add_oconn_desc(descs, desc);
	return;

cleanup:
	free(desc);
}

static void make_iconn_desc(struct dvc_descriptors *descs, struct pair_list *pl)
{
	struct pair *tval;
	struct dvc_iconn_desc *desc;

	desc = malloc(sizeof(*desc));
	if (!desc)
		return;

	memset(desc, 0, sizeof(*desc));

	desc->bLength = DC_INPUT_CONNECTION_SIZE;
	desc->bDescriptorType = USB_DT_CS_INTERFACE;
	desc->bDescriptorSubtype = DC_INPUT_CONNECTION;

	tval = get_value(pl, "bConnectionID");
	if (!tval)
		goto cleanup;
	desc->bConnectionID = tval->v.num & 0xff;

	tval = get_value(pl, "bConnectionType");
	if (!tval)
		goto cleanup;
	desc->bConnectionType = tval->v.num & 0xff;

	tval = get_value(pl, "bAssocConnection");
	if (!tval)
		goto cleanup;
	desc->bAssocConnection = tval->v.num & 0xff;

	/*iConnection skip*/
	tval = get_value(pl, "dTraceFormat");
	if (!tval)
		goto cleanup;
	desc->dTraceFormat = tval->v.num & 0xffffffff;

	tval = get_value(pl, "dStreamID");
	if (!tval)
		goto cleanup;
	desc->dStreamID = tval->v.num & 0xffffffff;

	/*iConnection*/
	tval = get_value(pl, "iConnection");
	if (tval)
		desc->sConnection = strdup(tval->v.str);

	add_iconn_desc(descs, desc);
	return;

cleanup:
	free(desc);
}

static void make_attr_desc(struct dvc_descriptors *descs,
			   struct pair_list *pl)
{
	struct pair *tval;
	size_t total_size, i;
	struct dvc_attr_desc *desc;

	desc = malloc(sizeof(*desc));
	if (!desc)
		return;

	memset(desc, 0, sizeof(*desc));

	desc->bDescriptorType = USB_DT_CS_INTERFACE;
	desc->bDescriptorSubtype = DC_DEBUG_ATTRIBUTES;

	tval = get_value(pl, "bcdDC");
	if (!tval)
		goto cleanup;
	desc->bcdDC = tval->v.num & 0xffff;

	tval = get_value(pl, "bTSorDIC");
	if (tval)
		desc->bTSorDIC = tval->v.num & 0xff;

	tval = get_value(pl, "bmSupportedEvents");
	if (!tval)
		goto cleanup;
	desc->bmSupportedEvents = tval->v.num & 0xff;

	tval = get_value(pl, "bmControl");
	if (tval && tval->v.num_array->len) {
		desc->bControlSize = tval->v.num_array->len;
		desc->bmControl = calloc(desc->bControlSize,
					 sizeof(*desc->bmControl));
		if (!desc->bmControl)
			goto cleanup;
		for (i = 0; i < tval->v.num_array->len; i++)
			desc->bmControl[i] = tval->v.num_array->elems[i] & 0xff;
	}

	tval = get_value(pl, "qBaseAddress");
	if (tval) {
		desc->bAuxDataSize = 8;
		desc->qBaseAddress = tval->v.num;
	}
	/*__le64 hGlobalID[2];	can be skipped	*/
	tval = get_value(pl, "hGlobalID");
	if (tval && tval->v.num_array->len) {
		desc->bAuxDataSize = 24;
		desc->hGlobalID[0] = tval->v.num_array->elems[0];
		if (tval->v.num_array->len > 1)
			desc->hGlobalID[1] = tval->v.num_array->elems[1];
	}

	tval = get_value(pl, "vendorData");
	if (tval && tval->v.num_array->len) {
		desc->wVendorDataSize = tval->v.num_array->len;
		desc->vendorData = calloc(desc->wVendorDataSize,
					  sizeof(*desc->vendorData));
		if (!desc->vendorData)
			goto cleanup_bmc;

		for (i = 0; i < desc->wVendorDataSize; i++)
			desc->vendorData[i] = tval->v.num_array->elems[i]
					& 0xff;
	}

	total_size = (10 + desc->bControlSize +
		      1 + desc->bAuxDataSize +
		      2 + desc->wVendorDataSize);
	desc->bLength = total_size;

	add_attr_desc(descs, desc);
	return;

cleanup_bmc:
	free(desc->bmControl);
cleanup:
	free(desc);
}

static void make_unit_desc(struct dvc_descriptors *descs,
			   struct pair_list *pl)
{
	struct pair *tval;
	size_t total_size, i;
	struct dvc_unit_desc *desc;

	desc = malloc(sizeof(*desc));
	if (!desc)
		return;

	memset(desc, 0, sizeof(*desc));

	desc->bDescriptorType = USB_DT_CS_INTERFACE;
	desc->bDescriptorSubtype = DC_DEBUG_UNIT;

	tval = get_value(pl, "bcdDC");
	if (!tval)
		goto cleanup;
	desc->bcdDC = tval->v.num & 0xffff;

	tval = get_value(pl, "bUnitID");
	if (!tval)
		goto cleanup;
	desc->bUnitID = tval->v.num & 0xff;

	tval = get_value(pl, "bDebugUnitType");
	if (!tval)
		goto cleanup;
	desc->bDebugUnitType = tval->v.num & 0xff;

	tval = get_value(pl, "bDebugSubUnitType");
	if (!tval)
		goto cleanup;
	desc->bDebugSubUnitType = tval->v.num & 0xff;

	tval = get_value(pl, "bAliasUnitID");
	if (!tval)
		goto cleanup;
	desc->bAliasUnitID = tval->v.num & 0xff;

	tval = get_value(pl, "wSourceID");
	if (tval && tval->v.num_array->len) {
		desc->bNrInPins = tval->v.num_array->len;
		desc->wSourceID = calloc(desc->bNrInPins,
					 sizeof(*desc->wSourceID));
		if (!desc->wSourceID)
			goto cleanup;
		for (i = 0; i < desc->bNrInPins; i++)
			desc->wSourceID[i] = tval->v.num_array->elems[i]
					& 0xffff;
	}

	tval = get_value(pl, "dTraceFormat");
	if (tval && tval->v.num_array->len) {
		desc->bNrOutPins = tval->v.num_array->len;
		desc->dTraceFormat = calloc(desc->bNrOutPins,
					    sizeof(*desc->dTraceFormat));
		if (!desc->dTraceFormat)
			goto cleanup_sid;

		for (i = 0; i < desc->bNrOutPins; i++)
			desc->dTraceFormat[i] = tval->v.num_array->elems[i]
					& 0xffffffff;
	}

	tval = get_value(pl, "dStreamID");
	if (tval && tval->v.num_array->len) {
		desc->dStreamID = calloc(desc->bNrOutPins,
					 sizeof(*desc->dStreamID));
		if (!desc->dStreamID)
			goto cleanup_tfmt;

		for (i = 0; i < desc->bNrOutPins; i++)
			desc->dStreamID[i] = tval->v.num_array->elems[i]
					& 0xffffffff;
	}

	tval = get_value(pl, "bmControl");
	if (tval && tval->v.num_array->len) {
		desc->bControlSize = tval->v.num_array->len;
		desc->bmControl = calloc(desc->bControlSize,
					 sizeof(*desc->bmControl));
		if (!desc->bmControl)
			goto cleanup_stid;

		for (i = 0; i < desc->bControlSize; i++)
			desc->bmControl[i] = tval->v.num_array->elems[i] & 0xff;
	}

	tval = get_value(pl, "qBaseAddress");
	if (tval) {
		desc->bAuxDataSize = 8;
		desc->qBaseAddress = tval->v.num;
	}
	tval = get_value(pl, "hGUID");
	if (tval && tval->v.num_array->len) {
		desc->bAuxDataSize = 24;
		desc->hGUID[0] = tval->v.num_array->elems[0];
		if (tval->v.num_array->len > 1)
			desc->hGUID[1] = tval->v.num_array->elems[1];
	}

	tval = get_value(pl, "bStandardsID");
	if (tval) {
		desc->wStandardsDataSize = 1;
		desc->bStandardsID = tval->v.num;
	}
	tval = get_value(pl, "standardsData");
	if (tval && tval->v.num_array->len) {
		desc->wStandardsDataSize = 1 + tval->v.num_array->len;
		desc->standardsData = calloc(tval->v.num_array->len,
					     sizeof(*desc->standardsData));
		if (!desc->standardsData)
			goto cleanup_bm;

		for (i = 0; i < tval->v.num_array->len; i++)
			desc->standardsData[i] = tval->v.num_array->elems[i]
							& 0xff;
	}

	tval = get_value(pl, "vendorData");
	if (tval && tval->v.num_array->len) {
		desc->wVendorDataSize = tval->v.num_array->len;
		desc->vendorData = calloc(desc->wVendorDataSize,
					  sizeof(*desc->vendorData));
		if (!desc->vendorData)
			goto cleanup_std;

		for (i = 0; i < desc->wVendorDataSize; i++)
			desc->vendorData[i] = tval->v.num_array->elems[i]
					& 0xff;
	}

	/*iDebugUnitType*/
	tval = get_value(pl, "iDebugUnitType");
	if (tval)
		desc->sDebugUnitType = strdup(tval->v.str);

	total_size = (9 +
		      1 + (desc->bNrInPins * 2) +
		      1 + (desc->bNrOutPins * 4 * 2) +
		      1 + desc->bControlSize +
		      1 + desc->bAuxDataSize +
		      2 + desc->wStandardsDataSize +
		      2 + desc->wVendorDataSize +
		      1);

	desc->bLength = total_size;
	add_unit_desc(descs, desc);
	return;

cleanup_std:
	free(desc->standardsData);
cleanup_bm:
	free(desc->bmControl);
cleanup_stid:
	free(desc->dStreamID);
cleanup_tfmt:
	free(desc->dTraceFormat);
cleanup_sid:
	free(desc->wSourceID);
cleanup:
	free(desc);
}

static void free_list(struct pair_list *pl)
{
	struct pair *current, *prev;

	for (prev = NULL, current = pl->first; current;
	     prev = current, current = current->next) {
		if (prev)
			free(prev);
		free(current->name);
		if (current->type == P_STRING)
			free(current->v.str);
		if (current->type == P_NUM_ARRAY) {
			free(current->v.num_array->elems);
			free(current->v.num_array);
		}
	}
	free(prev);
	pl->first = NULL;
	pl->last = NULL;
}

void flush_obj(struct dvc_descriptors *descs, int t, struct pair_list *pl)
{
	switch (t) {
	case ATTR_DESC:
		make_attr_desc(descs, pl);
		break;
	case UNIT_DESC:
		make_unit_desc(descs, pl);
		break;
	case IN_DESC:
		make_iconn_desc(descs, pl);
		break;
	case OUT_DESC:
		make_oconn_desc(descs, pl);
		break;
	}
	free_list(pl);
	free(pl);
}


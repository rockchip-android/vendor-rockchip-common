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

#include "incl/dvcdesc.h"
#include <stdio.h>
#include <inttypes.h>
#include <linux/usb/ch9.h>
#include "dvcdesc_internal.h"
#include <string.h>
#include <endian.h>

#define VALID_DESC_OFFSET(d, o) \
	(o > 0 && (o < (d->inputs_count + d->outputs_count + \
			d->units_count + 1)))

#define OFFSET_IS_UNIT(d, o) \
	(o > 0 && (o < (d->units_count + 1)))

#define OFFSET_IS_INPUT(d, o) \
	(o > d->units_count && (o < (d->inputs_count + d->units_count + 1)))

#define UNIT_OFFSET(d, o)	(o - 1)
#define INPUT_OFFSET(d, o)	(o - 1 - d->units_count)
#define OUTPUT_OFFSET(d, o)	(o - 1 - d->units_count - d->inputs_count)
#define UNIT_TO_OFFSET(d, o)	(o + 1)
#define INPUT_TO_OFFSET(d, o)	(o + 1 + d->units_count)
#define OUTPUT_TO_OFFSET(d, o)	(o + 1 + d->units_count + d->inputs_count)

extern int parse_string(const char *str, struct dvc_descriptors *descs);

static inline uint16_t get_le16(uint8_t *ptr)
{
	return le16toh(*(uint16_t *)ptr);
}

static inline uint32_t get_le32(uint8_t *ptr)
{
	return le32toh(*(uint32_t *)ptr);
}

static inline uint64_t get_le64(uint8_t *ptr)
{
	return le64toh(*(uint64_t *)ptr);
}

static inline void set_le16(uint8_t *ptr, uint16_t val)
{
	*(uint16_t *)ptr = htole16(val);
}

static inline void set_le32(uint8_t *ptr, uint32_t val)
{
	*(uint32_t *)ptr = htole32(val);
}

static inline void set_le64(uint8_t *ptr, uint64_t val)
{
	*(uint64_t *)ptr = htole64(val);
}

static inline size_t get_le16_array(uint16_t *dst, uint8_t *src, size_t count)
{
	size_t i;
	uint16_t *p_ptr = (uint16_t *)src;

	for (i = 0; i < count; i++)
		dst[i] = le16toh(p_ptr[i]);

	return i;
}

static inline size_t set_le16_array(uint8_t *dst, uint16_t *src, size_t count)
{
	size_t i;
	uint16_t *p_ptr = (uint16_t *)dst;

	for (i = 0; i < count; i++)
		p_ptr[i] = htole16(src[i]);

	return i;
}

static int hex_to_array(const char *in, uint8_t *out, size_t out_len)
{
	size_t len = 0;
	size_t pos = 0, in_len = strlen(in);
	__u8 tmp;
	int c_len, ret;

	ret = sscanf(in + pos, "%hhx%n", &tmp, &c_len);
	while (ret > 0 && c_len > 0 &&
	       len < out_len && c_len < (in_len - pos)) {
		out[len] = tmp;
		len++;
		pos += c_len;
		ret = sscanf(in + pos, "%hhx%n", &tmp, &c_len);
	}
	if (len >= out_len)
		return DVC_ERR_NOMEM;

	return len;
}

static int array_to_hex(uint8_t *in, size_t in_len, char *out, size_t max_len)
{
	size_t len = 0, i;

	for (i = 0; i < in_len; i++)
		len += snprintf(out + len, max_len - len, "%.2X ", in[i]);

	return len;
}

static int check_header(uint8_t *buffer, size_t len)
{
	if (buffer[0] > len)
		return DVC_ERR_INVALID;

	if (buffer[1] != USB_DT_CS_INTERFACE)
		return DVC_ERR_INVALID;

	return DVC_ERR_OK;
}

/*Attribute handlers*/
static int dvc_attr_unpack(struct dvc_attr_desc *attr, uint8_t *buffer,
			   size_t len)
{
	uint8_t bmcs, ads;
	uint16_t vds;
	int ret;
	size_t idx = 0;

	memset(attr, 0, sizeof(*attr));
	bmcs = buffer[9];
	ads = buffer[10 + bmcs];
	vds = get_le16(&buffer[11 + bmcs + ads]);

	ret = check_header(buffer, len);

	if (ret || buffer[2] != DC_DEBUG_ATTRIBUTES)
		return DVC_ERR_INVALID;

	attr->bLength = buffer[idx];
	idx++;

	attr->bDescriptorType = USB_DT_CS_INTERFACE;
	idx++;

	attr->bDescriptorType = DC_DEBUG_ATTRIBUTES;
	idx++;

	attr->bcdDC = get_le16(&buffer[idx]);
	idx += 2;

	attr->wTotalLength = get_le16(&buffer[idx]);
	idx += 2;

	attr->bTSorDIC = buffer[idx];
	idx += 1;

	attr->bmSupportedEvents = buffer[idx];
	idx++;

	attr->bControlSize = bmcs;
	idx++;

	if (bmcs) {
		attr->bmControl = malloc(bmcs);
		if (!attr->bmControl)
			goto err;
		memcpy(attr->bmControl, &buffer[idx], bmcs);
		idx += bmcs;
	}

	attr->bAuxDataSize = ads;
	idx += 1;

	if (ads >= 8)
		attr->qBaseAddress = get_le64(&buffer[idx]);
	idx += 8;

	if (ads >= 24) {
		attr->hGlobalID[0] = get_le64(&buffer[idx]);
		idx += 8;
		attr->hGlobalID[1] = get_le64(&buffer[idx]);
		idx += 8;
	}

	attr->wVendorDataSize = vds;
	idx += 2;

	if (vds) {
		attr->vendorData = malloc(vds);
		if (!attr->vendorData)
			goto err_bmControl;
		memcpy(attr->vendorData, &buffer[idx], vds);
		idx += vds;
	}

	return DVC_ERR_OK;

err_bmControl:
	free(attr->bmControl);
err:
	return DVC_ERR_NOMEM;
}

static void dvc_attr_free(struct dvc_attr_desc *attr)
{
	if (!attr)
		return;
	if (attr->bControlSize && attr->bmControl) {
		free(attr->bmControl);
		attr->bmControl = NULL;
	}
	if (attr->wVendorDataSize && attr->vendorData) {
		free(attr->vendorData);
		attr->vendorData = NULL;
	}
}

static int dvc_attr_pack(struct dvc_attr_desc *attr, uint8_t *buffer,
			 size_t len)
{
	size_t idx = 0;

	if (!attr->bLength || attr->bLength > len)
		return DVC_ERR_INVALID;

	memset(buffer, 0, attr->bLength);
	buffer[idx] = attr->bLength;
	idx++;

	buffer[idx] = USB_DT_CS_INTERFACE;
	idx++;

	buffer[idx] = DC_DEBUG_ATTRIBUTES;
	idx++;

	set_le16(&buffer[idx], attr->bcdDC);
	idx += 2;

	set_le16(&buffer[idx], attr->wTotalLength);
	idx += 2;

	buffer[idx] = attr->bTSorDIC;
	idx++;

	buffer[idx] = attr->bmSupportedEvents;
	idx++;

	buffer[idx] = attr->bControlSize;
	idx++;

	if (attr->bControlSize) {
		memcpy(&buffer[idx], attr->bmControl, attr->bControlSize);
		idx += attr->bControlSize;
	}

	buffer[idx] = attr->bAuxDataSize;
	idx++;

	if (attr->bAuxDataSize >= 8) {
		set_le64(&buffer[idx], attr->qBaseAddress);
		idx += 8;
	}

	if (attr->bAuxDataSize >= 24) {
		set_le64(&buffer[idx], attr->hGlobalID[0]);
		idx += 8;
		set_le64(&buffer[idx], attr->hGlobalID[1]);
		idx += 8;
	}

	set_le16(&buffer[idx], attr->wVendorDataSize);
	idx += 2;

	if (attr->wVendorDataSize) {
		memcpy(&buffer[idx], attr->vendorData, attr->wVendorDataSize);
		idx += attr->wVendorDataSize;
	}

	return attr->bLength;
}

static int dvc_attr_to_conf(struct dvc_attr_desc *attr, char *buffer,
			    size_t max_len)
{
	size_t len = 0;
	int i;

	if (!attr->bLength)
		return DVC_ERR_OK;

	len += snprintf(buffer + len, max_len - len, "\"attr\":{\n");
	len += snprintf(buffer + len, max_len - len, "\"bcdDC\": 0x%x,\n",
			attr->bcdDC);

	len += snprintf(buffer + len, max_len - len, "\"bTSorDIC\": 0x%x,\n",
			attr->bTSorDIC);
	len += snprintf(buffer + len, max_len - len,
			"\"bmSupportedEvents\": 0x%x,\n",
			attr->bmSupportedEvents);
	len += snprintf(buffer + len, max_len - len, "\"bmControl\":[");
	for (i = 0; i < attr->bControlSize; i++) {
		len += snprintf(buffer + len, max_len - len, "0x%x, ",
				attr->bmControl[i]);
	}
	len += snprintf(buffer + len, max_len - len, "],\n");

	if (attr->bAuxDataSize >= 8)
		len += snprintf(buffer + len, max_len - len,
				"\"qBaseAddress\":  0x%" PRIx64 ",\n",
				attr->qBaseAddress);

	if (attr->bAuxDataSize >= 16)
		len += snprintf(buffer + len, max_len - len,
				"\"hGlobalID\": [ 0x%" PRIx64 ", 0x%" PRIx64 " ],\n",
				attr->hGlobalID[0], attr->hGlobalID[1]);

	if (attr->wVendorDataSize) {
		len += snprintf(buffer + len, max_len - len,
				"\"vendorData\":[");
		for (i = 0; i < attr->wVendorDataSize; i++) {
			len += snprintf(buffer + len, max_len - len, "0x%x, ",
					attr->vendorData[i]);
		}
		len += snprintf(buffer + len, max_len - len, "],\n");
	}

	len += snprintf(buffer + len, max_len - len, "},\n");

	return len;
}

static int dvc_iconn_unpack(struct dvc_iconn_desc *iconn, uint8_t *buffer,
			    size_t len)
{
	int ret = check_header(buffer, len);

	memset(iconn, 0, sizeof(*iconn));

	if (ret || buffer[2] != DC_INPUT_CONNECTION)
		return DVC_ERR_INVALID;

	iconn->bLength = DC_INPUT_CONNECTION_SIZE;
	iconn->bDescriptorType = USB_DT_CS_INTERFACE;
	iconn->bDescriptorSubtype = DC_INPUT_CONNECTION;
	iconn->bConnectionID = buffer[3];
	iconn->bConnectionType = buffer[4];
	iconn->bAssocConnection = buffer[5];
	iconn->dTraceFormat = get_le32(&buffer[7]);
	iconn->dStreamID = get_le32(&buffer[11]);

	return DVC_ERR_OK;
}

static int dvc_iconn_pack(struct dvc_iconn_desc *iconn, uint8_t *buffer,
			  size_t len)
{
	if (len < DC_INPUT_CONNECTION_SIZE)
		return DVC_ERR_NOMEM;

	memset(buffer, 0, DC_INPUT_CONNECTION_SIZE);
	buffer[0] = DC_INPUT_CONNECTION_SIZE;
	buffer[1] = USB_DT_CS_INTERFACE;
	buffer[2] = DC_INPUT_CONNECTION;

	buffer[3] = iconn->bConnectionID;
	buffer[4] = iconn->bConnectionType;
	buffer[5] = iconn->bAssocConnection;
	set_le32(&buffer[7], iconn->dTraceFormat);
	set_le32(&buffer[11], iconn->dStreamID);

	return DC_INPUT_CONNECTION_SIZE;
}

static int dvc_iconn_to_conf(struct dvc_iconn_desc *iconn, char *buffer,
			     size_t max_len)
{
	size_t len = 0;

	if (!iconn->bLength)
		return DVC_ERR_INVALID;

	len += snprintf(buffer + len, max_len - len, "\"in\":{\n");
	len += snprintf(buffer + len, max_len - len,
			"\"bConnectionID\": 0x%x,\n", iconn->bConnectionID);
	len += snprintf(buffer + len, max_len - len,
			"\"bConnectionType\": 0x%x,\n", iconn->bConnectionType);
	len += snprintf(buffer + len, max_len - len,
			"\"bAssocConnection\": 0x%x,\n",
			iconn->bAssocConnection);
	if (iconn->sConnection)
		len += snprintf(buffer + len, max_len - len,
				"\"iConnection\": \"%s\",\n",
				iconn->sConnection);
	len += snprintf(buffer + len, max_len - len,
			"\"dTraceFormat\": 0x%x,\n", iconn->dTraceFormat);
	len += snprintf(buffer + len, max_len - len, "\"dStreamID\": 0x%x,\n",
			iconn->dStreamID);
	len += snprintf(buffer + len, max_len - len, "},\n");

	return len;
}

static void dvc_iconn_free(struct dvc_iconn_desc *iconn)
{
	if (!iconn)
		return;
	if (iconn->sConnection) {
		free(iconn->sConnection);
		iconn->sConnection = NULL;
	}
}

static int dvc_oconn_unpack(struct dvc_oconn_desc *oconn, uint8_t *buffer,
			    size_t len)
{
	int ret;

	ret = check_header(buffer, len);
	memset(oconn, 0, sizeof(*oconn));

	if (ret || buffer[2] != DC_OUTPUT_CONNECTION)
		return DVC_ERR_INVALID;

	oconn->bLength = DC_OUTPUT_CONNECTION_SIZE;
	oconn->bDescriptorType = USB_DT_CS_INTERFACE;
	oconn->bDescriptorSubtype = DC_OUTPUT_CONNECTION;

	oconn->bConnectionID = buffer[3];
	oconn->bConnectionType = buffer[4];
	oconn->bAssocConnection = buffer[5];
	oconn->wSourceID = get_le16(&buffer[6]);

	return DVC_ERR_OK;
}

static int dvc_oconn_pack(struct dvc_oconn_desc *oconn, uint8_t *buffer,
			  size_t len)
{
	if (len < DC_OUTPUT_CONNECTION_SIZE)
		return DVC_ERR_NOMEM;

	memset(buffer, 0, DC_OUTPUT_CONNECTION_SIZE);
	buffer[0] = DC_OUTPUT_CONNECTION_SIZE;
	buffer[1] = USB_DT_CS_INTERFACE;
	buffer[2] = DC_OUTPUT_CONNECTION;

	buffer[3] = oconn->bConnectionID;
	buffer[4] = oconn->bConnectionType;
	buffer[5] = oconn->bAssocConnection;
	set_le16(&buffer[6], oconn->wSourceID);

	return DC_OUTPUT_CONNECTION_SIZE;
}

static int dvc_oconn_to_conf(struct dvc_oconn_desc *oconn, char *buffer,
			     size_t max_len)
{
	size_t len = 0;

	if (!oconn->bLength)
		return DVC_ERR_NOMEM;

	len += snprintf(buffer + len, max_len - len, "\"out\":{\n");
	len += snprintf(buffer + len, max_len - len,
			"\"bConnectionID\": 0x%x,\n", oconn->bConnectionID);
	len += snprintf(buffer + len, max_len - len,
			"\"bConnectionType\": 0x%x,\n", oconn->bConnectionType);
	len += snprintf(buffer + len, max_len - len,
			"\"bAssocConnection\": 0x%x,\n",
			oconn->bAssocConnection);
	len += snprintf(buffer + len, max_len - len, "\"wSourceID\": 0x%x,\n",
			oconn->wSourceID);
	if (oconn->sConnection)
		len += snprintf(buffer + len, max_len - len,
				"\"iConnection\": \"%s\",\n",
				oconn->sConnection);
	len += snprintf(buffer + len, max_len - len, "},\n");

	return len;
}

static void dvc_oconn_free(struct dvc_oconn_desc *oconn)
{
	if (!oconn)
		return;
	if (oconn->sConnection) {
		free(oconn->sConnection);
		oconn->sConnection = NULL;
	}
}

/*Unit handlers*/
static int dvc_unit_unpack(struct dvc_unit_desc *unit, uint8_t *buffer,
			   size_t len)
{
	uint8_t ipc, opc, bmcs, ads;
	uint16_t sds, vds;
	int ret;
	size_t idx = 0;

	memset(unit, 0, sizeof(*unit));

	ipc = buffer[9];
	opc = buffer[10 + (ipc * 2)];
	bmcs = buffer[11 + (ipc * 2) + (opc * 8)];
	ads = buffer[12 + (ipc * 2) + (opc * 8) + bmcs];
	sds = get_le16(&buffer[13 + (ipc * 2) + (opc * 8) + bmcs + ads]);
	vds = get_le16(&buffer[15 + (ipc * 2) + (opc * 8) + bmcs + ads + sds]);

	ret = check_header(buffer, len);

	if (ret || buffer[2] != DC_DEBUG_UNIT)
		return DVC_ERR_INVALID;

	unit->bLength = buffer[idx];
	idx++;
	unit->bDescriptorType = USB_DT_CS_INTERFACE;
	idx++;
	unit->bDescriptorSubtype = DC_DEBUG_UNIT;
	idx++;

	unit->bcdDC = get_le16(&buffer[idx]);
	idx += 2;

	unit->bUnitID = buffer[idx];
	idx++;

	unit->bDebugUnitType = buffer[idx];
	idx++;

	unit->bDebugSubUnitType = buffer[idx];
	idx++;

	unit->bAliasUnitID = buffer[idx];
	idx++;

	unit->bNrInPins = ipc;
	idx++;

	if (ipc) {
		unit->wSourceID = malloc(ipc * sizeof(*unit->wSourceID));
		if (!unit->wSourceID)
			goto err;
		get_le16_array(unit->wSourceID, &buffer[idx], ipc);
		idx += (ipc * 2);
	}

	unit->bNrOutPins = opc;
	idx++;

	if (opc) {
		int i;

		unit->dTraceFormat = malloc(opc * sizeof(*unit->dTraceFormat));
		if (!unit->dTraceFormat)
			goto err_wSourceID;

		unit->dStreamID = malloc(opc * sizeof(*unit->dStreamID));
		if (!unit->dStreamID)
			goto err_dTraceFormat;

		for (i = 0; i < opc; i++) {
			unit->dTraceFormat[i] = get_le32(&buffer[idx]);
			idx += 4;
			unit->dStreamID[i] = get_le32(&buffer[idx]);
			idx += 4;
		}
	}

	unit->bControlSize = bmcs;
	idx++;

	if (bmcs) {
		unit->bmControl = malloc(bmcs);
		if (!unit->bmControl)
			goto err_dStreamID;
		memcpy(unit->bmControl, &buffer[idx], bmcs);
		idx += bmcs;
	}

	unit->bAuxDataSize = ads;
	idx++;

	if (ads >= 8) {
		unit->qBaseAddress = get_le64(&buffer[idx]);
		idx += 8;
	}

	if (ads >= 24) {
		unit->hGUID[0] = get_le64(&buffer[idx]);
		idx += 8;
		unit->hGUID[1] = get_le64(&buffer[idx]);
		idx += 8;
	}

	unit->wStandardsDataSize = sds;
	idx += 2;

	if (sds) {
		unit->bStandardsID = buffer[idx];
		idx++;
	}

	if (sds > 1) {
		unit->standardsData = malloc(sds - 1);
		if (!unit->standardsData)
			goto err_bmControl;
		memcpy(unit->standardsData, &buffer[idx], sds - 1);
		idx += (sds - 1);
	}

	unit->wVendorDataSize = vds;
	idx += 2;

	if (vds) {
		unit->vendorData = malloc(vds);
		if (!unit->vendorData)
			goto err_standardsData;
		memcpy(unit->vendorData, &buffer[idx], vds);
		idx += vds;
	}

	return DVC_ERR_OK;

err_standardsData:
	free(unit->standardsData);
err_bmControl:
	free(unit->bmControl);
err_dStreamID:
	free(unit->dStreamID);
err_dTraceFormat:
	free(unit->dTraceFormat);
err_wSourceID:
	free(unit->wSourceID);
err:
	return DVC_ERR_NOMEM;
}

static int dvc_unit_pack(struct dvc_unit_desc *unit, uint8_t *buffer,
			 size_t len)
{
	size_t idx = 0;

	if (!unit->bLength || unit->bLength > len)
		return DVC_ERR_INVALID;

	memset(buffer, 0, unit->bLength);
	buffer[idx] = unit->bLength;
	idx++;
	buffer[idx] = USB_DT_CS_INTERFACE;
	idx++;
	buffer[idx] = DC_DEBUG_UNIT;
	idx++;

	set_le16(&buffer[idx], unit->bcdDC);
	idx += 2;

	buffer[idx] = unit->bUnitID;
	idx++;
	buffer[idx] = unit->bDebugUnitType;
	idx++;
	buffer[idx] = unit->bDebugSubUnitType;
	idx++;
	buffer[idx] = unit->bAliasUnitID;
	idx++;
	buffer[idx] = unit->bNrInPins;
	idx++;

	if (unit->bNrInPins) {
		set_le16_array(&buffer[idx], unit->wSourceID, unit->bNrInPins);
		idx += (2 * unit->bNrInPins);
	}
	buffer[idx] = unit->bNrOutPins;
	idx++;

	if (unit->bNrOutPins) {
		int i;

		for (i = 0; i < unit->bNrOutPins; i++) {
			set_le32(&buffer[idx], unit->dTraceFormat[i]);
			idx += 4;
			set_le32(&buffer[idx], unit->dStreamID[i]);
			idx += 4;
		}
	}

	buffer[idx] = unit->bControlSize;
	idx++;

	if (unit->bControlSize) {
		memcpy(&buffer[idx], unit->bmControl, unit->bControlSize);
		idx += unit->bControlSize;
	}

	buffer[idx] = unit->bAuxDataSize;
	idx++;

	if (unit->bAuxDataSize >= 8)
		set_le64(&buffer[idx], unit->qBaseAddress);
	idx += 8;
	if (unit->bAuxDataSize >= 24) {
		set_le64(&buffer[idx], unit->hGUID[0]);
		idx += 8;
		set_le64(&buffer[idx], unit->hGUID[1]);
		idx += 8;
	}

	set_le16(&buffer[idx], unit->wStandardsDataSize);
	idx += 2;

	if (unit->wStandardsDataSize) {
		buffer[idx] = unit->bStandardsID;
		idx++;

		memcpy(&buffer[idx], unit->standardsData,
		       unit->wStandardsDataSize - 1);
		idx += (unit->wStandardsDataSize - 1);
	}

	set_le16(&buffer[idx], unit->wVendorDataSize);
	idx += 2;

	if (unit->wVendorDataSize) {
		memcpy(&buffer[idx], unit->vendorData,
		       unit->wVendorDataSize);
		idx += unit->wVendorDataSize;
	}

	buffer[idx] = 0; /*unit->iDebugUnitType*/

	return unit->bLength;
}

static int dvc_unit_to_conf(struct dvc_unit_desc *unit, char *buffer,
			    size_t max_len)
{
	size_t len = 0;
	int i;

	if (!unit->bLength)
		return DVC_ERR_INVALID;

	len += snprintf(buffer + len, max_len - len, "\"unit\":{\n");

	len += snprintf(buffer + len, max_len - len, "\"bcdDC\": 0x%x,\n",
			unit->bcdDC);

	len += snprintf(buffer + len, max_len - len, "\"bUnitID\": 0x%x,\n",
			unit->bUnitID);

	len += snprintf(buffer + len, max_len - len,
			"\"bDebugUnitType\": 0x%x,\n", unit->bDebugUnitType);

	len += snprintf(buffer + len, max_len - len,
			"\"bDebugSubUnitType\": 0x%x,\n",
			unit->bDebugSubUnitType);

	len += snprintf(buffer + len, max_len - len,
			"\"bAliasUnitID\": 0x%x,\n", unit->bAliasUnitID);

	if (unit->bNrInPins) {
		len += snprintf(buffer + len, max_len - len, "\"wSourceID\":[");
		for (i = 0; i < unit->bNrInPins; i++) {
			len += snprintf(buffer + len, max_len - len, "0x%x, ",
					unit->wSourceID[i]);
		}
		len += snprintf(buffer + len, max_len - len, "],\n");
	}

	if (unit->bNrOutPins) {
		len += snprintf(buffer + len, max_len - len,
				"\"dTraceFormat\":[");
		for (i = 0; i < unit->bNrOutPins; i++) {
			len += snprintf(buffer + len, max_len - len, "0x%x, ",
					unit->dTraceFormat[i]);
		}
		len += snprintf(buffer + len, max_len - len, "],\n");

		len += snprintf(buffer + len, max_len - len, "\"dStreamID\":[");
		for (i = 0; i < unit->bNrOutPins; i++) {
			len += snprintf(buffer + len, max_len - len, "0x%x, ",
					unit->dStreamID[i]);
		}
		len += snprintf(buffer + len, max_len - len, "],\n");
	}

	len += snprintf(buffer + len, max_len - len, "\"bmControl\":[");
	for (i = 0; i < unit->bControlSize; i++) {
		len += snprintf(buffer + len, max_len - len, "0x%x, ",
				unit->bmControl[i]);
	}
	len += snprintf(buffer + len, max_len - len, "],\n");

	if (unit->bAuxDataSize >= 8)
		len += snprintf(buffer + len, max_len - len,
				"\"qBaseAddress\": 0x%" PRIx64 ",\n",
				unit->qBaseAddress);

	if (unit->bAuxDataSize >= 24)
		len += snprintf(buffer + len, max_len - len,
				"\"hGUID\": [0x%" PRIx64 ", 0x%" PRIx64 " ],\n",
				unit->hGUID[0], unit->hGUID[1]);

	if (unit->wStandardsDataSize)
		len += snprintf(buffer + len, max_len - len,
				"\"bStandardsID\": 0x%x,\n",
				unit->bStandardsID);

	if (unit->wStandardsDataSize > 1) {
		len += snprintf(buffer + len, max_len - len,
				"\"standardsData\":[");
		for (i = 0; i < (unit->wStandardsDataSize - 1); i++) {
			len += snprintf(buffer + len, max_len - len, "0x%x, ",
					unit->standardsData[i]);
		}
		len += snprintf(buffer + len, max_len - len, "],\n");
	}

	if (unit->wVendorDataSize) {
		len += snprintf(buffer + len, max_len - len,
				"\"vendorData\":[");
		for (i = 0; i < unit->wVendorDataSize; i++) {
			len += snprintf(buffer + len, max_len - len, "0x%x, ",
					unit->vendorData[i]);
		}
		len += snprintf(buffer + len, max_len - len, "],\n");
	}
	if (unit->sDebugUnitType)
		len += snprintf(buffer + len, max_len - len,
				"\"iDebugUnitType\": \"%s\",\n",
				unit->sDebugUnitType);

	len += snprintf(buffer + len, max_len - len, "},\n");

	return len;
}

static void dvc_unit_free(struct dvc_unit_desc *unit)
{
	if (!unit)
		return;
	if (unit->bNrInPins && unit->wSourceID) {
		free(unit->wSourceID);
		unit->wSourceID = NULL;
	}

	if (unit->bNrOutPins && unit->dTraceFormat) {
		free(unit->dTraceFormat);
		unit->dTraceFormat = NULL;
	}

	if (unit->bControlSize && unit->bmControl) {
		free(unit->bmControl);
		unit->bmControl = NULL;
	}

	if (unit->wStandardsDataSize && unit->standardsData) {
		free(unit->standardsData);
		unit->standardsData = NULL;
	}

	if (unit->wVendorDataSize > 24 && unit->vendorData) {
		free(unit->vendorData);
		unit->vendorData = NULL;
	}

	if (unit->sDebugUnitType) {
		free(unit->sDebugUnitType);
		unit->sDebugUnitType = NULL;
	}
}

static void init_dvc_descs(struct dvc_descriptors *ptr)
{
	memset(ptr, 0, sizeof(*ptr));
}

int add_attr_desc(struct dvc_descriptors *ptr, struct dvc_attr_desc *attr)
{
	if (ptr->attr)
		return DVC_ERR_INVALID;
	ptr->attr = attr;
	return DVC_ERR_OK;
}

int add_iconn_desc(struct dvc_descriptors *ptr, struct dvc_iconn_desc *iconn)
{
	struct dvc_iconn_desc **new;

	new = malloc((ptr->inputs_count + 1) * sizeof(*ptr->inputs));
	if (!new)
		return DVC_ERR_NOMEM;

	if (ptr->inputs) {
		memcpy(new, ptr->inputs,
		       ptr->inputs_count * sizeof(*ptr->inputs));
		free(ptr->inputs);
	}

	ptr->inputs = new;
	ptr->inputs[ptr->inputs_count] = iconn;
	ptr->inputs_count++;

	return DVC_ERR_OK;
}

int add_oconn_desc(struct dvc_descriptors *ptr, struct dvc_oconn_desc *oconn)
{
	struct dvc_oconn_desc **new;

	new = malloc((ptr->outputs_count + 1) * sizeof(*ptr->outputs));
	if (!new)
		return DVC_ERR_NOMEM;

	if (ptr->outputs) {
		memcpy(new, ptr->outputs,
		       ptr->outputs_count * sizeof(*ptr->outputs));
		free(ptr->outputs);
	}

	ptr->outputs = new;
	ptr->outputs[ptr->outputs_count] = oconn;
	ptr->outputs_count++;

	return DVC_ERR_OK;
}

int add_unit_desc(struct dvc_descriptors *ptr, struct dvc_unit_desc *unit)
{
	struct dvc_unit_desc **new;

	new = malloc((ptr->units_count + 1) * sizeof(*ptr->units));
	if (!new)
		return DVC_ERR_NOMEM;

	if (ptr->units) {
		memcpy(new, ptr->units,
		       ptr->units_count * sizeof(*ptr->units));
		free(ptr->units);
	}

	ptr->units = new;
	ptr->units[ptr->units_count] = unit;
	ptr->units_count++;

	return DVC_ERR_OK;
}

int dvc_set_bin(struct dvc_descriptors *descs, uint8_t *buffer, size_t len)
{
	uint8_t *ptr = buffer;
	struct dvc_desc_header *current;
	struct dvc_iconn_desc *tmp_iconn;
	struct dvc_oconn_desc *tmp_oconn;
	struct dvc_unit_desc *tmp_unit;
	struct dvc_attr_desc *tmp_attr;
	int ret;

	while (ptr < (buffer + len)) {
		current = (struct dvc_desc_header *)ptr;
		switch (current->bDescriptorSubtype) {
		case DC_INPUT_CONNECTION:
			tmp_iconn = malloc(sizeof(*tmp_iconn));
			if (!tmp_iconn)
				goto err_mem;
			ret = dvc_iconn_unpack(tmp_iconn, ptr,
					       current->bLength);
			if (ret) {
				free(tmp_iconn);
				goto err;
			}
			add_iconn_desc(descs, tmp_iconn);
			break;
		case DC_OUTPUT_CONNECTION:
			tmp_oconn = malloc(sizeof(*tmp_oconn));
			if (!tmp_oconn)
				goto err_mem;
			ret = dvc_oconn_unpack(tmp_oconn, ptr,
					       current->bLength);
			if (ret) {
				free(tmp_oconn);
				goto err;
			}
			add_oconn_desc(descs, tmp_oconn);
			break;
		case DC_DEBUG_UNIT:
			tmp_unit = malloc(sizeof(*tmp_unit));
			if (!tmp_unit)
				goto err_mem;
			ret = dvc_unit_unpack(tmp_unit, ptr, current->bLength);
			if (ret) {
				free(tmp_unit);
				goto err;
			}
			add_unit_desc(descs, tmp_unit);
			break;
		case DC_DEBUG_ATTRIBUTES:
			tmp_attr = malloc(sizeof(*tmp_attr));
			if (!tmp_attr)
				goto err_mem;
			ret = dvc_attr_unpack(tmp_attr, ptr, current->bLength);
			if (ret) {
				free(tmp_attr);
				goto err;
			}
			add_attr_desc(descs, tmp_attr);
			break;
		default:
			ret = DVC_ERR_INVALID;
			goto err;
		}
		ptr += current->bLength;
	}

	return DVC_ERR_OK;

err_mem:
	ret = DVC_ERR_NOMEM;
err:
	 dvc_free_content(descs);
	 return DVC_ERR_INVALID;
}

int dvc_get_bin(struct dvc_descriptors *descs, uint8_t *buffer, size_t len)
{
	size_t total_length = 0;
	size_t i;
	uint8_t *ptr = buffer;
	int ret;

	if (!descs->attr)
		return DVC_ERR_FAIL;

	total_length += descs->attr->bLength;
	for (i = 0; i < descs->units_count; i++)
		total_length += descs->units[i]->bLength;

	total_length += (descs->inputs_count * DC_INPUT_CONNECTION_SIZE);
	total_length += (descs->outputs_count * DC_OUTPUT_CONNECTION_SIZE);

	if (total_length > len)
		return DVC_ERR_NOMEM;

	/*update attr with the total length*/
	descs->attr->wTotalLength = total_length;

	ret = dvc_attr_pack(descs->attr, ptr, descs->attr->bLength);
	if (ret <= 0)
		return DVC_ERR_FAIL;
	ptr += ret;

	for (i = 0; i < descs->units_count; i++) {
		ret = dvc_unit_pack(descs->units[i], ptr,
				    descs->units[i]->bLength);
		if (ret <= 0)
			return DVC_ERR_FAIL;
		ptr += ret;
	}

	for (i = 0; i < descs->inputs_count; i++) {
		ret = dvc_iconn_pack(descs->inputs[i], ptr,
				     descs->inputs[i]->bLength);
		if (ret <= 0)
			return DVC_ERR_FAIL;
		ptr += ret;
	}

	for (i = 0; i < descs->outputs_count; i++) {
		ret = dvc_oconn_pack(descs->outputs[i], ptr,
				     descs->outputs[i]->bLength);
		if (ret <= 0)
			return DVC_ERR_FAIL;
		ptr += ret;
	}

	return ptr - buffer;
}

int dvc_set_config_string(struct dvc_descriptors *descs, const char *buffer)
{
	init_dvc_descs(descs);

	return parse_string(buffer, descs);
}

int dvc_get_config_string(struct dvc_descriptors *descs, char *buffer,
			  size_t max_len)
{
	size_t len = 0;
	size_t i;
	int ret;

	if (!descs->attr)
		return DVC_ERR_OK;

	/* Also skip on error for now */
	ret = dvc_attr_to_conf(descs->attr, buffer + len, max_len - len);
	if (ret > 0)
		len += ret;

	for (i = 0; i < descs->units_count; i++) {
		ret = dvc_unit_to_conf(descs->units[i], buffer + len,
				       max_len - len);
		if (ret > 0)
			len += ret;
	}

	for (i = 0; i < descs->inputs_count; i++) {
		ret = dvc_iconn_to_conf(descs->inputs[i], buffer + len,
					max_len - len);
		if (ret > 0)
			len += ret;
	}

	for (i = 0; i < descs->outputs_count; i++) {
		ret = dvc_oconn_to_conf(descs->outputs[i], buffer + len,
					max_len - len);
		if (ret > 0)
			len += ret;
	}

	return len;
}

void dvc_free_content(struct dvc_descriptors *descs)
{
	size_t i;

	if (!descs)
		return;

	if (descs->attr) {
		dvc_attr_free(descs->attr);
		free(descs->attr);
		descs->attr = NULL;
	}

	for (i = 0; i < descs->units_count && descs->units; i++) {
		dvc_unit_free(descs->units[i]);
		free(descs->units[i]);
		descs->units[i] = NULL;
	}

	if (descs->units) {
		free(descs->units);
		descs->units = NULL;
	}

	descs->units_count = 0;

	for (i = 0; i < descs->inputs_count && descs->inputs; i++) {
		dvc_iconn_free(descs->inputs[i]);
		free(descs->inputs[i]);
		descs->inputs[i] = NULL;
	}

	if (descs->inputs) {
		free(descs->inputs);
		descs->inputs = NULL;
	}

	descs->inputs_count = 0;

	for (i = 0; i < descs->outputs_count && descs->outputs; i++) {
		dvc_oconn_free(descs->outputs[i]);
		free(descs->outputs[i]);
		descs->outputs[i] = NULL;
	}

	if (descs->outputs) {
		free(descs->outputs);
		descs->outputs = NULL;
	}

	descs->outputs_count = 0;
}

void dvc_free(struct dvc_descriptors *descs)
{
	dvc_free_content(descs);
	free(descs);
}

struct dvc_descriptors *dvc_alloc()
{
	struct dvc_descriptors *ret;

	ret = malloc(sizeof(*ret));
	if (!ret)
		return NULL;

	init_dvc_descs(ret);

	return ret;
}

int dvc_set_ascii_hex(struct dvc_descriptors *descs, const char *buffer)
{
	uint8_t array[1024];
	int arr_len;

	arr_len = hex_to_array(buffer, array, 1024);
	if (arr_len <= 0)
		return DVC_ERR_FAIL;

	return dvc_set_bin(descs, array, arr_len);
}

int dvc_get_ascii_hex(struct dvc_descriptors *descs, char *buffer,
		      size_t max_len)
{
	__u8 array[1024];
	int arr_len;

	arr_len = dvc_get_bin(descs, array, 1024);
	if (arr_len <= 0)
		return DVC_ERR_FAIL;

	return array_to_hex(array, arr_len, buffer, max_len);
}

int dvc_set_string_table(struct dvc_descriptors *descs, const char *buffer)
{
	size_t size = strlen(buffer);
	int count = 0;
	size_t off = 0;
	unsigned desc_offset, offset;
	char *tmp;

	while (off < size) {
		size_t slen;
		int i, j;

		j = sscanf(buffer + off, "%u.%u: %n",
			   &desc_offset, &offset, &i);
		if (j < 2 || i <= 0 || i >= (size - off))
			break;
		off += i;
		slen = 0;
		while (off + slen < size) {
			if (buffer[off + slen] == ';' ||
			    buffer[off + slen] == '\n')
				break;
			slen++;
		}
		/*set it up*/
		/*skip if out of bounds*/
		if (slen && VALID_DESC_OFFSET(descs, desc_offset)) {
			tmp = malloc(slen + 1);
			if (!tmp) /* just give up*/
				break;
			memcpy(tmp, buffer + off, slen);
			tmp[slen] = 0;
			if (OFFSET_IS_UNIT(descs, desc_offset)) {
				int o = UNIT_OFFSET(descs, desc_offset);

				if (descs->units[o]->sDebugUnitType)
					free(descs->units[o]->sDebugUnitType);
				else
					descs->units[o]->bLength++;

				descs->units[o]->sDebugUnitType = tmp;
			} else if (OFFSET_IS_INPUT(descs, desc_offset)) {
				int o = INPUT_OFFSET(descs, desc_offset);

				if (descs->inputs[o]->sConnection)
					free(descs->inputs[o]->sConnection);
				descs->inputs[o]->sConnection = tmp;
			} else {
				int o = OUTPUT_OFFSET(descs, desc_offset);

				if (descs->outputs[o]->sConnection)
					free(descs->outputs[o]->sConnection);
				descs->outputs[o]->sConnection = tmp;
			}
		}
		off += slen;
		if (buffer[off] == ';' || buffer[off] == '\n')
			off++;
		count++;
	}
	return count;
}

int dvc_get_string_table(struct dvc_descriptors *descs, char *buffer,
			 size_t max_len)
{
	size_t clen = 0;
	unsigned i;

	for (i = 0; i < descs->units_count; i++) {
		if (!descs->units[i]->sDebugUnitType)
			continue;
		clen += snprintf(buffer + clen, max_len - clen, "%u.%u: %s\n",
				 (unsigned int)UNIT_TO_OFFSET(descs, i),
				 descs->units[i]->bLength - 1,
				 descs->units[i]->sDebugUnitType);
	}

	for (i = 0; i < descs->inputs_count; i++) {
		if (!descs->inputs[i]->sConnection)
			continue;
		clen += snprintf(buffer + clen, max_len - clen, "%u.6: %s\n",
				 (unsigned int)INPUT_TO_OFFSET(descs, i),
				 descs->inputs[i]->sConnection);
	}

	for (i = 0; i < descs->outputs_count; i++) {
		if (!descs->outputs[i]->sConnection)
			continue;
		clen += snprintf(buffer + clen, max_len - clen, "%u.8: %s\n",
				 (unsigned int)INPUT_TO_OFFSET(descs, i),
				 descs->outputs[i]->sConnection);
	}

	return clen;
}

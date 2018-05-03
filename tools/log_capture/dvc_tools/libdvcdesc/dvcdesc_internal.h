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

#ifndef DVCDESC_INTERNAL_H
#define DVCDESC_INTERNAL_H
#include <inttypes.h>
#include <stdlib.h>

/*
 * USB Debug Class Rev. 1.0
 * Appendix A: Debug-Device-Class Codes
 * Table 8-6: Debug Class-Specific Descriptor SubTypes
 */
#define DC_UNDEFINED			0x00
#define DC_INPUT_CONNECTION		0x01
#define DC_OUTPUT_CONNECTION		0x02
#define DC_DEBUG_UNIT			0x03
#define DC_DEBUG_ATTRIBUTES		0x04

/*
 * USB Debug Class Rev. 1.0
 * 4.4.3 Debug-Attributes Descriptor
 * Table 4-6: Debug Class Debug-Attributes Descriptor - bTSorDIC
 */

/* Friendly representation*/
struct dvc_attr_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdDC;
	uint16_t wTotalLength;
	uint8_t bTSorDIC;
	uint8_t bmSupportedEvents;
	uint8_t bControlSize; /*_bmcs*/
	uint8_t *bmControl;
	uint8_t bAuxDataSize;
	uint64_t qBaseAddress;
	uint64_t hGlobalID[2];
	uint16_t wVendorDataSize;
	uint8_t *vendorData;
};

/*
 * USB Debug Class Rev. 1.0
 * 4.4.4 Input-Connection Descriptor
 * Table 4-7: Input Connection Descriptor
 */

/* Friendly representation*/
struct dvc_iconn_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bConnectionID;
	uint8_t bConnectionType;
	uint8_t bAssocConnection;
	uint8_t iConnection;
	uint32_t dTraceFormat; /*optional*/
	uint32_t dStreamID; /*optional*/
	char *sConnection;
};

#define DC_INPUT_CONNECTION_SIZE	15 /* or 7 */

/*
 * USB Debug Class Rev. 1.0
 * 4.4.5 Output Connection Descriptor
 * Table 4-8: Output Connection Descriptor
 */

/* Friendly representation*/
struct dvc_oconn_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bConnectionID;
	uint8_t bConnectionType;
	uint8_t bAssocConnection;
	uint16_t wSourceID;
	uint8_t iConnection;
	char *sConnection;
};

#define DC_OUTPUT_CONNECTION_SIZE	9

/*
 * USB Debug Class Rev. 1.0
 * 4.4.6 Debug-Unit Descriptor
 */

/* Friendly representation*/
struct dvc_unit_desc {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdDC;
	uint8_t bUnitID;
	uint8_t bDebugUnitType;
	uint8_t bDebugSubUnitType;
	uint8_t bAliasUnitID;
	uint8_t bNrInPins; /*_ipc*/
	uint16_t *wSourceID;
	uint8_t bNrOutPins; /*_opc */
	uint32_t *dTraceFormat;
	uint32_t *dStreamID;
	uint8_t bControlSize; /*_bmcs*/
	uint8_t *bmControl;
	uint8_t bAuxDataSize;
	uint64_t qBaseAddress;
	uint64_t hGUID[2];
	uint16_t wStandardsDataSize;
	uint8_t bStandardsID;
	uint8_t *standardsData;
	uint16_t wVendorDataSize;
	uint8_t *vendorData;
	uint8_t iDebugUnitType;
	char *sDebugUnitType;
};

struct dvc_desc_header {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
};

struct dvc_descriptors {
	struct dvc_attr_desc *attr;
	size_t units_count, inputs_count, outputs_count;
	struct dvc_unit_desc **units;
	struct dvc_iconn_desc **inputs;
	struct dvc_oconn_desc **outputs;
};

/* Internal API*/
int add_attr_desc(struct dvc_descriptors *ptr, struct dvc_attr_desc *attr);
int add_unit_desc(struct dvc_descriptors *ptr, struct dvc_unit_desc *desc);
int add_iconn_desc(struct dvc_descriptors *ptr, struct dvc_iconn_desc *desc);
int add_oconn_desc(struct dvc_descriptors *ptr, struct dvc_oconn_desc *desc);
void dvc_free_content(struct dvc_descriptors *descs);
#endif

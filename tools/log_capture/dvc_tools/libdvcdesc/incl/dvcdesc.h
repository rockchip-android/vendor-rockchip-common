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

#ifndef DVCDESC_H
#define DVCDESC_H
#include <stdlib.h>
#include <inttypes.h>

/** API Errors*/
#define DVC_ERR_OK	0	/**< Success */
#define DVC_ERR_FAIL	(-1)	/**< Generic error */
#define DVC_ERR_INVALID	(-2)	/**< Invalid input */
#define DVC_ERR_NOMEM	(-3)	/**< Allocation failed */

/**
 * @brief Internal descriptors representation
 *
 * Opaque representation of a DvC-Trace topology
 * descriptors set.
 */
struct dvc_descriptors;

/**
 * @brief Set Binary
 *
 * Parse a binary representation of the descriptors and place the result
 * into given @descs
 *
 * @param descs Descriptor representation
 * @param buffer
 * @param len
 *
 * @return DVC_ERR_*
 */
int dvc_set_bin(struct dvc_descriptors *descs, uint8_t *buffer, size_t len);

/**
 * @brief Get Binary
 *
 * Get the binary representation of @descs
 *
 * @param descs Descriptor representation
 * @param buffer
 * @param len Maximum length
 *
 * @return The length of the output or DVC_ERR*
 */
int dvc_get_bin(struct dvc_descriptors *descs, uint8_t *buffer, size_t len);

/**
 * @brief Set Ascii Hex
 *
 * Same as @dvc_set_bin but the buffer is represented in ASCII hex
 * eg. 09 0a 0b ...
 *
 * @param descs
 * @param buffer
 *
 * @return DVC_ERR_*
 */
int dvc_set_ascii_hex(struct dvc_descriptors *descs, const char *buffer);

/**
 * @brief Get Ascii Hex
 *
 * Same as @dvc_get_bin but the buffer is represented in ASCII hex
 *
 * @param descs
 * @param buffer
 * @param max_len
 *
 * @return The length of the output or DVC_ERR*
 */
int dvc_get_ascii_hex(struct dvc_descriptors *descs, char *buffer,
		      size_t max_len);

/**
 * @brief Set Config String
 *
 * Set the content of a config string buffer
 *
 * @param descs
 * @param buffer
 *
 * @return DVC_ERR_*
 */
int dvc_set_config_string(struct dvc_descriptors *descs, const char *buffer);

/**
 * @brief Get config string
 *
 * Get descriptors in cfg (json like) representation
 *
 * @param descs
 * @param buffer
 * @param max_len
 *
 * @return Output length or DVC_ERR_*
 */
int dvc_get_config_string(struct dvc_descriptors *descs, char *buffer,
			  size_t max_len);

/**
 * @brief Allocate descriptors representation
 *
 * Allocate and initialize a new dvc_descriptors structure.
 * The user should free this using @dvc_free.
 *
 * @return
 */
struct dvc_descriptors *dvc_alloc();

/**
 * @brief Free a descriptors representation
 *
 * @param descs
 */
void dvc_free(struct dvc_descriptors *descs);

/**
 * @brief Set string table
 *
 * Please check <kernel>/Documentation/ABI/testing/sysfs-bus-dvctrace for
 * format details.
 *
 * @param descs
 * @param buffer
 *
 * @return
 */
int dvc_set_string_table(struct dvc_descriptors *descs, const char *buffer);

/**
 * @brief Get string table
 *
 * Please check <kernel>/Documentation/ABI/testing/sysfs-bus-dvctrace for
 * format details.
 *
 * @param descs
 * @param buffer
 * @param max_len
 *
 * @return Length or DVC_ERR*
 */
int dvc_get_string_table(struct dvc_descriptors *descs,
			 char *buffer, size_t max_len);

#endif /* DVCDESC_H */

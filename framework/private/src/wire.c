/**
 *Licensed to the Apache Software Foundation (ASF) under one
 *or more contributor license agreements.  See the NOTICE file
 *distributed with this work for additional information
 *regarding copyright ownership.  The ASF licenses this file
 *to you under the Apache License, Version 2.0 (the
 *"License"); you may not use this file except in compliance
 *with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing,
 *software distributed under the License is distributed on an
 *"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 *specific language governing permissions and limitations
 *under the License.
 */
/*
 * wire.c
 *
 *  Created on: Jul 19, 2010
 *      Author: alexanderb
 */
#include <stdlib.h>
#include <string.h>

#include "wire.h"

struct wire {
	MODULE importer;
	REQUIREMENT requirement;
	MODULE exporter;
	CAPABILITY capability;
};

apr_status_t wire_destroy(void *wireP);

celix_status_t wire_create(apr_pool_t *pool, MODULE importer, REQUIREMENT requirement,
		MODULE exporter, CAPABILITY capability, WIRE *wire) {
	celix_status_t status = CELIX_SUCCESS;

	(*wire) = (WIRE) apr_palloc(pool, sizeof(**wire));
	if (!*wire) {
		status = CELIX_ENOMEM;
	} else {
		apr_pool_pre_cleanup_register(pool, *wire, wire_destroy);

		(*wire)->importer = importer;
		(*wire)->requirement = requirement;
		(*wire)->exporter = exporter;
		(*wire)->capability = capability;
	}

	return status;
}

apr_status_t wire_destroy(void *wireP) {
	WIRE wire = wireP;
	wire->importer = NULL;
	wire->requirement = NULL;
	wire->exporter = NULL;
	wire->capability = NULL;
	return APR_SUCCESS;
}

celix_status_t wire_getCapability(WIRE wire, CAPABILITY *capability) {
	*capability = wire->capability;
	return CELIX_SUCCESS;
}

celix_status_t wire_getRequirement(WIRE wire, REQUIREMENT *requirement) {
	*requirement = wire->requirement;
	return CELIX_SUCCESS;
}

celix_status_t wire_getImporter(WIRE wire, MODULE *importer) {
	*importer = wire->importer;
	return CELIX_SUCCESS;
}

celix_status_t wire_getExporter(WIRE wire, MODULE *exporter) {
	*exporter = wire->exporter;
	return CELIX_SUCCESS;
}
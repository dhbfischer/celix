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
 * wire.h
 *
 *  Created on: Jul 12, 2010
 *      Author: alexanderb
 */

#ifndef WIRE_H_
#define WIRE_H_

#include "requirement.h"
#include "capability.h"
#include "module.h"
#include "linkedlist.h"
#include "headers.h"

/**
 * @defgroup Version Version
 * @ingroup framework
 * @{
 */

/**
 * Create a wire between two modules using a requirement and capability.
 *
 * @param pool The pool in which the wire is created.
 * @param importer The importer module of the wire.
 * @param requirement The requirement of the importer.
 * @param exporter The exporter module of the wire.
 * @param capability The capability of the wire.
 * @param wire The created wire.
 * @return Status code indication failure or success:
 * 		- CELIX_SUCCESS when no errors are encountered.
 * 		- CELIX_ENOMEM If allocating memory for <code>wire</code> failed.
 */
celix_status_t wire_create(apr_pool_t *pool, MODULE importer, REQUIREMENT requirement,
		MODULE exporter, CAPABILITY capability, WIRE *wire);

/**
 * Getter for the capability of the exporting module.
 *
 * @param wire The wire to get the capability from.
 * @param capability The capability
 * @return Status code indication failure or success:
 * 		- CELIX_SUCCESS when no errors are encountered.
 */
celix_status_t wire_getCapability(WIRE wire, CAPABILITY *capability);

/**
 * Getter for the requirement of the importing module.
 *
 * @param wire The wire to get the requirement from.
 * @param requirement The requirement
 * @return Status code indication failure or success:
 * 		- CELIX_SUCCESS when no errors are encountered.
 */
celix_status_t wire_getRequirement(WIRE wire, REQUIREMENT *requirement);

/**
 * Getter for the importer of the wire.
 *
 * @param wire The wire to get the importer from.
 * @param importer The importing module.
 * @return Status code indication failure or success:
 * 		- CELIX_SUCCESS when no errors are encountered.
 */
celix_status_t wire_getImporter(WIRE wire, MODULE *importer);

/**
 * Getter for the exporter of the wire.
 *
 * @param wire The wire to get the exporter from.
 * @param importer The exporting module.
 * @return Status code indication failure or success:
 * 		- CELIX_SUCCESS when no errors are encountered.
 */
celix_status_t wire_getExporter(WIRE wire, MODULE *exporter);

/**
 * @}
 */

#endif /* WIRE_H_ */
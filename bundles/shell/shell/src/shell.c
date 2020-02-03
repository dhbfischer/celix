/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


#include <stdlib.h>
#include <string.h>
#include <log_helper.h>
#include <celix_constants.h>

#include "celix_errno.h"

#include "shell_private.h"


#include "utils.h"


shell_t* shell_create(celix_bundle_context_t *ctx) {
    shell_t *shell = calloc(1, sizeof(*shell));

    shell->ctx = ctx;
    logHelper_create(ctx, &shell->logHelper);

    celix_thread_mutexattr_t attr;
    celixThreadMutexAttr_create(&attr);
    celixThreadMutexAttr_settype(&attr, CELIX_THREAD_MUTEX_RECURSIVE); //NOTE recursive, because command can also use the shell service
    celixThreadMutex_create(&shell->mutex, &attr);
    shell->commandServices = hashMap_create(utils_stringHash, NULL, utils_stringEquals, NULL);

    return shell;
}

void shell_destroy(shell_t *shell) {
    if (shell != NULL) {
        celixThreadMutex_destroy(&shell->mutex);
        hashMap_destroy(shell->commandServices, false, false);
        logHelper_destroy(&shell->logHelper);
        free(shell);
    }
}

celix_status_t shell_addCommand(shell_t *shell, celix_shell_command_t *svc, const celix_properties_t *props) {
    celix_status_t status = CELIX_SUCCESS;
    const char *name = celix_properties_get(props, "command.name", NULL);

    if (name == NULL) {
        logHelper_log(shell->logHelper, OSGI_LOGSERVICE_WARNING, "Command service must contain a 'command.name' property!");
        status = CELIX_BUNDLE_EXCEPTION;
    } else {
        long svcId = celix_properties_getAsLong(props, OSGI_FRAMEWORK_SERVICE_ID, -1L);
        celixThreadMutex_lock(&shell->mutex);
        if (hashMap_containsKey(shell->commandServices, name)) {
            logHelper_log(shell->logHelper, OSGI_LOGSERVICE_WARNING, "Command with name %s already registered!", name);
        } else {
            celix_shell_command_entry_t *entry = calloc(1, sizeof(*entry));
            entry->svcId = svcId;
            entry->svc = svc;
            entry->props = props;
            hashMap_put(shell->commandServices, (void*)name, entry);
        }
        celixThreadMutex_unlock(&shell->mutex);
    }

    return status;
}

celix_status_t shell_removeCommand(shell_t *shell, celix_shell_command_t *svc, const celix_properties_t *props) {
    celix_status_t status = CELIX_SUCCESS;
    const char *name = celix_properties_get(props, "command.name", NULL);

    if (name == NULL) {
        logHelper_log(shell->logHelper, OSGI_LOGSERVICE_WARNING, "Command service must contain a 'command.name' property!");
        status = CELIX_BUNDLE_EXCEPTION;
    } else {
        long svcId = celix_properties_getAsLong(props, OSGI_FRAMEWORK_SERVICE_ID, -1L);
        celixThreadMutex_lock(&shell->mutex);
        if (hashMap_containsKey(shell->commandServices, name)) {
            celix_shell_command_entry_t *entry = hashMap_get(shell->commandServices, name);
            if (entry->svcId == svcId) {
                hashMap_remove(shell->commandServices, name);
                free(entry);
            } else {
                logHelper_log(shell->logHelper, OSGI_LOGSERVICE_WARNING, "svc id for command with name %s does not match (%li == %li)!", name, svcId, entry->svcId);
            }
        } else {
            logHelper_log(shell->logHelper, OSGI_LOGSERVICE_WARNING, "Cannot find shell command with name %s!", name);
        }
        celixThreadMutex_unlock(&shell->mutex);
    }

    return status;
}

celix_status_t shell_getCommands(shell_t *shell, celix_array_list_t **outCommands) {
	celix_status_t status = CELIX_SUCCESS;
	celix_array_list_t *result = celix_arrayList_create();

    celixThreadMutex_lock(&shell->mutex);
    hash_map_iterator_t iter = hashMapIterator_construct(shell->commandServices);
    while (hashMapIterator_hasNext(&iter)) {
        const char *name = hashMapIterator_nextKey(&iter);
        celix_arrayList_add(result, strndup(name, 1024*1024*10));
    }
    celixThreadMutex_unlock(&shell->mutex);

    *outCommands = result;

    return status;
}


celix_status_t shell_getCommandUsage(shell_t *shell, const char *commandName, char **outUsage) {
    celix_status_t status = CELIX_SUCCESS;

    celixThreadMutex_lock(&shell->mutex);
    celix_shell_command_entry_t *entry = hashMap_get(shell->commandServices, commandName);
    if (entry != NULL) {
        const char *usage = celix_properties_get(entry->props, CELIX_SHELL_COMMAND_USAGE, "N/A");
        *outUsage = strndup(usage, 1024*1024*10);
    } else {
        *outUsage = NULL;
    }
    celixThreadMutex_unlock(&shell->mutex);


    return status;
}

celix_status_t shell_getCommandDescription(shell_t *shell, const char *commandName, char **outDescription) {
    celix_status_t status = CELIX_SUCCESS;

    celixThreadMutex_lock(&shell->mutex);
    celix_shell_command_entry_t *entry = hashMap_get(shell->commandServices, commandName);
    if (entry != NULL) {
        const char *desc = celix_properties_get(entry->props, CELIX_SHELL_COMMAND_DESCRIPTION, "N/A");
        *outDescription = strndup(desc, 1024*1024*10);
    } else {
        *outDescription = NULL;
    }
    celixThreadMutex_unlock(&shell->mutex);

    return status;
}

celix_status_t shell_executeCommand(shell_t *shell, const char *commandLine, FILE *out, FILE *err) {
	celix_status_t status = CELIX_SUCCESS;

	if (status == CELIX_SUCCESS) {
		size_t pos = strcspn(commandLine, " ");

		char *command_name_str = (pos != strlen(commandLine)) ? strndup(commandLine, pos) : strdup(commandLine);

        celixThreadMutex_lock(&shell->mutex);
        celix_shell_command_entry_t *entry = hashMap_get(shell->commandServices, command_name_str);
        if (entry == NULL) {
            fprintf(err, "No such command\n");
            status = CELIX_BUNDLE_EXCEPTION;
        } else {
            char * cl = (void*)commandLine; //NOTE needed because the shell command was written with char* instead of const char*
            entry->svc->executeCommand(entry->svc->handle, cl, out, err);
        }
        celixThreadMutex_unlock(&shell->mutex);
		free(command_name_str);

	}

	return status;
}

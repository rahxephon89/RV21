#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include "smedl_types.h"
#include "global_event_queue.h"
#include "sync_global_wrapper.h"
#include "CreateMCI_local_wrapper.h"
#include "CreateMC_local_wrapper.h"
#include "CreateMCI_mon.h"
#include "CreateMC_mon.h"

/* Global event queues - containing exported events */
static GlobalEventQueue intra_queue;
static GlobalEventQueue inter_queue;

/* Callback function pointers */
static SMEDLCallback cb_CreateMCI_violation;

/* Initialization interface - Initialize the global wrapper. Must be called once
 * before importing any events. Return nonzero on success, zero on failure. */
int init_sync_syncset() {
    /* Initialize all local wrappers */
    init_CreateMCI_local_wrapper();
    init_CreateMC_local_wrapper();

    return 1;

fail_init_CreateMC:
    free_CreateMCI_local_wrapper();
fail_init_CreateMCI:
    return 0;
}

/* Cleanup interface - Tear down and free the resources used by this global
 * wrapper and all the local wrappers and monitors it manages. */
void free_sync_syncset() {
    /* Free local wrappers */
    free_CreateMCI_local_wrapper();
    free_CreateMC_local_wrapper();

    /* Unset callbacks */
    cb_CreateMCI_violation = NULL;
}

/* Intra routing function - Called by import interface functions and intra queue
 * processing function to route events to the local wrappers.
 * Return nonzero on success, zero on failure. */
int route_sync_ch1(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'sync' routing for conn 'ch1'\n");
    #endif
    {
        SMEDLValue new_identities[2] = {
                params[0],
                params[1],
            };

        SMEDLValue *new_params = NULL;

        if (!process_CreateMC_new_mc(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_sync_ch2(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'sync' routing for conn 'ch2'\n");
    #endif
    {
        SMEDLValue new_identities[2] = {
                {SMEDL_NULL},
                params[0],
            };

        SMEDLValue new_params[1] = {
                params[1],
            };

        if (!process_CreateMC_new_ci(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_sync_ch4(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'sync' routing for conn 'ch4'\n");
    #endif
    {
        SMEDLValue new_identities[3] = {
                params[0],
                {SMEDL_NULL},
                {SMEDL_NULL},
            };

        SMEDLValue *new_params = NULL;

        if (!process_CreateMCI_traverse_m(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_sync_ch5(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'sync' routing for conn 'ch5'\n");
    #endif
    {
        SMEDLValue new_identities[3] = {
                {SMEDL_NULL},
                {SMEDL_NULL},
                params[0],
            };

        SMEDLValue *new_params = NULL;

        if (!process_CreateMCI_traverse_i(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_sync_ch3(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'sync' routing for conn 'ch3'\n");
    #endif
    {
        SMEDLValue new_identities[3] = {
                identities[0],
                identities[1],
                params[0],
            };

        SMEDLValue *new_params = NULL;

        if (!process_CreateMCI_new_mci(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}

/* Intra queue processing function - Route events to the local wrappers. Return
 * nonzero on success, zero on failure. */
static int handle_sync_intra() {
    int success = 1;
    int channel;
    SMEDLValue *identities, *params;
    void *aux;

    while (pop_global_event(&intra_queue, &channel, &identities, &params, &aux)) {
        switch (channel) {
            case CHANNEL_sync_ch3:
                success = success &&
                    route_sync_ch3(identities, params, aux);
                break;
        }

        /* Event params and identites were malloc'd. They are no longer needed.
         * (String and opaque data were already free'd in the switch.) */
        free(identities);
        free(params);
    }
    return success;
}

/* Inter queue processing function - Call the export callbacks. Return nonzero
 * on success, zero on failure. */
static int handle_sync_inter() {
    int success = 1;
    int channel;
    SMEDLValue *identities, *params;
    void *aux;

    while (pop_global_event(&inter_queue, &channel, &identities, &params, &aux)) {
        switch (channel) {
            case CHANNEL_sync_CreateMCI_violation:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'sync' exporting for conn 'CreateMCI_violation'\n");
#endif
                if (cb_CreateMCI_violation != NULL) {
                    success = success &&
                        cb_CreateMCI_violation(identities, params, aux);
                }
                break;
        }

        /* Event params and identites were malloc'd. They are no longer needed.
         * (String and opaque data were already free'd in the switch.) */
        free(identities);
        free(params);
    }
    return success;
}

/* Queue processing function - Handle the events in the intra queue, then the
 * inter queue. Return nonzero on success, zero on failure. */
static int handle_sync_queues() {
    int success = handle_sync_intra();
    return handle_sync_inter() && success;
}

/* Global wrapper export interfaces - Called by monitors to place exported
 * events into the appropriate export queues, where they will later be routed to
 * the proper destinations inside and outside the synchronous set.
 * Returns nonzero on success, zero on failure.
 *
 * Parameters:
 * identites - An array of SMEDLValue of the proper length for the exporting
 *   monitor
 * params - An array of SMEDLValue, one for each parameter of the exported event
 * aux - Extra data that was passed from the imported event that caused this
 *   exported event
 */
int raise_CreateMCI_violation(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 3);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 0);
    if (!push_global_event(&inter_queue, CHANNEL_sync_CreateMCI_violation, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 0);
        return 0;
    }
}
int raise_CreateMC_new_mci(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 2);
    /* Store on intra queue */
    SMEDLValue *params_intra = smedl_copy_array(params, 1);
    if (!push_global_event(&intra_queue, CHANNEL_sync_ch3, ids_copy, params_intra, aux)) {
        /* malloc fail */
        smedl_free_array(params_intra, 1);
        return 0;
    }
}

/* Global wrapper import interface - Called by the environment (other
 * synchronous sets, the target system) to import events into this global
 * wrapper. Each connection that this synchronous set receives has a separate
 * function.
 * Returns nonzero on success, zero on failure.
 *
 * Parameters:
 * identities - An array of the source monitor's identities. If the connection
 *   is from the target system, this parameter is ignored and can safely be set
 *   to NULL.
 * params - An array of the source event's parameters
 * aux - Extra data to be passed through unchanged */

int import_sync_ch1(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_sync_ch1(identities, params, aux);
    return handle_sync_queues() && success;
}

int import_sync_ch2(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_sync_ch2(identities, params, aux);
    return handle_sync_queues() && success;
}

int import_sync_ch4(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_sync_ch4(identities, params, aux);
    return handle_sync_queues() && success;
}

int import_sync_ch5(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_sync_ch5(identities, params, aux);
    return handle_sync_queues() && success;
}

/* Global wrapper callback interface - Used to register callback functions to be
 * called by this global wrapper when it has an event to export to the
 * environment (other synchronous sets, the target system).
 *
 * Parameters:
 * cb_func - A function pointer for the callback to register, or NULL to
 *   unregister a callback. Must accept three parameters: An array of SMEDLValue
 *   for the source monitor's identities (or NULL if the source monitor has
 *   none), another array of SMEDLValue for the source event's parameters, and
 *   a void * for passthrough data. */

void callback_sync_CreateMCI_violation(SMEDLCallback cb_func) {
    cb_CreateMCI_violation = cb_func;
}

#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include "smedl_types.h"
#include "global_event_queue.h"
#include "CandidateSelection_global_wrapper.h"
#include "CandidateSelection_local_wrapper.h"
#include "CandidateSelection_mon.h"

/* Global event queues - containing exported events */
static GlobalEventQueue intra_queue;
static GlobalEventQueue inter_queue;

/* Callback function pointers */
static SMEDLCallback cb_ch6;
static SMEDLCallback cb_ch8;
static SMEDLCallback cb_ch9;

/* Initialization interface - Initialize the global wrapper. Must be called once
 * before importing any events. Return nonzero on success, zero on failure. */
int init_CandidateSelection_syncset() {
    /* Initialize all local wrappers */
    init_CandidateSelection_local_wrapper();

    return 1;

fail_init_CandidateSelection:
    return 0;
}

/* Cleanup interface - Tear down and free the resources used by this global
 * wrapper and all the local wrappers and monitors it manages. */
void free_CandidateSelection_syncset() {
    /* Free local wrappers */
    free_CandidateSelection_local_wrapper();

    /* Unset callbacks */
    cb_ch6 = NULL;
    cb_ch8 = NULL;
    cb_ch9 = NULL;
}

/* Intra routing function - Called by import interface functions and intra queue
 * processing function to route events to the local wrappers.
 * Return nonzero on success, zero on failure. */
int route_CandidateSelection_ch1(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'CandidateSelection' routing for conn 'ch1'\n");
    #endif
    {
        SMEDLValue new_identities[2] = {
                params[1],
                params[0],
            };

        SMEDLValue new_params[2] = {
                params[0],
                params[1],
            };

        if (!process_CandidateSelection_member(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_CandidateSelection_ch2(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'CandidateSelection' routing for conn 'ch2'\n");
    #endif
    {
        SMEDLValue new_identities[2] = {
                params[1],
                {SMEDL_NULL},
            };

        SMEDLValue new_params[2] = {
                params[0],
                params[1],
            };

        if (!process_CandidateSelection_candidate(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_CandidateSelection_ch3(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'CandidateSelection' routing for conn 'ch3'\n");
    #endif
    {
        SMEDLValue new_identities[2] = {
                {SMEDL_NULL},
                {SMEDL_NULL},
            };

        SMEDLValue *new_params = NULL;

        if (!process_CandidateSelection_end(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_CandidateSelection_ch5(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'CandidateSelection' routing for conn 'ch5'\n");
    #endif
    {
        SMEDLValue new_identities[2] = {
                identities[2],
                identities[1],
            };

        SMEDLValue *new_params = NULL;

        if (!process_CandidateSelection_valid(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}

/* Intra queue processing function - Route events to the local wrappers. Return
 * nonzero on success, zero on failure. */
static int handle_CandidateSelection_intra() {
    int success = 1;
    int channel;
    SMEDLValue *identities, *params;
    void *aux;

    while (pop_global_event(&intra_queue, &channel, &identities, &params, &aux)) {
        switch (channel) {
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
static int handle_CandidateSelection_inter() {
    int success = 1;
    int channel;
    SMEDLValue *identities, *params;
    void *aux;

    while (pop_global_event(&inter_queue, &channel, &identities, &params, &aux)) {
        switch (channel) {
            case CHANNEL_CandidateSelection_ch6:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'CandidateSelection' exporting for conn 'ch6'\n");
#endif
                if (cb_ch6 != NULL) {
                    success = success &&
                        cb_ch6(identities, params, aux);
                }
                free(identities[0].v.s);
                free(identities[1].v.s);
                free(params[0].v.s);
                break;
            case CHANNEL_CandidateSelection_ch8:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'CandidateSelection' exporting for conn 'ch8'\n");
#endif
                if (cb_ch8 != NULL) {
                    success = success &&
                        cb_ch8(identities, params, aux);
                }
                free(identities[0].v.s);
                free(identities[1].v.s);
                break;
            case CHANNEL_CandidateSelection_ch9:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'CandidateSelection' exporting for conn 'ch9'\n");
#endif
                if (cb_ch9 != NULL) {
                    success = success &&
                        cb_ch9(identities, params, aux);
                }
                free(identities[0].v.s);
                free(identities[1].v.s);
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
static int handle_CandidateSelection_queues() {
    int success = handle_CandidateSelection_intra();
    return handle_CandidateSelection_inter() && success;
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
int raise_CandidateSelection_shouldrank(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 2);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 1);
    if (!push_global_event(&inter_queue, CHANNEL_CandidateSelection_ch6, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 1);
        return 0;
    }
}
int raise_CandidateSelection_addP(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 2);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 0);
    if (!push_global_event(&inter_queue, CHANNEL_CandidateSelection_ch8, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 0);
        return 0;
    }
}
int raise_CandidateSelection_result(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 2);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 1);
    if (!push_global_event(&inter_queue, CHANNEL_CandidateSelection_ch9, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 1);
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

int import_CandidateSelection_ch1(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_CandidateSelection_ch1(identities, params, aux);
    return handle_CandidateSelection_queues() && success;
}

int import_CandidateSelection_ch2(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_CandidateSelection_ch2(identities, params, aux);
    return handle_CandidateSelection_queues() && success;
}

int import_CandidateSelection_ch3(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_CandidateSelection_ch3(identities, params, aux);
    return handle_CandidateSelection_queues() && success;
}

int import_CandidateSelection_ch5(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_CandidateSelection_ch5(identities, params, aux);
    return handle_CandidateSelection_queues() && success;
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

void callback_CandidateSelection_ch6(SMEDLCallback cb_func) {
    cb_ch6 = cb_func;
}

void callback_CandidateSelection_ch8(SMEDLCallback cb_func) {
    cb_ch8 = cb_func;
}

void callback_CandidateSelection_ch9(SMEDLCallback cb_func) {
    cb_ch9 = cb_func;
}

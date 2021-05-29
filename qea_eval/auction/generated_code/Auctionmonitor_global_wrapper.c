#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include "smedl_types.h"
#include "global_event_queue.h"
#include "Auctionmonitor_global_wrapper.h"
#include "Auctionmonitor_local_wrapper.h"
#include "Auctionmonitor_mon.h"

/* Global event queues - containing exported events */
static GlobalEventQueue intra_queue;
static GlobalEventQueue inter_queue;

/* Callback function pointers */
static SMEDLCallback cb_Auctionmonitor_alarm_recreation;
static SMEDLCallback cb_Auctionmonitor_alarm_low_bid;
static SMEDLCallback cb_Auctionmonitor_alarm_sold_early;
static SMEDLCallback cb_Auctionmonitor_alarm_not_sold;
static SMEDLCallback cb_Auctionmonitor_alarm_action_after_end;
static SMEDLCallback cb_Auctionmonitor_alarm_action_before_start;

/* Initialization interface - Initialize the global wrapper. Must be called once
 * before importing any events. Return nonzero on success, zero on failure. */
int init_Auctionmonitor_syncset() {
    /* Initialize all local wrappers */
    init_Auctionmonitor_local_wrapper();

    return 1;

fail_init_Auctionmonitor:
    return 0;
}

/* Cleanup interface - Tear down and free the resources used by this global
 * wrapper and all the local wrappers and monitors it manages. */
void free_Auctionmonitor_syncset() {
    /* Free local wrappers */
    free_Auctionmonitor_local_wrapper();

    /* Unset callbacks */
    cb_Auctionmonitor_alarm_recreation = NULL;
    cb_Auctionmonitor_alarm_low_bid = NULL;
    cb_Auctionmonitor_alarm_sold_early = NULL;
    cb_Auctionmonitor_alarm_not_sold = NULL;
    cb_Auctionmonitor_alarm_action_after_end = NULL;
    cb_Auctionmonitor_alarm_action_before_start = NULL;
}

/* Intra routing function - Called by import interface functions and intra queue
 * processing function to route events to the local wrappers.
 * Return nonzero on success, zero on failure. */
int route_Auctionmonitor_ch1(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'Auctionmonitor' routing for conn 'ch1'\n");
    #endif
    {
        SMEDLValue new_identities[1] = {
                params[0],
            };

        SMEDLValue new_params[3] = {
                params[0],
                params[1],
                params[2],
            };

        if (!process_Auctionmonitor_create_auction(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_Auctionmonitor_ch2(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'Auctionmonitor' routing for conn 'ch2'\n");
    #endif
    {
        SMEDLValue new_identities[1] = {
                params[0],
            };

        SMEDLValue new_params[2] = {
                params[0],
                params[1],
            };

        if (!process_Auctionmonitor_bid(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_Auctionmonitor_ch3(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'Auctionmonitor' routing for conn 'ch3'\n");
    #endif
    {
        SMEDLValue new_identities[1] = {
                params[0],
            };

        SMEDLValue new_params[1] = {
                params[0],
            };

        if (!process_Auctionmonitor_sold(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}
int route_Auctionmonitor_ch4(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    #if DEBUG >= 4
    fprintf(stderr, "Global wrapper 'Auctionmonitor' routing for conn 'ch4'\n");
    #endif
    {
        SMEDLValue new_identities[1] = {
                {SMEDL_NULL},
            };

        SMEDLValue *new_params = NULL;

        if (!process_Auctionmonitor_end_of_day(new_identities, new_params, aux)) {
            /* malloc fail */
            return 0;
        }
    }
}

/* Intra queue processing function - Route events to the local wrappers. Return
 * nonzero on success, zero on failure. */
static int handle_Auctionmonitor_intra() {
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
static int handle_Auctionmonitor_inter() {
    int success = 1;
    int channel;
    SMEDLValue *identities, *params;
    void *aux;

    while (pop_global_event(&inter_queue, &channel, &identities, &params, &aux)) {
        switch (channel) {
            case CHANNEL_Auctionmonitor_Auctionmonitor_alarm_recreation:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'Auctionmonitor' exporting for conn 'Auctionmonitor_alarm_recreation'\n");
#endif
                if (cb_Auctionmonitor_alarm_recreation != NULL) {
                    success = success &&
                        cb_Auctionmonitor_alarm_recreation(identities, params, aux);
                }
                break;
            case CHANNEL_Auctionmonitor_Auctionmonitor_alarm_low_bid:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'Auctionmonitor' exporting for conn 'Auctionmonitor_alarm_low_bid'\n");
#endif
                if (cb_Auctionmonitor_alarm_low_bid != NULL) {
                    success = success &&
                        cb_Auctionmonitor_alarm_low_bid(identities, params, aux);
                }
                break;
            case CHANNEL_Auctionmonitor_Auctionmonitor_alarm_sold_early:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'Auctionmonitor' exporting for conn 'Auctionmonitor_alarm_sold_early'\n");
#endif
                if (cb_Auctionmonitor_alarm_sold_early != NULL) {
                    success = success &&
                        cb_Auctionmonitor_alarm_sold_early(identities, params, aux);
                }
                break;
            case CHANNEL_Auctionmonitor_Auctionmonitor_alarm_not_sold:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'Auctionmonitor' exporting for conn 'Auctionmonitor_alarm_not_sold'\n");
#endif
                if (cb_Auctionmonitor_alarm_not_sold != NULL) {
                    success = success &&
                        cb_Auctionmonitor_alarm_not_sold(identities, params, aux);
                }
                break;
            case CHANNEL_Auctionmonitor_Auctionmonitor_alarm_action_after_end:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'Auctionmonitor' exporting for conn 'Auctionmonitor_alarm_action_after_end'\n");
#endif
                if (cb_Auctionmonitor_alarm_action_after_end != NULL) {
                    success = success &&
                        cb_Auctionmonitor_alarm_action_after_end(identities, params, aux);
                }
                break;
            case CHANNEL_Auctionmonitor_Auctionmonitor_alarm_action_before_start:
#if DEBUG >= 4
                fprintf(stderr, "Global wrapper 'Auctionmonitor' exporting for conn 'Auctionmonitor_alarm_action_before_start'\n");
#endif
                if (cb_Auctionmonitor_alarm_action_before_start != NULL) {
                    success = success &&
                        cb_Auctionmonitor_alarm_action_before_start(identities, params, aux);
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
static int handle_Auctionmonitor_queues() {
    int success = handle_Auctionmonitor_intra();
    return handle_Auctionmonitor_inter() && success;
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
int raise_Auctionmonitor_alarm_recreation(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 1);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 0);
    if (!push_global_event(&inter_queue, CHANNEL_Auctionmonitor_Auctionmonitor_alarm_recreation, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 0);
        return 0;
    }
}
int raise_Auctionmonitor_alarm_low_bid(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 1);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 0);
    if (!push_global_event(&inter_queue, CHANNEL_Auctionmonitor_Auctionmonitor_alarm_low_bid, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 0);
        return 0;
    }
}
int raise_Auctionmonitor_alarm_sold_early(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 1);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 0);
    if (!push_global_event(&inter_queue, CHANNEL_Auctionmonitor_Auctionmonitor_alarm_sold_early, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 0);
        return 0;
    }
}
int raise_Auctionmonitor_alarm_not_sold(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 1);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 0);
    if (!push_global_event(&inter_queue, CHANNEL_Auctionmonitor_Auctionmonitor_alarm_not_sold, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 0);
        return 0;
    }
}
int raise_Auctionmonitor_alarm_action_after_end(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 1);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 0);
    if (!push_global_event(&inter_queue, CHANNEL_Auctionmonitor_Auctionmonitor_alarm_action_after_end, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 0);
        return 0;
    }
}
int raise_Auctionmonitor_alarm_action_before_start(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 1);
    /* Store on inter queue */
    SMEDLValue *params_inter = smedl_copy_array(params, 0);
    if (!push_global_event(&inter_queue, CHANNEL_Auctionmonitor_Auctionmonitor_alarm_action_before_start, ids_copy, params_inter, aux)) {
        /* malloc fail */
        smedl_free_array(params_inter, 0);
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

int import_Auctionmonitor_ch1(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_Auctionmonitor_ch1(identities, params, aux);
    return handle_Auctionmonitor_queues() && success;
}

int import_Auctionmonitor_ch2(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_Auctionmonitor_ch2(identities, params, aux);
    return handle_Auctionmonitor_queues() && success;
}

int import_Auctionmonitor_ch3(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_Auctionmonitor_ch3(identities, params, aux);
    return handle_Auctionmonitor_queues() && success;
}

int import_Auctionmonitor_ch4(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    int success = route_Auctionmonitor_ch4(identities, params, aux);
    return handle_Auctionmonitor_queues() && success;
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

void callback_Auctionmonitor_Auctionmonitor_alarm_recreation(SMEDLCallback cb_func) {
    cb_Auctionmonitor_alarm_recreation = cb_func;
}

void callback_Auctionmonitor_Auctionmonitor_alarm_low_bid(SMEDLCallback cb_func) {
    cb_Auctionmonitor_alarm_low_bid = cb_func;
}

void callback_Auctionmonitor_Auctionmonitor_alarm_sold_early(SMEDLCallback cb_func) {
    cb_Auctionmonitor_alarm_sold_early = cb_func;
}

void callback_Auctionmonitor_Auctionmonitor_alarm_not_sold(SMEDLCallback cb_func) {
    cb_Auctionmonitor_alarm_not_sold = cb_func;
}

void callback_Auctionmonitor_Auctionmonitor_alarm_action_after_end(SMEDLCallback cb_func) {
    cb_Auctionmonitor_alarm_action_after_end = cb_func;
}

void callback_Auctionmonitor_Auctionmonitor_alarm_action_before_start(SMEDLCallback cb_func) {
    cb_Auctionmonitor_alarm_action_before_start = cb_func;
}

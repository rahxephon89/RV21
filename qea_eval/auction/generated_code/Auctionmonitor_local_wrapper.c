#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "smedl_types.h"
#include "monitor_map.h"
#include "Auctionmonitor_global_wrapper.h"
#include "Auctionmonitor_local_wrapper.h"
#include "Auctionmonitor_mon.h"

/* Auctionmonitor Monitor Maps - One for each subset of non-wildcard identities
 * used for lookups.
 *
 * For fast lookups, SMEDL keeps a monitor map for each subset of non-wildcard
 * identities used. For example, with the following connections:
 *
 *     ev1 => Mon1[$0, $1, $2].ev1();
 *     ev2 => Mon1[$0, *, $1].ev2();
 *
 * There would be two monitor maps: one that hashes all three identities, and
 * one that hashes the first and third. For the latter, any monitors with
 * identical first and third identities would be grouped together in the hash
 * table (as a linked list).
 *
 * The monitor map that hashes all identities (i.e. no wildcards) is always
 * called "monitor_map_all". The monitor map that stores all monitors in the
 * same bucket (i.e. all identities are wildcard), if present, is called
 * "monitor_map_none".
 */
static MonitorMap monitor_map_all;
static MonitorMap monitor_map_none;

/* Monitor map hash functions - One for each monitor map */

static uint64_t hash_all(SMEDLValue *ids) {
    murmur_state s = MURMUR_INIT(0);
    murmur(&ids[0].v.i, sizeof(ids[0].v.i), &s);
    return murmur_f(&s);
}

static uint64_t hash_none(SMEDLValue *ids) {
    murmur_state s = MURMUR_INIT(0);
    return murmur_f(&s);
}

/* Monitor map equals functions - One for each monitor map */

static int equals_all(SMEDLValue *ids1, SMEDLValue *ids2) {
    if (ids1[0].v.i != ids2[0].v.i) {
        return 0;
    }
    return 1;
}

static int equals_none(SMEDLValue *ids1, SMEDLValue *ids2) {
    return 1;
}

/* Register the global wrapper's export callbacks with the monitor */
static void setup_Auctionmonitor_callbacks(AuctionmonitorMonitor *mon) {
    register_Auctionmonitor_alarm_recreation(mon, raise_Auctionmonitor_alarm_recreation);
    register_Auctionmonitor_alarm_low_bid(mon, raise_Auctionmonitor_alarm_low_bid);
    register_Auctionmonitor_alarm_sold_early(mon, raise_Auctionmonitor_alarm_sold_early);
    register_Auctionmonitor_alarm_not_sold(mon, raise_Auctionmonitor_alarm_not_sold);
    register_Auctionmonitor_alarm_action_after_end(mon, raise_Auctionmonitor_alarm_action_after_end);
    register_Auctionmonitor_alarm_action_before_start(mon, raise_Auctionmonitor_alarm_action_before_start);
    registercleanup_Auctionmonitor(mon, recycle_Auctionmonitor_monitor);
}

/* Initialization interface - Initialize the local wrapper. Must be called once
 * before creating any monitors or importing any events.
 * Return nonzero on success, zero on failure. */
int init_Auctionmonitor_local_wrapper() {
    if (!monitormap_init(&monitor_map_all, offsetof(AuctionmonitorMonitor, identities), hash_all, equals_all)) {
        goto fail_init_monitor_map_all;
    }
    if (!monitormap_init(&monitor_map_none, offsetof(AuctionmonitorMonitor, identities), hash_none, equals_none)) {
        goto fail_init_monitor_map_none;
    }

    return 1;

fail_init_monitor_map_none:
    monitormap_free(&monitor_map_all, 0);
fail_init_monitor_map_all:
    return 0;
}

/* Cleanup interface - Tear down and free the resources used by this local
 * wrapper and all the monitors it manages */
void free_Auctionmonitor_local_wrapper() {
    MonitorInstance *instances = monitormap_free(&monitor_map_all, 1);
    monitormap_free(&monitor_map_none, 0);

    while (instances != NULL) {
        MonitorInstance *tmp = instances->next;
        smedl_free_array(((AuctionmonitorMonitor *) instances->mon)->identities, 1);
        free_Auctionmonitor_monitor(instances->mon);
        free(instances);
        instances = tmp;
    }
}

/* Creation interface - Instantiate a new Auctionmonitor monitor.
 * Return nonzero on success or if monitor already exists, zero on failure.
 *
 * Parameters:
 * identites - An array of SMEDLValue of the proper length for this monitor.
 *   Must be fully specified; no wildcards.
 * init_state - A pointer to a AuctionmonitorState containing
 *   the initial state variable values for this monitor. A default initial
 *   state can be retrieved with default_Auctionmonitor_state()
 *   and then just the desired variables can be updated. */
int create_Auctionmonitor_monitor(SMEDLValue *identities, AuctionmonitorState *init_state) {
    /* Check if monitor with identities already exists */
    if (monitormap_lookup(&monitor_map_all, identities) != NULL) {
#if DEBUG >= 4
        fprintf(stderr, "Local wrapper 'Auctionmonitor' skipping explicit creation for existing monitor\n");
#endif
        return 1;
    }
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'Auctionmonitor' doing explicit creation\n");
#endif

    /* Initialize new monitor with identities and state */
    SMEDLValue *ids_copy = smedl_copy_array(identities, 1);
    if (ids_copy == NULL) {
        /* malloc fail */
        return 0;
    }
    AuctionmonitorMonitor *mon = init_Auctionmonitor_with_state(ids_copy, init_state);
    if (mon == NULL) {
        /* malloc fail */
        smedl_free_array(ids_copy, 1);
        return 0;
    }
    setup_Auctionmonitor_callbacks(mon);

    /* Store monitor in maps */
    if (add_Auctionmonitor_monitor(mon) == NULL) {
        /* malloc fail */
        free_Auctionmonitor_monitor(mon);
        smedl_free_array(ids_copy, 1);
        return 0;
    }
    return 1;
}

/* Event import interfaces - Send the respective event to the monitor(s) and
 * potentially perform dynamic instantiation.
 * Return nonzero on success, zero on failure.
 *
 * Parameters:
 * identites - An array of SMEDLValue of the proper length for this monitor.
 *   Wildcards can be represented with a SMEDLValue of type SMEDL_NULL.
 * params - An array of SMEDLValue, one for each parameter of the event.
 * aux - Extra data that is passed through to exported events unchanged. */

int process_Auctionmonitor_create_auction(SMEDLValue *identities, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'Auctionmonitor' processing event 'create_auction'\n");
#endif
    /* Fetch the monitors to send the event to or do dynamic instantiation if
     * necessary */
    MonitorInstance *instances = get_Auctionmonitor_monitors(identities);
    if (instances == INVALID_INSTANCE) {
        /* malloc fail */
        return 0;
    }

    /* Send the event to each monitor */
    int success = 1;
    while (instances != NULL) {
        AuctionmonitorMonitor *mon = instances->mon;
        instances = instances->next;
        if (!execute_Auctionmonitor_create_auction(mon, params, aux)) {
            success = 0;
        }
    }
    return success;
}

int process_Auctionmonitor_bid(SMEDLValue *identities, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'Auctionmonitor' processing event 'bid'\n");
#endif
    /* Fetch the monitors to send the event to or do dynamic instantiation if
     * necessary */
    MonitorInstance *instances = get_Auctionmonitor_monitors(identities);
    if (instances == INVALID_INSTANCE) {
        /* malloc fail */
        return 0;
    }

    /* Send the event to each monitor */
    int success = 1;
    while (instances != NULL) {
        AuctionmonitorMonitor *mon = instances->mon;
        instances = instances->next;
        if (!execute_Auctionmonitor_bid(mon, params, aux)) {
            success = 0;
        }
    }
    return success;
}

int process_Auctionmonitor_sold(SMEDLValue *identities, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'Auctionmonitor' processing event 'sold'\n");
#endif
    /* Fetch the monitors to send the event to or do dynamic instantiation if
     * necessary */
    MonitorInstance *instances = get_Auctionmonitor_monitors(identities);
    if (instances == INVALID_INSTANCE) {
        /* malloc fail */
        return 0;
    }

    /* Send the event to each monitor */
    int success = 1;
    while (instances != NULL) {
        AuctionmonitorMonitor *mon = instances->mon;
        instances = instances->next;
        if (!execute_Auctionmonitor_sold(mon, params, aux)) {
            success = 0;
        }
    }
    return success;
}

int process_Auctionmonitor_end_of_day(SMEDLValue *identities, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'Auctionmonitor' processing event 'end_of_day'\n");
#endif
    /* Fetch the monitors to send the event to or do dynamic instantiation if
     * necessary */
    MonitorInstance *instances = get_Auctionmonitor_monitors(identities);
    if (instances == INVALID_INSTANCE) {
        /* malloc fail */
        return 0;
    }

    /* Send the event to each monitor */
    int success = 1;
    while (instances != NULL) {
        AuctionmonitorMonitor *mon = instances->mon;
        instances = instances->next;
        if (!execute_Auctionmonitor_end_of_day(mon, params, aux)) {
            success = 0;
        }
    }
    return success;
}

/* Recycle a monitor instance - Used as the callback for when final states are
 * reached in the monitor. Return nonzero if successful, zero on failure. */
int recycle_Auctionmonitor_monitor(AuctionmonitorMonitor *mon) {
#if DEBUG >= 4
        fprintf(stderr, "Recycling an instance of 'Auctionmonitor'\n");
#endif
    monitormap_remove(&monitor_map_all, mon);
    smedl_free_array(mon->identities, 1);
    free_Auctionmonitor_monitor(mon);
    return 1;
}

/* Add the provided monitor to the monitor maps. Return a
 * MonitorInstance, or NULL if unsuccessful. */
MonitorInstance * add_Auctionmonitor_monitor(AuctionmonitorMonitor *mon) {
    MonitorInstance *prev_inst = NULL;
    MonitorMap *prev_map = NULL;
    MonitorInstance *inst;

    inst = monitormap_insert(&monitor_map_none, mon, prev_inst, prev_map);
    if (inst == NULL) {
        return NULL;
    }
    prev_inst = inst;
    prev_map = &monitor_map_none;

    inst = monitormap_insert(&monitor_map_all, mon, prev_inst, prev_map);
    if (inst == NULL) {
        monitormap_removeinst(prev_map, prev_inst);
        return NULL;
    }

    return inst;
}

/* Fetch a list of monitor instances matching the given identities.
 *
 * Identities are given as an array of SMEDLValue. Wildcards for multicast
 * are indicated with SMEDLValues with type SMEDL_NULL.
 *
 * If there are no matching monitor instances but the identity list is fully
 * specified (i.e. there are no wildcards), create an instance with those
 * identities and return it.
 *
 * Returns a linked list of MonitorInstance (which may be empty, i.e. NULL).
 * If dynamic instantiation fails, returns INVALID_INSTANCE. */
MonitorInstance * get_Auctionmonitor_monitors(SMEDLValue *identities) {
    /* Look up from the proper monitor map */
    MonitorInstance *instances;
    int dynamic_instantiation = 0;
    if (identities[0].t == SMEDL_NULL) {
        instances = monitormap_lookup(&monitor_map_none, identities);
    } else {
        instances = monitormap_lookup(&monitor_map_all, identities);
        dynamic_instantiation = 1;
    }

    /* Do dynamic instantiation if wildcards were fully specified and there
     * are no matching monitors */
    if (instances == NULL && dynamic_instantiation) {
#if DEBUG >= 4
        fprintf(stderr, "Dynamic instantiation for 'Auctionmonitor'\n");
#endif
        SMEDLValue *ids_copy = smedl_copy_array(identities, 1);
        if (ids_copy == NULL) {
            /* malloc fail */
            return INVALID_INSTANCE;
        }
        AuctionmonitorMonitor *mon = init_Auctionmonitor_monitor(ids_copy);
        if (mon == NULL) {
            /* malloc fail */
            smedl_free_array(ids_copy, 1);
            return INVALID_INSTANCE;
        }
        setup_Auctionmonitor_callbacks(mon);
        instances = add_Auctionmonitor_monitor(mon);
        if (instances == NULL) {
            /* malloc fail */
            free_Auctionmonitor_monitor(mon);
            smedl_free_array(ids_copy, 1);
            return INVALID_INSTANCE;
        }
    }

    return instances;
}

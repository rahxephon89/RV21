#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "smedl_types.h"
#include "monitor_map.h"
#include "CandidateRank_global_wrapper.h"
#include "CandidateRank_local_wrapper.h"
#include "CandidateRank_mon.h"

/* CandidateRank Monitor Maps - One for each subset of non-wildcard identities
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
static MonitorMap monitor_map_0_1;
static MonitorMap monitor_map_all;

/* Monitor map hash functions - One for each monitor map */

static uint64_t hash_0_1(SMEDLValue *ids) {
    murmur_state s = MURMUR_INIT(0);
    murmur(ids[0].v.s, strlen(ids[0].v.s), &s);
    murmur(ids[1].v.s, strlen(ids[1].v.s), &s);
    return murmur_f(&s);
}

static uint64_t hash_all(SMEDLValue *ids) {
    murmur_state s = MURMUR_INIT(0);
    murmur(ids[0].v.s, strlen(ids[0].v.s), &s);
    murmur(ids[1].v.s, strlen(ids[1].v.s), &s);
    murmur(ids[2].v.s, strlen(ids[2].v.s), &s);
    return murmur_f(&s);
}

/* Monitor map equals functions - One for each monitor map */

static int equals_0_1(SMEDLValue *ids1, SMEDLValue *ids2) {
    if (strcmp(ids1[0].v.s, ids2[0].v.s)) {
        return 0;
    }
    if (strcmp(ids1[1].v.s, ids2[1].v.s)) {
        return 0;
    }
    return 1;
}

static int equals_all(SMEDLValue *ids1, SMEDLValue *ids2) {
    if (strcmp(ids1[0].v.s, ids2[0].v.s)) {
        return 0;
    }
    if (strcmp(ids1[1].v.s, ids2[1].v.s)) {
        return 0;
    }
    if (strcmp(ids1[2].v.s, ids2[2].v.s)) {
        return 0;
    }
    return 1;
}

/* Register the global wrapper's export callbacks with the monitor */
static void setup_CandidateRank_callbacks(CandidateRankMonitor *mon) {
    register_CandidateRank_valid(mon, raise_CandidateRank_valid);
    registercleanup_CandidateRank(mon, recycle_CandidateRank_monitor);
}

/* Initialization interface - Initialize the local wrapper. Must be called once
 * before creating any monitors or importing any events.
 * Return nonzero on success, zero on failure. */
int init_CandidateRank_local_wrapper() {
    if (!monitormap_init(&monitor_map_0_1, offsetof(CandidateRankMonitor, identities), hash_0_1, equals_0_1)) {
        goto fail_init_monitor_map_0_1;
    }
    if (!monitormap_init(&monitor_map_all, offsetof(CandidateRankMonitor, identities), hash_all, equals_all)) {
        goto fail_init_monitor_map_all;
    }

    return 1;

fail_init_monitor_map_all:
    monitormap_free(&monitor_map_0_1, 0);
fail_init_monitor_map_0_1:
    return 0;
}

/* Cleanup interface - Tear down and free the resources used by this local
 * wrapper and all the monitors it manages */
void free_CandidateRank_local_wrapper() {
    MonitorInstance *instances = monitormap_free(&monitor_map_all, 1);
    monitormap_free(&monitor_map_0_1, 0);

    while (instances != NULL) {
        MonitorInstance *tmp = instances->next;
        smedl_free_array(((CandidateRankMonitor *) instances->mon)->identities, 3);
        free_CandidateRank_monitor(instances->mon);
        free(instances);
        instances = tmp;
    }
}

/* Creation interface - Instantiate a new CandidateRank monitor.
 * Return nonzero on success or if monitor already exists, zero on failure.
 *
 * Parameters:
 * identites - An array of SMEDLValue of the proper length for this monitor.
 *   Must be fully specified; no wildcards.
 * init_state - A pointer to a CandidateRankState containing
 *   the initial state variable values for this monitor. A default initial
 *   state can be retrieved with default_CandidateRank_state()
 *   and then just the desired variables can be updated. */
int create_CandidateRank_monitor(SMEDLValue *identities, CandidateRankState *init_state) {
    /* Check if monitor with identities already exists */
    if (monitormap_lookup(&monitor_map_all, identities) != NULL) {
#if DEBUG >= 4
        fprintf(stderr, "Local wrapper 'CandidateRank' skipping explicit creation for existing monitor\n");
#endif
        return 1;
    }
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'CandidateRank' doing explicit creation\n");
#endif

    /* Initialize new monitor with identities and state */
    SMEDLValue *ids_copy = smedl_copy_array(identities, 3);
    if (ids_copy == NULL) {
        /* malloc fail */
        return 0;
    }
    CandidateRankMonitor *mon = init_CandidateRank_with_state(ids_copy, init_state);
    if (mon == NULL) {
        /* malloc fail */
        smedl_free_array(ids_copy, 3);
        return 0;
    }
    setup_CandidateRank_callbacks(mon);

    /* Store monitor in maps */
    if (add_CandidateRank_monitor(mon) == NULL) {
        /* malloc fail */
        free_CandidateRank_monitor(mon);
        smedl_free_array(ids_copy, 3);
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

int process_CandidateRank_shouldrank(SMEDLValue *identities, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'CandidateRank' processing event 'shouldrank'\n");
#endif
    /* Fetch the monitors to send the event to or do dynamic instantiation if
     * necessary */
    MonitorInstance *instances = get_CandidateRank_monitors(identities);
    if (instances == INVALID_INSTANCE) {
        /* malloc fail */
        return 0;
    }

    /* Send the event to each monitor */
    int success = 1;
    while (instances != NULL) {
        CandidateRankMonitor *mon = instances->mon;
        instances = instances->next;
        if (!execute_CandidateRank_shouldrank(mon, params, aux)) {
            success = 0;
        }
    }
    return success;
}

int process_CandidateRank_rank(SMEDLValue *identities, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'CandidateRank' processing event 'rank'\n");
#endif
    /* Fetch the monitors to send the event to or do dynamic instantiation if
     * necessary */
    MonitorInstance *instances = get_CandidateRank_monitors(identities);
    if (instances == INVALID_INSTANCE) {
        /* malloc fail */
        return 0;
    }

    /* Send the event to each monitor */
    int success = 1;
    while (instances != NULL) {
        CandidateRankMonitor *mon = instances->mon;
        instances = instances->next;
        if (!execute_CandidateRank_rank(mon, params, aux)) {
            success = 0;
        }
    }
    return success;
}

/* Recycle a monitor instance - Used as the callback for when final states are
 * reached in the monitor. Return nonzero if successful, zero on failure. */
int recycle_CandidateRank_monitor(CandidateRankMonitor *mon) {
#if DEBUG >= 4
        fprintf(stderr, "Recycling an instance of 'CandidateRank'\n");
#endif
    monitormap_remove(&monitor_map_all, mon);
    smedl_free_array(mon->identities, 3);
    free_CandidateRank_monitor(mon);
    return 1;
}

/* Add the provided monitor to the monitor maps. Return a
 * MonitorInstance, or NULL if unsuccessful. */
MonitorInstance * add_CandidateRank_monitor(CandidateRankMonitor *mon) {
    MonitorInstance *prev_inst = NULL;
    MonitorMap *prev_map = NULL;
    MonitorInstance *inst;

    inst = monitormap_insert(&monitor_map_0_1, mon, prev_inst, prev_map);
    if (inst == NULL) {
        return NULL;
    }
    prev_inst = inst;
    prev_map = &monitor_map_0_1;

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
MonitorInstance * get_CandidateRank_monitors(SMEDLValue *identities) {
    /* Look up from the proper monitor map */
    MonitorInstance *instances;
    int dynamic_instantiation = 0;
    if (identities[2].t == SMEDL_NULL) {
        instances = monitormap_lookup(&monitor_map_0_1, identities);
    } else {
        instances = monitormap_lookup(&monitor_map_all, identities);
        dynamic_instantiation = 1;
    }

    /* Do dynamic instantiation if wildcards were fully specified and there
     * are no matching monitors */
    if (instances == NULL && dynamic_instantiation) {
#if DEBUG >= 4
        fprintf(stderr, "Dynamic instantiation for 'CandidateRank'\n");
#endif
        SMEDLValue *ids_copy = smedl_copy_array(identities, 3);
        if (ids_copy == NULL) {
            /* malloc fail */
            return INVALID_INSTANCE;
        }
        CandidateRankMonitor *mon = init_CandidateRank_monitor(ids_copy);
        if (mon == NULL) {
            /* malloc fail */
            smedl_free_array(ids_copy, 3);
            return INVALID_INSTANCE;
        }
        setup_CandidateRank_callbacks(mon);
        instances = add_CandidateRank_monitor(mon);
        if (instances == NULL) {
            /* malloc fail */
            free_CandidateRank_monitor(mon);
            smedl_free_array(ids_copy, 3);
            return INVALID_INSTANCE;
        }
    }

    return instances;
}

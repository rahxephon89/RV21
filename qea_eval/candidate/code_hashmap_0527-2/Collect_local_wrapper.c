#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "smedl_types.h"
#include "monitor_map.h"
#include "Collect_global_wrapper.h"
#include "Collect_local_wrapper.h"
#include "Collect_mon.h"

/* Singleton monitor */
static CollectMonitor *monitor;

/* Register the global wrapper's export callbacks with the monitor */
static void setup_Collect_callbacks(CollectMonitor *mon) {
    register_Collect_result(mon, raise_Collect_result);
}

/* Initialization interface - Initialize the local wrapper. Must be called once
 * before creating any monitors or importing any events.
 * Return nonzero on success, zero on failure. */
int init_Collect_local_wrapper() {
    /* Initialize the singleton */
    monitor = init_Collect_monitor(NULL);
    if (monitor == NULL) {
        return 0;
    }
    setup_Collect_callbacks(monitor);
    return 1;
}

/* Cleanup interface - Tear down and free the resources used by this local
 * wrapper and all the monitors it manages */
void free_Collect_local_wrapper() {
    free_Collect_monitor(monitor);
}

/* Creation interface - Instantiate a new Collect monitor.
 * Return nonzero on success or if monitor already exists, zero on failure.
 *
 * Parameters:
 * identites - An array of SMEDLValue of the proper length for this monitor.
 *   Must be fully specified; no wildcards.
 * init_state - A pointer to a CollectState containing
 *   the initial state variable values for this monitor. A default initial
 *   state can be retrieved with default_Collect_state()
 *   and then just the desired variables can be updated. */
int create_Collect_monitor(SMEDLValue *identities, CollectState *init_state) {
    /* Singleton monitor - This is a no-op */
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

int process_Collect_addV(SMEDLValue *identities, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'Collect' processing event 'addV'\n");
#endif
    /* Send event to the singleton monitor */
    return execute_Collect_addV(monitor, params, aux);
}

int process_Collect_inRes(SMEDLValue *identities, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Local wrapper 'Collect' processing event 'inRes'\n");
#endif
    /* Send event to the singleton monitor */
    return execute_Collect_inRes(monitor, params, aux);
}

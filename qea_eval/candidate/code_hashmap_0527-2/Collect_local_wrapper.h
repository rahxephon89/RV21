#ifndef Collect_LOCAL_WRAPPER_H
#define Collect_LOCAL_WRAPPER_H

#include <stdint.h>
#include "smedl_types.h"
#include "monitor_map.h"
#include "Collect_mon.h"

/******************************************************************************
 * External Interface                                                         *
 ******************************************************************************/

/* Initialization interface - Initialize the local wrapper. Must be called once
 * before creating any monitors or importing any events.
 * Return nonzero on success, zero on failure. */
int init_Collect_local_wrapper();

/* Cleanup interface - Tear down and free the resources used by this local
 * wrapper and all the monitors it manages */
void free_Collect_local_wrapper();

/* Creation interface - Instantiate a new Collect monitor.
 * Return nonzero on success or if monitor already exists, zero on failure.
 *
 * Parameters:
 * identites - An array of SMEDLValue of the proper length for this monitor.
 * init_state - A pointer to a CollectState containing
 *   the initial state variable values for this monitor. A default initial
 *   state can be retrieved with default_Collect_state()
 *   and then just the desired variables can be updated. */
int create_Collect_monitor(SMEDLValue *identities, CollectState *init_state);

/* Event import interfaces - Send the respective event to the monitor(s) and
 * potentially perform dynamic instantiation.
 * Return nonzero on success, zero on failure.
 *
 * Parameters:
 * identites - An array of SMEDLValue of the proper length for this monitor.
 * params - An array of SMEDLValue, one for each parameter of the event.
 * aux - Extra data that is passed through to exported events unchanged. */
int process_Collect_addV(SMEDLValue *identities, SMEDLValue *params, void *aux);
int process_Collect_inRes(SMEDLValue *identities, SMEDLValue *params, void *aux);

/******************************************************************************
 * End of External Interface                                                  *
 ******************************************************************************/

#endif /* Collect_LOCAL_WRAPPER_H */

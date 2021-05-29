#ifndef CollectV_MON_H
#define CollectV_MON_H

#include "smedl_types.h"
#include "event_queue.h"

/* Internal/exported event enum for action queues */
typedef enum {
    EVENT_CollectV_check,
    EVENT_CollectV_result,
    EVENT_CollectV_addV,
} CollectVEvent;

/* Scenario state enums */
typedef enum {
    STATE_CollectV_sce_init,
    STATE_CollectV_sce_start,
} CollectV_sce_State;
typedef enum {
    STATE_CollectV_sce1_start,
} CollectV_sce1_State;
typedef enum {
    STATE_CollectV_sce2_start,
} CollectV_sce2_State;

/* State variables for CollectV.
 * Used for initialization as well as in the CollectVMonitor
 * struct. */
typedef struct CollectVState {
    int pNum;
    int res;
} CollectVState;

/* CollectV monitor struct.
 * Maintains all of the internal state for the monitor. */
typedef struct CollectVMonitor {
    /* Array of monitor's identities */
    SMEDLValue *identities;

    /* Scenario states */
    CollectV_sce_State sce_state;
    CollectV_sce1_State sce1_state;
    CollectV_sce2_State sce2_state;

    /* Scenario execution flags (ensures each scenario only processes one event
     * per macro-step) */
    struct {
        unsigned int sce_flag : 1;
        unsigned int sce1_flag : 1;
        unsigned int sce2_flag : 1;
    } ef;

    /* State variables */
    CollectVState s;

    /* Exported event callback pointers */
    SMEDLCallback callback_result;
    SMEDLCallback callback_addV;

    /* Cleanup callback pointer */
    int (*cleanup)(struct CollectVMonitor *mon);

    /* Local event queue */
    EventQueue event_queue;

    //TODO mutex?
} CollectVMonitor;

/* Callback registration functions - Set the export callback for an exported
 * event. Set to NULL to unregister a callback. */
void register_CollectV_result(CollectVMonitor *mon, SMEDLCallback cb_func);
void register_CollectV_addV(CollectVMonitor *mon, SMEDLCallback cb_func);

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_CollectV_monitor(). It must accept the monitor pointer as a
 * parameter and return nonzero on success, zero on failure. */
void registercleanup_CollectV(CollectVMonitor *mon, int (*cleanup_func)(CollectVMonitor *mon));

/* Event handling functions:
 *
 * execute_* - For imported and internal events, process the event through the
 *   scenarios. For exported events, do that and then export by calling the
 *   callback. This is the function to call to "import" an imported event.
 * queue_* - Queue an internal or exported event for processing. ("Raise" the
 *   event.) Note that for exported events, this refers to internal queuing
 *   within the monitor. If the monitor belongs to a synchronous set, the global
 *   wrapper's queuing happens when the event is actually exported.
 * export_* - Export an exported event by calling the registered callback, if
 *   any.
 *
 * All return nonzero on success, zero on failure. Note that when an event
 * handler fails, it means the monitor is no longer consistent with its
 * specification, has very possibly dropped events, and is likely to misbehave
 * when handling future events. However, it is still safe to clean it up, and
 * it will not leak memory as long as that is done. */
int execute_CollectV_addP(CollectVMonitor *mon, SMEDLValue *params, void *aux);
int execute_CollectV_inRes(CollectVMonitor *mon, SMEDLValue *params, void *aux);
int execute_CollectV_check(CollectVMonitor *mon, SMEDLValue *params, void *aux);
int queue_CollectV_check(CollectVMonitor *mon, SMEDLValue *params, void *aux);
int execute_CollectV_result(CollectVMonitor *mon, SMEDLValue *params, void *aux);
int queue_CollectV_result(CollectVMonitor *mon, SMEDLValue *params, void *aux);
int export_CollectV_result(CollectVMonitor *mon, SMEDLValue *params, void *aux);
int execute_CollectV_addV(CollectVMonitor *mon, SMEDLValue *params, void *aux);
int queue_CollectV_addV(CollectVMonitor *mon, SMEDLValue *params, void *aux);
int export_CollectV_addV(CollectVMonitor *mon, SMEDLValue *params, void *aux);

/* Monitor management functions */

/* Initialize a CollectV monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_CollectV_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CollectVMonitor * init_CollectV_monitor(SMEDLValue *identities);

/* Fill the provided CollectVState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_CollectV_state(CollectVState *state);

/* Initialize a CollectV monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) defualt_CollectV_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_CollectV_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CollectVMonitor * init_CollectV_with_state(SMEDLValue *identities, CollectVState *init_state);

/* Free a CollectV monitor. NOTE: Does not free the identities. That must
 * be done by the caller, if necessary. */
void free_CollectV_monitor(CollectVMonitor *mon);

#endif /* CollectV_MON_H */

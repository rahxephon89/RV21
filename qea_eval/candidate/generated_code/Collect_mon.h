#ifndef Collect_MON_H
#define Collect_MON_H

#include "smedl_types.h"
#include "event_queue.h"

/* Internal/exported event enum for action queues */
typedef enum {
    EVENT_Collect_result,
} CollectEvent;

/* Scenario state enums */
typedef enum {
    STATE_Collect_sce_start,
} Collect_sce_State;
typedef enum {
    STATE_Collect_sce1_init,
    STATE_Collect_sce1_start,
} Collect_sce1_State;

/* State variables for Collect.
 * Used for initialization as well as in the CollectMonitor
 * struct. */
typedef struct CollectState {
    int vNum;
    int vNumTemp;
    int res;
} CollectState;

/* Collect monitor struct.
 * Maintains all of the internal state for the monitor. */
typedef struct CollectMonitor {
    /* Array of monitor's identities */
    SMEDLValue *identities;

    /* Scenario states */
    Collect_sce_State sce_state;
    Collect_sce1_State sce1_state;

    /* Scenario execution flags (ensures each scenario only processes one event
     * per macro-step) */
    struct {
        unsigned int sce_flag : 1;
        unsigned int sce1_flag : 1;
    } ef;

    /* State variables */
    CollectState s;

    /* Exported event callback pointers */
    SMEDLCallback callback_result;

    /* Cleanup callback pointer */
    int (*cleanup)(struct CollectMonitor *mon);

    /* Local event queue */
    EventQueue event_queue;

    //TODO mutex?
} CollectMonitor;

/* Callback registration functions - Set the export callback for an exported
 * event. Set to NULL to unregister a callback. */
void register_Collect_result(CollectMonitor *mon, SMEDLCallback cb_func);

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_Collect_monitor(). It must accept the monitor pointer as a
 * parameter and return nonzero on success, zero on failure. */
void registercleanup_Collect(CollectMonitor *mon, int (*cleanup_func)(CollectMonitor *mon));

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
int execute_Collect_addV(CollectMonitor *mon, SMEDLValue *params, void *aux);
int execute_Collect_inRes(CollectMonitor *mon, SMEDLValue *params, void *aux);
int execute_Collect_result(CollectMonitor *mon, SMEDLValue *params, void *aux);
int queue_Collect_result(CollectMonitor *mon, SMEDLValue *params, void *aux);
int export_Collect_result(CollectMonitor *mon, SMEDLValue *params, void *aux);

/* Monitor management functions */

/* Initialize a Collect monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_Collect_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CollectMonitor * init_Collect_monitor(SMEDLValue *identities);

/* Fill the provided CollectState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_Collect_state(CollectState *state);

/* Initialize a Collect monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) defualt_Collect_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_Collect_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CollectMonitor * init_Collect_with_state(SMEDLValue *identities, CollectState *init_state);

/* Free a Collect monitor. NOTE: Does not free the identities. That must
 * be done by the caller, if necessary. */
void free_Collect_monitor(CollectMonitor *mon);

#endif /* Collect_MON_H */

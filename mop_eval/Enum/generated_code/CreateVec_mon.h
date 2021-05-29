#ifndef CreateVec_MON_H
#define CreateVec_MON_H

#include "smedl_types.h"
#include "event_queue.h"

/* Internal/exported event enum for action queues */
typedef enum {
    EVENT_CreateVec_violation,
} CreateVecEvent;

/* Scenario state enums */
typedef enum {
    STATE_CreateVec_sce1_init,
    STATE_CreateVec_sce1_start,
    STATE_CreateVec_sce1_exec,
    STATE_CreateVec_sce1_ad,
} CreateVec_sce1_State;

/* State variables for CreateVec.
 * Used for initialization as well as in the CreateVecMonitor
 * struct. */
typedef struct CreateVecState {
} CreateVecState;

/* CreateVec monitor struct.
 * Maintains all of the internal state for the monitor. */
typedef struct CreateVecMonitor {
    /* Array of monitor's identities */
    SMEDLValue *identities;

    /* Scenario states */
    CreateVec_sce1_State sce1_state;

    /* Scenario execution flags (ensures each scenario only processes one event
     * per macro-step) */
    struct {
        unsigned int sce1_flag : 1;
    } ef;

    /* State variables */
    CreateVecState s;

    /* Exported event callback pointers */
    SMEDLCallback callback_violation;

    /* Cleanup callback pointer */
    int (*cleanup)(struct CreateVecMonitor *mon);

    /* Local event queue */
    EventQueue event_queue;

    //TODO mutex?
} CreateVecMonitor;

/* Callback registration functions - Set the export callback for an exported
 * event. Set to NULL to unregister a callback. */
void register_CreateVec_violation(CreateVecMonitor *mon, SMEDLCallback cb_func);

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_CreateVec_monitor(). It must accept the monitor pointer as a
 * parameter and return nonzero on success, zero on failure. */
void registercleanup_CreateVec(CreateVecMonitor *mon, int (*cleanup_func)(CreateVecMonitor *mon));

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
int execute_CreateVec_new_v(CreateVecMonitor *mon, SMEDLValue *params, void *aux);
int execute_CreateVec_create_e(CreateVecMonitor *mon, SMEDLValue *params, void *aux);
int execute_CreateVec_visit_e(CreateVecMonitor *mon, SMEDLValue *params, void *aux);
int execute_CreateVec_add_v(CreateVecMonitor *mon, SMEDLValue *params, void *aux);
int execute_CreateVec_violation(CreateVecMonitor *mon, SMEDLValue *params, void *aux);
int queue_CreateVec_violation(CreateVecMonitor *mon, SMEDLValue *params, void *aux);
int export_CreateVec_violation(CreateVecMonitor *mon, SMEDLValue *params, void *aux);

/* Monitor management functions */

/* Initialize a CreateVec monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_CreateVec_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CreateVecMonitor * init_CreateVec_monitor(SMEDLValue *identities);

/* Fill the provided CreateVecState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_CreateVec_state(CreateVecState *state);

/* Initialize a CreateVec monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) defualt_CreateVec_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_CreateVec_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CreateVecMonitor * init_CreateVec_with_state(SMEDLValue *identities, CreateVecState *init_state);

/* Free a CreateVec monitor. NOTE: Does not free the identities. That must
 * be done by the caller, if necessary. */
void free_CreateVec_monitor(CreateVecMonitor *mon);

#endif /* CreateVec_MON_H */

#ifndef CreateMCI_MON_H
#define CreateMCI_MON_H

#include "smedl_types.h"
#include "event_queue.h"

/* Internal/exported event enum for action queues */
typedef enum {
    EVENT_CreateMCI_violation,
} CreateMCIEvent;

/* Scenario state enums */
typedef enum {
    STATE_CreateMCI_sce1_init,
    STATE_CreateMCI_sce1_start,
    STATE_CreateMCI_sce1_updateM,
} CreateMCI_sce1_State;

/* State variables for CreateMCI.
 * Used for initialization as well as in the CreateMCIMonitor
 * struct. */
typedef struct CreateMCIState {
} CreateMCIState;

/* CreateMCI monitor struct.
 * Maintains all of the internal state for the monitor. */
typedef struct CreateMCIMonitor {
    /* Array of monitor's identities */
    SMEDLValue *identities;

    /* Scenario states */
    CreateMCI_sce1_State sce1_state;

    /* Scenario execution flags (ensures each scenario only processes one event
     * per macro-step) */
    struct {
        unsigned int sce1_flag : 1;
    } ef;

    /* State variables */
    CreateMCIState s;

    /* Exported event callback pointers */
    SMEDLCallback callback_violation;

    /* Cleanup callback pointer */
    int (*cleanup)(struct CreateMCIMonitor *mon);

    /* Local event queue */
    EventQueue event_queue;

    //TODO mutex?
} CreateMCIMonitor;

/* Callback registration functions - Set the export callback for an exported
 * event. Set to NULL to unregister a callback. */
void register_CreateMCI_violation(CreateMCIMonitor *mon, SMEDLCallback cb_func);

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_CreateMCI_monitor(). It must accept the monitor pointer as a
 * parameter and return nonzero on success, zero on failure. */
void registercleanup_CreateMCI(CreateMCIMonitor *mon, int (*cleanup_func)(CreateMCIMonitor *mon));

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
int execute_CreateMCI_new_mci(CreateMCIMonitor *mon, SMEDLValue *params, void *aux);
int execute_CreateMCI_traverse_m(CreateMCIMonitor *mon, SMEDLValue *params, void *aux);
int execute_CreateMCI_traverse_i(CreateMCIMonitor *mon, SMEDLValue *params, void *aux);
int execute_CreateMCI_violation(CreateMCIMonitor *mon, SMEDLValue *params, void *aux);
int queue_CreateMCI_violation(CreateMCIMonitor *mon, SMEDLValue *params, void *aux);
int export_CreateMCI_violation(CreateMCIMonitor *mon, SMEDLValue *params, void *aux);

/* Monitor management functions */

/* Initialize a CreateMCI monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_CreateMCI_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CreateMCIMonitor * init_CreateMCI_monitor(SMEDLValue *identities);

/* Fill the provided CreateMCIState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_CreateMCI_state(CreateMCIState *state);

/* Initialize a CreateMCI monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) defualt_CreateMCI_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_CreateMCI_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CreateMCIMonitor * init_CreateMCI_with_state(SMEDLValue *identities, CreateMCIState *init_state);

/* Free a CreateMCI monitor. NOTE: Does not free the identities. That must
 * be done by the caller, if necessary. */
void free_CreateMCI_monitor(CreateMCIMonitor *mon);

#endif /* CreateMCI_MON_H */

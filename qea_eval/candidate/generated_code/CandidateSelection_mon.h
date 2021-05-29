#ifndef CandidateSelection_MON_H
#define CandidateSelection_MON_H

#include "smedl_types.h"
#include "event_queue.h"

/* Internal/exported event enum for action queues */
typedef enum {
    EVENT_CandidateSelection_check,
    EVENT_CandidateSelection_shouldrank,
    EVENT_CandidateSelection_result,
    EVENT_CandidateSelection_addP,
} CandidateSelectionEvent;

/* Scenario state enums */
typedef enum {
    STATE_CandidateSelection_sce_init,
    STATE_CandidateSelection_sce_start,
} CandidateSelection_sce_State;
typedef enum {
    STATE_CandidateSelection_sce1_start,
} CandidateSelection_sce1_State;
typedef enum {
    STATE_CandidateSelection_sce2_start,
} CandidateSelection_sce2_State;

/* State variables for CandidateSelection.
 * Used for initialization as well as in the CandidateSelectionMonitor
 * struct. */
typedef struct CandidateSelectionState {
    int canNum;
} CandidateSelectionState;

/* CandidateSelection monitor struct.
 * Maintains all of the internal state for the monitor. */
typedef struct CandidateSelectionMonitor {
    /* Array of monitor's identities */
    SMEDLValue *identities;

    /* Scenario states */
    CandidateSelection_sce_State sce_state;
    CandidateSelection_sce1_State sce1_state;
    CandidateSelection_sce2_State sce2_state;

    /* Scenario execution flags (ensures each scenario only processes one event
     * per macro-step) */
    struct {
        unsigned int sce_flag : 1;
        unsigned int sce1_flag : 1;
        unsigned int sce2_flag : 1;
    } ef;

    /* State variables */
    CandidateSelectionState s;

    /* Exported event callback pointers */
    SMEDLCallback callback_shouldrank;
    SMEDLCallback callback_result;
    SMEDLCallback callback_addP;

    /* Cleanup callback pointer */
    int (*cleanup)(struct CandidateSelectionMonitor *mon);

    /* Local event queue */
    EventQueue event_queue;

    //TODO mutex?
} CandidateSelectionMonitor;

/* Callback registration functions - Set the export callback for an exported
 * event. Set to NULL to unregister a callback. */
void register_CandidateSelection_shouldrank(CandidateSelectionMonitor *mon, SMEDLCallback cb_func);
void register_CandidateSelection_result(CandidateSelectionMonitor *mon, SMEDLCallback cb_func);
void register_CandidateSelection_addP(CandidateSelectionMonitor *mon, SMEDLCallback cb_func);

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_CandidateSelection_monitor(). It must accept the monitor pointer as a
 * parameter and return nonzero on success, zero on failure. */
void registercleanup_CandidateSelection(CandidateSelectionMonitor *mon, int (*cleanup_func)(CandidateSelectionMonitor *mon));

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
int execute_CandidateSelection_member(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int execute_CandidateSelection_candidate(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int execute_CandidateSelection_countcan(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int execute_CandidateSelection_valid(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int execute_CandidateSelection_end(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int execute_CandidateSelection_check(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int queue_CandidateSelection_check(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int execute_CandidateSelection_shouldrank(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int queue_CandidateSelection_shouldrank(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int export_CandidateSelection_shouldrank(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int execute_CandidateSelection_result(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int queue_CandidateSelection_result(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int export_CandidateSelection_result(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int execute_CandidateSelection_addP(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int queue_CandidateSelection_addP(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);
int export_CandidateSelection_addP(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux);

/* Monitor management functions */

/* Initialize a CandidateSelection monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_CandidateSelection_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CandidateSelectionMonitor * init_CandidateSelection_monitor(SMEDLValue *identities);

/* Fill the provided CandidateSelectionState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_CandidateSelection_state(CandidateSelectionState *state);

/* Initialize a CandidateSelection monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) defualt_CandidateSelection_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_CandidateSelection_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CandidateSelectionMonitor * init_CandidateSelection_with_state(SMEDLValue *identities, CandidateSelectionState *init_state);

/* Free a CandidateSelection monitor. NOTE: Does not free the identities. That must
 * be done by the caller, if necessary. */
void free_CandidateSelection_monitor(CandidateSelectionMonitor *mon);

#endif /* CandidateSelection_MON_H */

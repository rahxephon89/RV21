#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "smedl_types.h"
#include "event_queue.h"
#include "CandidateSelection_mon.h"

/* Callback registration functions - Set the export callback for an exported
 * event */

void register_CandidateSelection_shouldrank(CandidateSelectionMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_shouldrank = cb_func;
}

void register_CandidateSelection_result(CandidateSelectionMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_result = cb_func;
}

void register_CandidateSelection_addP(CandidateSelectionMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_addP = cb_func;
}

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_CandidateSelection_monitor(). */
void registercleanup_CandidateSelection(CandidateSelectionMonitor *mon, int (*cleanup_func)(CandidateSelectionMonitor *mon)) {
    mon->cleanup = cleanup_func;
}

/* Queue processing function - Call the handlers for all the events in the
 * queue until it is empty.
 * Return nonzero if all handlers ran successfully, zero if not. */
static int handle_CandidateSelection_queue(CandidateSelectionMonitor *mon) {
    int success = 1;
    int event;
    SMEDLValue *params;
    void *aux;

    while (pop_event(&mon->event_queue, &event, &params, &aux)) {
        switch (event) {
            case EVENT_CandidateSelection_check:
                success = success &&
                    execute_CandidateSelection_check(mon, params, aux);
                break;
            case EVENT_CandidateSelection_shouldrank:
                success = success &&
                    execute_CandidateSelection_shouldrank(mon, params, aux);
                free(params[0].v.s);
                break;
            case EVENT_CandidateSelection_result:
                success = success &&
                    execute_CandidateSelection_result(mon, params, aux);
                break;
            case EVENT_CandidateSelection_addP:
                success = success &&
                    execute_CandidateSelection_addP(mon, params, aux);
                break;
        }

        free(params);
    }

    /* Macro-step is finished. */
    memset(&mon->ef, 0, sizeof(mon->ef));
    return success;
}

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
 * it will not leak memory as long as that is done.
 * TODO When could future events actually cause crashes? I suspect it's
 * possible when there are strings or opaques and helper functions, but not
 * otherwise. */

/* Imported events */

int execute_CandidateSelection_member(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' handling imported event 'member'\n");
#endif

    /* sce scenario */
    //if (!mon->ef.sce_flag) {
        switch (mon->sce_state) {
            case STATE_CandidateSelection_sce_init:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 0);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        queue_CandidateSelection_addP(mon, new_params, aux);
                    }

                    mon->sce_state = STATE_CandidateSelection_sce_start;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            case STATE_CandidateSelection_sce_start:
                if (1) {

                    mon->sce_state = STATE_CandidateSelection_sce_start;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            default:
                /* XXX Do something here: Scenario handles this event but not
                 * from this state */
                ;
        }
        //mon->ef.sce_flag = 1;
    //}

    /* Finish the macro-step */
    return handle_CandidateSelection_queue(mon);
}

int execute_CandidateSelection_candidate(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' handling imported event 'candidate'\n");
#endif

    /* sce scenario */
    //if (!mon->ef.sce_flag) {
        switch (mon->sce_state) {
            case STATE_CandidateSelection_sce_start:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 1);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        new_params[0].t = SMEDL_STRING;
                        if (!smedl_assign_string(&new_params[0].v.s, params[0].v.s)) {
                            /* malloc fail */
                            smedl_free_array(new_params, 0);
                            return 0;
                        }
                        queue_CandidateSelection_shouldrank(mon, new_params, aux);
                    }
                    mon->s.canNum++;

                    mon->sce_state = STATE_CandidateSelection_sce_start;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            default:
                /* XXX Do something here: Scenario handles this event but not
                 * from this state */
                ;
        }
        //mon->ef.sce_flag = 1;
    //}

    /* Finish the macro-step */
    return handle_CandidateSelection_queue(mon);
}

int execute_CandidateSelection_countcan(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' handling imported event 'countcan'\n");
#endif

    /* Finish the macro-step */
    return handle_CandidateSelection_queue(mon);
}

int execute_CandidateSelection_valid(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' handling imported event 'valid'\n");
#endif

    /* sce1 scenario */
    //if (!mon->ef.sce1_flag) {
        switch (mon->sce1_state) {
            case STATE_CandidateSelection_sce1_start:
                if (1) {
                    mon->s.canNum--;

                    mon->sce1_state = STATE_CandidateSelection_sce1_start;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            default:
                /* XXX Do something here: Scenario handles this event but not
                 * from this state */
                ;
        }
        //mon->ef.sce1_flag = 1;
    //}

    /* Finish the macro-step */
    return handle_CandidateSelection_queue(mon);
}

int execute_CandidateSelection_end(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' handling imported event 'end'\n");
#endif

    /* sce1 scenario */
    //if (!mon->ef.sce1_flag) {
        switch (mon->sce1_state) {
            case STATE_CandidateSelection_sce1_start:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 0);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        queue_CandidateSelection_check(mon, new_params, aux);
                    }

                    mon->sce1_state = STATE_CandidateSelection_sce1_start;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            default:
                /* XXX Do something here: Scenario handles this event but not
                 * from this state */
                ;
        }
        //mon->ef.sce1_flag = 1;
    //}

    /* Finish the macro-step */
    return handle_CandidateSelection_queue(mon);
}

/* Internal events */

int execute_CandidateSelection_check(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' handling internal event 'check'\n");
#endif

    /* sce2 scenario */
    //if (!mon->ef.sce2_flag) {
        switch (mon->sce2_state) {
            case STATE_CandidateSelection_sce2_start:
                if (mon->s.canNum == 0) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 1);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        new_params[0].t = SMEDL_INT;
                        new_params[0].v.i = 1;
                        queue_CandidateSelection_result(mon, new_params, aux);
                    }
                    mon->s.canNum = 0;

                    mon->sce2_state = STATE_CandidateSelection_sce2_start;
                } else if (mon->s.canNum > 0) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 1);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        new_params[0].t = SMEDL_INT;
                        new_params[0].v.i = 0;
                        queue_CandidateSelection_result(mon, new_params, aux);
                    }
                    mon->s.canNum = 0;

                    mon->sce2_state = STATE_CandidateSelection_sce2_start;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            default:
                /* XXX Do something here: Scenario handles this event but not
                 * from this state */
                ;
        }
        //mon->ef.sce2_flag = 1;
    //}

    return 1;
}

int queue_CandidateSelection_check(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' queuing raised internal event 'check'\n");
#endif

    return push_event(&mon->event_queue, EVENT_CandidateSelection_check, params, aux);
}

/* Exported events */

int execute_CandidateSelection_shouldrank(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' handling exported event 'shouldrank'\n");
#endif

    /* Export the event */
    return export_CandidateSelection_shouldrank(mon, params, aux);
}

int queue_CandidateSelection_shouldrank(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' queuing raised exported event 'shouldrank'\n");
#endif

    return push_event(&mon->event_queue, EVENT_CandidateSelection_shouldrank, params, aux);
}

int export_CandidateSelection_shouldrank(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_shouldrank != NULL) {
        return (mon->callback_shouldrank)(mon->identities, params, aux);
    }
    return 1;
}

int execute_CandidateSelection_result(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' handling exported event 'result'\n");
#endif

    /* Export the event */
    return export_CandidateSelection_result(mon, params, aux);
}

int queue_CandidateSelection_result(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' queuing raised exported event 'result'\n");
#endif

    return push_event(&mon->event_queue, EVENT_CandidateSelection_result, params, aux);
}

int export_CandidateSelection_result(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_result != NULL) {
        return (mon->callback_result)(mon->identities, params, aux);
    }
    return 1;
}

int execute_CandidateSelection_addP(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' handling exported event 'addP'\n");
#endif

    /* Export the event */
    return export_CandidateSelection_addP(mon, params, aux);
}

int queue_CandidateSelection_addP(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CandidateSelection' queuing raised exported event 'addP'\n");
#endif

    return push_event(&mon->event_queue, EVENT_CandidateSelection_addP, params, aux);
}

int export_CandidateSelection_addP(CandidateSelectionMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_addP != NULL) {
        return (mon->callback_addP)(mon->identities, params, aux);
    }
    return 1;
}

/* Monitor management functions */

/* Initialize a CandidateSelection monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_CandidateSelection_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CandidateSelectionMonitor * init_CandidateSelection_monitor(SMEDLValue *identities) {
    CandidateSelectionState init_state;

    /* Initialize the monitor with default state */
    if (!default_CandidateSelection_state(&init_state)) {
        /* malloc fail */
        return NULL;
    }
    CandidateSelectionMonitor *result = init_CandidateSelection_with_state(identities, &init_state);
    if (result == NULL) {
        /* malloc fail. Need to clean up strings and opaques in init_state. */
    }
    return result;
}

/* Fill the provided CandidateSelectionState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_CandidateSelection_state(CandidateSelectionState *state) {
    state->canNum = 0;
    return 1;
}

/* Initialize a CandidateSelection monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) default_CandidateSelection_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_CandidateSelection_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CandidateSelectionMonitor * init_CandidateSelection_with_state(SMEDLValue *identities, CandidateSelectionState *init_state) {
    CandidateSelectionMonitor *mon = malloc(sizeof(CandidateSelectionMonitor));
    if (mon == NULL) {
        return NULL;
    }

    /* Store the assigned identities */
    mon->identities = identities;

    /* Copy initial state vars in */
    mon->s = *init_state;

    /* Set all scenarios to their initial state */
    mon->sce_state = 0;
    mon->sce1_state = 0;
    mon->sce2_state = 0;

    /* Reset all scenario execution flags */
    memset(&mon->ef, 0, sizeof(mon->ef));

    /* Set all export callbacks to NULL */
    mon->callback_shouldrank = NULL;
    mon->callback_result = NULL;
    mon->callback_addP = NULL;

    /* Initialize event queue */
    mon->event_queue = (EventQueue){0};

    return mon;
}

/* Free a CandidateSelection monitor */
void free_CandidateSelection_monitor(CandidateSelectionMonitor *mon) {
    free(mon);
}

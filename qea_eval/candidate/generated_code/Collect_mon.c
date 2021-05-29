#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "smedl_types.h"
#include "event_queue.h"
#include "Collect_mon.h"

/* Callback registration functions - Set the export callback for an exported
 * event */

void register_Collect_result(CollectMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_result = cb_func;
}

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_Collect_monitor(). */
void registercleanup_Collect(CollectMonitor *mon, int (*cleanup_func)(CollectMonitor *mon)) {
    mon->cleanup = cleanup_func;
}

/* Queue processing function - Call the handlers for all the events in the
 * queue until it is empty.
 * Return nonzero if all handlers ran successfully, zero if not. */
static int handle_Collect_queue(CollectMonitor *mon) {
    int success = 1;
    int event;
    SMEDLValue *params;
    void *aux;

    while (pop_event(&mon->event_queue, &event, &params, &aux)) {
        switch (event) {
            case EVENT_Collect_result:
                success = success &&
                    execute_Collect_result(mon, params, aux);
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

int execute_Collect_addV(CollectMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Collect' handling imported event 'addV'\n");
#endif

    /* sce scenario */
    //if (!mon->ef.sce_flag) {
        switch (mon->sce_state) {
            case STATE_Collect_sce_start:
                if (1) {
                    mon->s.vNum++;

                    mon->sce_state = STATE_Collect_sce_start;
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
    return handle_Collect_queue(mon);
}

int execute_Collect_inRes(CollectMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Collect' handling imported event 'inRes'\n");
#endif

    /* sce1 scenario */
    //if (!mon->ef.sce1_flag) {
        switch (mon->sce1_state) {
            case STATE_Collect_sce1_init:
                if (mon->s.vNum > 1) {
                    mon->s.res = mon->s.res + params[0].v.i;
                    mon->s.vNumTemp = mon->s.vNum;
                    mon->s.vNum--;

                    mon->sce1_state = STATE_Collect_sce1_start;
                } else if (mon->s.vNum == 1) {
                    mon->s.res = params[0].v.i - mon->s.vNum;
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 1);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        new_params[0].t = SMEDL_INT;
                        new_params[0].v.i = mon->s.res;
                        queue_Collect_result(mon, new_params, aux);
                    }
                    mon->s.vNum = 0;

                    mon->sce1_state = STATE_Collect_sce1_start;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            case STATE_Collect_sce1_start:
                if (mon->s.vNum > 1) {
                    mon->s.res = mon->s.res + params[0].v.i;
                    mon->s.vNum--;

                    mon->sce1_state = STATE_Collect_sce1_start;
                } else if (mon->s.vNum == 1) {
                    mon->s.res = mon->s.res + params[0].v.i - mon->s.vNumTemp;
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 1);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        new_params[0].t = SMEDL_INT;
                        new_params[0].v.i = mon->s.res;
                        queue_Collect_result(mon, new_params, aux);
                    }
                    mon->s.vNum = 0;

                    mon->sce1_state = STATE_Collect_sce1_init;
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
    return handle_Collect_queue(mon);
}

/* Exported events */

int execute_Collect_result(CollectMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Collect' handling exported event 'result'\n");
#endif

    /* Export the event */
    return export_Collect_result(mon, params, aux);
}

int queue_Collect_result(CollectMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Collect' queuing raised exported event 'result'\n");
#endif

    return push_event(&mon->event_queue, EVENT_Collect_result, params, aux);
}

int export_Collect_result(CollectMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_result != NULL) {
        return (mon->callback_result)(mon->identities, params, aux);
    }
    return 1;
}

/* Monitor management functions */

/* Initialize a Collect monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_Collect_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CollectMonitor * init_Collect_monitor(SMEDLValue *identities) {
    CollectState init_state;

    /* Initialize the monitor with default state */
    if (!default_Collect_state(&init_state)) {
        /* malloc fail */
        return NULL;
    }
    CollectMonitor *result = init_Collect_with_state(identities, &init_state);
    if (result == NULL) {
        /* malloc fail. Need to clean up strings and opaques in init_state. */
    }
    return result;
}

/* Fill the provided CollectState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_Collect_state(CollectState *state) {
    state->vNum = 0;
    state->vNumTemp = 0;
    state->res = 0;
    return 1;
}

/* Initialize a Collect monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) default_Collect_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_Collect_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CollectMonitor * init_Collect_with_state(SMEDLValue *identities, CollectState *init_state) {
    CollectMonitor *mon = malloc(sizeof(CollectMonitor));
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

    /* Reset all scenario execution flags */
    memset(&mon->ef, 0, sizeof(mon->ef));

    /* Set all export callbacks to NULL */
    mon->callback_result = NULL;

    /* Initialize event queue */
    mon->event_queue = (EventQueue){0};

    return mon;
}

/* Free a Collect monitor */
void free_Collect_monitor(CollectMonitor *mon) {
    free(mon);
}

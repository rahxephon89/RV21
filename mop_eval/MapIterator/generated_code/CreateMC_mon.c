#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "smedl_types.h"
#include "event_queue.h"
#include "CreateMC_mon.h"

/* Callback registration functions - Set the export callback for an exported
 * event */

void register_CreateMC_new_mci(CreateMCMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_new_mci = cb_func;
}

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_CreateMC_monitor(). */
void registercleanup_CreateMC(CreateMCMonitor *mon, int (*cleanup_func)(CreateMCMonitor *mon)) {
    mon->cleanup = cleanup_func;
}

/* Queue processing function - Call the handlers for all the events in the
 * queue until it is empty.
 * Return nonzero if all handlers ran successfully, zero if not. */
static int handle_CreateMC_queue(CreateMCMonitor *mon) {
    int success = 1;
    int event;
    SMEDLValue *params;
    void *aux;

    while (pop_event(&mon->event_queue, &event, &params, &aux)) {
        switch (event) {
            case EVENT_CreateMC_new_mci:
                success = success &&
                    execute_CreateMC_new_mci(mon, params, aux);
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

int execute_CreateMC_new_mc(CreateMCMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CreateMC' handling imported event 'new_mc'\n");
#endif

    /* sce1 scenario */
    //if (!mon->ef.sce1_flag) {
        switch (mon->sce1_state) {
            case STATE_CreateMC_sce1_init:
                if (1) {

                    mon->sce1_state = STATE_CreateMC_sce1_start;
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
    return handle_CreateMC_queue(mon);
}

int execute_CreateMC_new_ci(CreateMCMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CreateMC' handling imported event 'new_ci'\n");
#endif

    /* sce1 scenario */
    //if (!mon->ef.sce1_flag) {
        switch (mon->sce1_state) {
            case STATE_CreateMC_sce1_start:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 1);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        new_params[0].t = SMEDL_POINTER;
                        new_params[0].v.p = params[0].v.p;
                        queue_CreateMC_new_mci(mon, new_params, aux);
                    }

                    mon->sce1_state = STATE_CreateMC_sce1_start;
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
    return handle_CreateMC_queue(mon);
}

/* Exported events */

int execute_CreateMC_new_mci(CreateMCMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CreateMC' handling exported event 'new_mci'\n");
#endif

    /* Export the event */
    return export_CreateMC_new_mci(mon, params, aux);
}

int queue_CreateMC_new_mci(CreateMCMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'CreateMC' queuing raised exported event 'new_mci'\n");
#endif

    return push_event(&mon->event_queue, EVENT_CreateMC_new_mci, params, aux);
}

int export_CreateMC_new_mci(CreateMCMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_new_mci != NULL) {
        return (mon->callback_new_mci)(mon->identities, params, aux);
    }
    return 1;
}

/* Monitor management functions */

/* Initialize a CreateMC monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_CreateMC_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CreateMCMonitor * init_CreateMC_monitor(SMEDLValue *identities) {
    CreateMCState init_state;

    /* Initialize the monitor with default state */
    if (!default_CreateMC_state(&init_state)) {
        /* malloc fail */
        return NULL;
    }
    CreateMCMonitor *result = init_CreateMC_with_state(identities, &init_state);
    if (result == NULL) {
        /* malloc fail. Need to clean up strings and opaques in init_state. */
    }
    return result;
}

/* Fill the provided CreateMCState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_CreateMC_state(CreateMCState *state) {
    return 1;
}

/* Initialize a CreateMC monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) default_CreateMC_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_CreateMC_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
CreateMCMonitor * init_CreateMC_with_state(SMEDLValue *identities, CreateMCState *init_state) {
    CreateMCMonitor *mon = malloc(sizeof(CreateMCMonitor));
    if (mon == NULL) {
        return NULL;
    }

    /* Store the assigned identities */
    mon->identities = identities;

    /* Copy initial state vars in */
    mon->s = *init_state;

    /* Set all scenarios to their initial state */
    mon->sce1_state = 0;

    /* Reset all scenario execution flags */
    memset(&mon->ef, 0, sizeof(mon->ef));

    /* Set all export callbacks to NULL */
    mon->callback_new_mci = NULL;

    /* Initialize event queue */
    mon->event_queue = (EventQueue){0};

    return mon;
}

/* Free a CreateMC monitor */
void free_CreateMC_monitor(CreateMCMonitor *mon) {
    free(mon);
}

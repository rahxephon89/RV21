#if DEBUG > 0
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "smedl_types.h"
#include "event_queue.h"
#include "Auctionmonitor_mon.h"

/* Callback registration functions - Set the export callback for an exported
 * event */

void register_Auctionmonitor_alarm_recreation(AuctionmonitorMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_alarm_recreation = cb_func;
}

void register_Auctionmonitor_alarm_low_bid(AuctionmonitorMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_alarm_low_bid = cb_func;
}

void register_Auctionmonitor_alarm_sold_early(AuctionmonitorMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_alarm_sold_early = cb_func;
}

void register_Auctionmonitor_alarm_not_sold(AuctionmonitorMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_alarm_not_sold = cb_func;
}

void register_Auctionmonitor_alarm_action_after_end(AuctionmonitorMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_alarm_action_after_end = cb_func;
}

void register_Auctionmonitor_alarm_action_before_start(AuctionmonitorMonitor *mon, SMEDLCallback cb_func) {
    mon->callback_alarm_action_before_start = cb_func;
}

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_Auctionmonitor_monitor(). */
void registercleanup_Auctionmonitor(AuctionmonitorMonitor *mon, int (*cleanup_func)(AuctionmonitorMonitor *mon)) {
    mon->cleanup = cleanup_func;
}

/* Queue processing function - Call the handlers for all the events in the
 * queue until it is empty.
 * Return nonzero if all handlers ran successfully, zero if not. */
static int handle_Auctionmonitor_queue(AuctionmonitorMonitor *mon) {
    int success = 1;
    int event;
    SMEDLValue *params;
    void *aux;

    while (pop_event(&mon->event_queue, &event, &params, &aux)) {
        switch (event) {
            case EVENT_Auctionmonitor_alarm_recreation:
                success = success &&
                    execute_Auctionmonitor_alarm_recreation(mon, params, aux);
                break;
            case EVENT_Auctionmonitor_alarm_low_bid:
                success = success &&
                    execute_Auctionmonitor_alarm_low_bid(mon, params, aux);
                break;
            case EVENT_Auctionmonitor_alarm_sold_early:
                success = success &&
                    execute_Auctionmonitor_alarm_sold_early(mon, params, aux);
                break;
            case EVENT_Auctionmonitor_alarm_not_sold:
                success = success &&
                    execute_Auctionmonitor_alarm_not_sold(mon, params, aux);
                break;
            case EVENT_Auctionmonitor_alarm_action_after_end:
                success = success &&
                    execute_Auctionmonitor_alarm_action_after_end(mon, params, aux);
                break;
            case EVENT_Auctionmonitor_alarm_action_before_start:
                success = success &&
                    execute_Auctionmonitor_alarm_action_before_start(mon, params, aux);
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

int execute_Auctionmonitor_create_auction(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling imported event 'create_auction'\n");
#endif

    /* main scenario */
    //if (!mon->ef.main_flag) {
        switch (mon->main_state) {
            case STATE_Auctionmonitor_main_init:
                if (1) {
                    mon->s.reserve_price = params[1].v.i;
                    mon->s.duration = params[2].v.i;

                    mon->main_state = STATE_Auctionmonitor_main_bidding;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            case STATE_Auctionmonitor_main_bidding:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 0);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        queue_Auctionmonitor_alarm_recreation(mon, new_params, aux);
                    }

                    mon->main_state = STATE_Auctionmonitor_main_error;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            case STATE_Auctionmonitor_main_above_reserve:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 0);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        queue_Auctionmonitor_alarm_recreation(mon, new_params, aux);
                    }

                    mon->main_state = STATE_Auctionmonitor_main_error;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            case STATE_Auctionmonitor_main_done:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 0);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        queue_Auctionmonitor_alarm_recreation(mon, new_params, aux);
                    }

                    mon->main_state = STATE_Auctionmonitor_main_error;
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
        //mon->ef.main_flag = 1;
    //}

    /* Finish the macro-step */
    return handle_Auctionmonitor_queue(mon);
}

int execute_Auctionmonitor_bid(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling imported event 'bid'\n");
#endif

    /* main scenario */
    //if (!mon->ef.main_flag) {
        switch (mon->main_state) {
            case STATE_Auctionmonitor_main_bidding:
                if (1) {
                    mon->s.current_price = params[1].v.i;

                    mon->main_state = STATE_Auctionmonitor_main_above_reserve;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            case STATE_Auctionmonitor_main_above_reserve:
                if (params[1].v.i > mon->s.current_price) {
                    mon->s.current_price = params[1].v.i;

                    mon->main_state = STATE_Auctionmonitor_main_above_reserve;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            case STATE_Auctionmonitor_main_done:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 0);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        queue_Auctionmonitor_alarm_action_after_end(mon, new_params, aux);
                    }

                    mon->main_state = STATE_Auctionmonitor_main_error;
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
        //mon->ef.main_flag = 1;
    //}

    /* Finish the macro-step */
    return handle_Auctionmonitor_queue(mon);
}

int execute_Auctionmonitor_sold(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling imported event 'sold'\n");
#endif

    /* main scenario */
    //if (!mon->ef.main_flag) {
        switch (mon->main_state) {
            case STATE_Auctionmonitor_main_bidding:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 0);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        queue_Auctionmonitor_alarm_sold_early(mon, new_params, aux);
                    }

                    mon->main_state = STATE_Auctionmonitor_main_error;
                } else {
                    /* XXX Do something here: Event matches but conditions
                     * not met, no else */
                }
                break;

            case STATE_Auctionmonitor_main_above_reserve:
                if (mon->s.current_price < mon->s.reserve_price) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 0);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        queue_Auctionmonitor_alarm_low_bid(mon, new_params, aux);
                    }

                    mon->main_state = STATE_Auctionmonitor_main_error;
                } else {

                    mon->main_state = STATE_Auctionmonitor_main_done;
                }
                break;

            case STATE_Auctionmonitor_main_done:
                if (1) {
                    {
                        SMEDLValue *new_params = malloc(sizeof(SMEDLValue) * 0);
                        if (new_params == NULL) {
                            /* malloc fail */
                            return 0;
                        }
                        queue_Auctionmonitor_alarm_action_after_end(mon, new_params, aux);
                    }

                    mon->main_state = STATE_Auctionmonitor_main_error;
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
        //mon->ef.main_flag = 1;
    //}

    /* Finish the macro-step */
    return handle_Auctionmonitor_queue(mon);
}

int execute_Auctionmonitor_end_of_day(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling imported event 'end_of_day'\n");
#endif

    /* main scenario */
    //if (!mon->ef.main_flag) {
        switch (mon->main_state) {
            case STATE_Auctionmonitor_main_bidding:
                if (mon->s.days_passed < mon->s.duration - 1) {
                    mon->s.days_passed++;

                    mon->main_state = STATE_Auctionmonitor_main_bidding;
                } else {

                    mon->main_state = STATE_Auctionmonitor_main_done;
                }
                break;

            case STATE_Auctionmonitor_main_above_reserve:
                if (mon->s.days_passed < mon->s.duration - 1) {
                    mon->s.days_passed++;

                    mon->main_state = STATE_Auctionmonitor_main_above_reserve;
                } else {

                    mon->main_state = STATE_Auctionmonitor_main_done;
                }
                break;

            case STATE_Auctionmonitor_main_done:
                if (1) {

                    mon->main_state = STATE_Auctionmonitor_main_done;
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
        //mon->ef.main_flag = 1;
    //}

    /* Finish the macro-step */
    return handle_Auctionmonitor_queue(mon);
}

/* Exported events */

int execute_Auctionmonitor_alarm_recreation(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling exported event 'alarm_recreation'\n");
#endif

    /* Export the event */
    return export_Auctionmonitor_alarm_recreation(mon, params, aux);
}

int queue_Auctionmonitor_alarm_recreation(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' queuing raised exported event 'alarm_recreation'\n");
#endif

    return push_event(&mon->event_queue, EVENT_Auctionmonitor_alarm_recreation, params, aux);
}

int export_Auctionmonitor_alarm_recreation(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_alarm_recreation != NULL) {
        return (mon->callback_alarm_recreation)(mon->identities, params, aux);
    }
    return 1;
}

int execute_Auctionmonitor_alarm_low_bid(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling exported event 'alarm_low_bid'\n");
#endif

    /* Export the event */
    return export_Auctionmonitor_alarm_low_bid(mon, params, aux);
}

int queue_Auctionmonitor_alarm_low_bid(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' queuing raised exported event 'alarm_low_bid'\n");
#endif

    return push_event(&mon->event_queue, EVENT_Auctionmonitor_alarm_low_bid, params, aux);
}

int export_Auctionmonitor_alarm_low_bid(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_alarm_low_bid != NULL) {
        return (mon->callback_alarm_low_bid)(mon->identities, params, aux);
    }
    return 1;
}

int execute_Auctionmonitor_alarm_sold_early(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling exported event 'alarm_sold_early'\n");
#endif

    /* Export the event */
    return export_Auctionmonitor_alarm_sold_early(mon, params, aux);
}

int queue_Auctionmonitor_alarm_sold_early(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' queuing raised exported event 'alarm_sold_early'\n");
#endif

    return push_event(&mon->event_queue, EVENT_Auctionmonitor_alarm_sold_early, params, aux);
}

int export_Auctionmonitor_alarm_sold_early(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_alarm_sold_early != NULL) {
        return (mon->callback_alarm_sold_early)(mon->identities, params, aux);
    }
    return 1;
}

int execute_Auctionmonitor_alarm_not_sold(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling exported event 'alarm_not_sold'\n");
#endif

    /* Export the event */
    return export_Auctionmonitor_alarm_not_sold(mon, params, aux);
}

int queue_Auctionmonitor_alarm_not_sold(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' queuing raised exported event 'alarm_not_sold'\n");
#endif

    return push_event(&mon->event_queue, EVENT_Auctionmonitor_alarm_not_sold, params, aux);
}

int export_Auctionmonitor_alarm_not_sold(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_alarm_not_sold != NULL) {
        return (mon->callback_alarm_not_sold)(mon->identities, params, aux);
    }
    return 1;
}

int execute_Auctionmonitor_alarm_action_after_end(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling exported event 'alarm_action_after_end'\n");
#endif

    /* Export the event */
    return export_Auctionmonitor_alarm_action_after_end(mon, params, aux);
}

int queue_Auctionmonitor_alarm_action_after_end(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' queuing raised exported event 'alarm_action_after_end'\n");
#endif

    return push_event(&mon->event_queue, EVENT_Auctionmonitor_alarm_action_after_end, params, aux);
}

int export_Auctionmonitor_alarm_action_after_end(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_alarm_action_after_end != NULL) {
        return (mon->callback_alarm_action_after_end)(mon->identities, params, aux);
    }
    return 1;
}

int execute_Auctionmonitor_alarm_action_before_start(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' handling exported event 'alarm_action_before_start'\n");
#endif

    /* Export the event */
    return export_Auctionmonitor_alarm_action_before_start(mon, params, aux);
}

int queue_Auctionmonitor_alarm_action_before_start(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
#if DEBUG >= 4
    fprintf(stderr, "Monitor 'Auctionmonitor' queuing raised exported event 'alarm_action_before_start'\n");
#endif

    return push_event(&mon->event_queue, EVENT_Auctionmonitor_alarm_action_before_start, params, aux);
}

int export_Auctionmonitor_alarm_action_before_start(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux) {
    if (mon->callback_alarm_action_before_start != NULL) {
        return (mon->callback_alarm_action_before_start)(mon->identities, params, aux);
    }
    return 1;
}

/* Monitor management functions */

/* Initialize a Auctionmonitor monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_Auctionmonitor_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
AuctionmonitorMonitor * init_Auctionmonitor_monitor(SMEDLValue *identities) {
    AuctionmonitorState init_state;

    /* Initialize the monitor with default state */
    if (!default_Auctionmonitor_state(&init_state)) {
        /* malloc fail */
        return NULL;
    }
    AuctionmonitorMonitor *result = init_Auctionmonitor_with_state(identities, &init_state);
    if (result == NULL) {
        /* malloc fail. Need to clean up strings and opaques in init_state. */
    }
    return result;
}

/* Fill the provided AuctionmonitorState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_Auctionmonitor_state(AuctionmonitorState *state) {
    state->reserve_price = 0.0;
    state->current_price = 0;
    state->duration = 0.0;
    state->days_passed = 0;
    return 1;
}

/* Initialize a Auctionmonitor monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) default_Auctionmonitor_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_Auctionmonitor_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
AuctionmonitorMonitor * init_Auctionmonitor_with_state(SMEDLValue *identities, AuctionmonitorState *init_state) {
    AuctionmonitorMonitor *mon = malloc(sizeof(AuctionmonitorMonitor));
    if (mon == NULL) {
        return NULL;
    }

    /* Store the assigned identities */
    mon->identities = identities;

    /* Copy initial state vars in */
    mon->s = *init_state;

    /* Set all scenarios to their initial state */
    mon->main_state = 0;

    /* Reset all scenario execution flags */
    memset(&mon->ef, 0, sizeof(mon->ef));

    /* Set all export callbacks to NULL */
    mon->callback_alarm_recreation = NULL;
    mon->callback_alarm_low_bid = NULL;
    mon->callback_alarm_sold_early = NULL;
    mon->callback_alarm_not_sold = NULL;
    mon->callback_alarm_action_after_end = NULL;
    mon->callback_alarm_action_before_start = NULL;

    /* Initialize event queue */
    mon->event_queue = (EventQueue){0};

    return mon;
}

/* Free a Auctionmonitor monitor */
void free_Auctionmonitor_monitor(AuctionmonitorMonitor *mon) {
    free(mon);
}

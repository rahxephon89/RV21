#ifndef Auctionmonitor_MON_H
#define Auctionmonitor_MON_H

#include "smedl_types.h"
#include "event_queue.h"

/* Internal/exported event enum for action queues */
typedef enum {
    EVENT_Auctionmonitor_alarm_recreation,
    EVENT_Auctionmonitor_alarm_low_bid,
    EVENT_Auctionmonitor_alarm_sold_early,
    EVENT_Auctionmonitor_alarm_not_sold,
    EVENT_Auctionmonitor_alarm_action_after_end,
    EVENT_Auctionmonitor_alarm_action_before_start,
} AuctionmonitorEvent;

/* Scenario state enums */
typedef enum {
    STATE_Auctionmonitor_main_init,
    STATE_Auctionmonitor_main_bidding,
    STATE_Auctionmonitor_main_above_reserve,
    STATE_Auctionmonitor_main_error,
    STATE_Auctionmonitor_main_done,
} Auctionmonitor_main_State;

/* State variables for Auctionmonitor.
 * Used for initialization as well as in the AuctionmonitorMonitor
 * struct. */
typedef struct AuctionmonitorState {
    double reserve_price;
    double current_price;
    double duration;
    double days_passed;
} AuctionmonitorState;

/* Auctionmonitor monitor struct.
 * Maintains all of the internal state for the monitor. */
typedef struct AuctionmonitorMonitor {
    /* Array of monitor's identities */
    SMEDLValue *identities;

    /* Scenario states */
    Auctionmonitor_main_State main_state;

    /* Scenario execution flags (ensures each scenario only processes one event
     * per macro-step) */
    struct {
        unsigned int main_flag : 1;
    } ef;

    /* State variables */
    AuctionmonitorState s;

    /* Exported event callback pointers */
    SMEDLCallback callback_alarm_recreation;
    SMEDLCallback callback_alarm_low_bid;
    SMEDLCallback callback_alarm_sold_early;
    SMEDLCallback callback_alarm_not_sold;
    SMEDLCallback callback_alarm_action_after_end;
    SMEDLCallback callback_alarm_action_before_start;

    /* Cleanup callback pointer */
    int (*cleanup)(struct AuctionmonitorMonitor *mon);

    /* Local event queue */
    EventQueue event_queue;

    //TODO mutex?
} AuctionmonitorMonitor;

/* Callback registration functions - Set the export callback for an exported
 * event. Set to NULL to unregister a callback. */
void register_Auctionmonitor_alarm_recreation(AuctionmonitorMonitor *mon, SMEDLCallback cb_func);
void register_Auctionmonitor_alarm_low_bid(AuctionmonitorMonitor *mon, SMEDLCallback cb_func);
void register_Auctionmonitor_alarm_sold_early(AuctionmonitorMonitor *mon, SMEDLCallback cb_func);
void register_Auctionmonitor_alarm_not_sold(AuctionmonitorMonitor *mon, SMEDLCallback cb_func);
void register_Auctionmonitor_alarm_action_after_end(AuctionmonitorMonitor *mon, SMEDLCallback cb_func);
void register_Auctionmonitor_alarm_action_before_start(AuctionmonitorMonitor *mon, SMEDLCallback cb_func);

/* Cleanup callback registration function - Set the callback for when the
 * monitor is ready to be recycled. The callback is responsible for calling
 * free_Auctionmonitor_monitor(). It must accept the monitor pointer as a
 * parameter and return nonzero on success, zero on failure. */
void registercleanup_Auctionmonitor(AuctionmonitorMonitor *mon, int (*cleanup_func)(AuctionmonitorMonitor *mon));

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
int execute_Auctionmonitor_create_auction(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int execute_Auctionmonitor_bid(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int execute_Auctionmonitor_sold(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int execute_Auctionmonitor_end_of_day(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int execute_Auctionmonitor_alarm_recreation(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int queue_Auctionmonitor_alarm_recreation(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int export_Auctionmonitor_alarm_recreation(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int execute_Auctionmonitor_alarm_low_bid(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int queue_Auctionmonitor_alarm_low_bid(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int export_Auctionmonitor_alarm_low_bid(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int execute_Auctionmonitor_alarm_sold_early(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int queue_Auctionmonitor_alarm_sold_early(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int export_Auctionmonitor_alarm_sold_early(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int execute_Auctionmonitor_alarm_not_sold(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int queue_Auctionmonitor_alarm_not_sold(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int export_Auctionmonitor_alarm_not_sold(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int execute_Auctionmonitor_alarm_action_after_end(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int queue_Auctionmonitor_alarm_action_after_end(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int export_Auctionmonitor_alarm_action_after_end(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int execute_Auctionmonitor_alarm_action_before_start(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int queue_Auctionmonitor_alarm_action_before_start(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);
int export_Auctionmonitor_alarm_action_before_start(AuctionmonitorMonitor *mon, SMEDLValue *params, void *aux);

/* Monitor management functions */

/* Initialize a Auctionmonitor monitor with default state.
 * Return a pointer to the monitor. Must be freed with
 * free_Auctionmonitor_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
AuctionmonitorMonitor * init_Auctionmonitor_monitor(SMEDLValue *identities);

/* Fill the provided AuctionmonitorState
 * with the default initial values for the monitor. Note that strings and
 * opaque data must be free()'d if they are reassigned! The following two
 * functions from smedl_types.h make that simple:
 * - smedl_replace_string()
 * - smedl_replace_opaque()
 * Returns nonzero on success, zero on malloc failure. */
int default_Auctionmonitor_state(AuctionmonitorState *state);

/* Initialize a Auctionmonitor monitor with the provided state. Note that this
 * function takes ownership of the memory used by any strings and opaques when
 * successful! (That is, it will call free() on them when they are no longer
 * needed.) defualt_Auctionmonitor_state() is aware of this, so unless changing
 * initial string or opaque state, there is no need to be concerned about this.
 *
 * Return a pointer to the monitor. Must be freed with
 * free_Auctionmonitor_monitor() when no longer needed.
 * Returns NULL on malloc failure. */
AuctionmonitorMonitor * init_Auctionmonitor_with_state(SMEDLValue *identities, AuctionmonitorState *init_state);

/* Free a Auctionmonitor monitor. NOTE: Does not free the identities. That must
 * be done by the caller, if necessary. */
void free_Auctionmonitor_monitor(AuctionmonitorMonitor *mon);

#endif /* Auctionmonitor_MON_H */

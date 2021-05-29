#ifndef Auctionmonitor_GLOBAL_WRAPPER_H
#define Auctionmonitor_GLOBAL_WRAPPER_H

#include "smedl_types.h"

/******************************************************************************
 * External Interface                                                         *
 ******************************************************************************/

/* Initialization interface - Initialize the global wrapper. Must be called once
 * before importing any events. Return nonzero on success, zero on failure. */
int init_Auctionmonitor_syncset();

/* Cleanup interface - Tear down and free the resources used by this global
 * wrapper and all the local wrappers and monitors it manages. */
void free_Auctionmonitor_syncset();

/* Global wrapper export interfaces - Called by monitors to place exported
 * events into the appropriate export queues, where they will later be routed to
 * the proper destinations inside and outside the synchronous set.
 * Returns nonzero on success, zero on failure.
 *
 * Parameters:
 * identites - An array of SMEDLValue of the proper length for the exporting
 *   monitor
 * params - An array of SMEDLValue, one for each parameter of the exported event
 * aux - Extra data that was passed from the imported event that caused this
 *   exported event
 */
int raise_Auctionmonitor_alarm_recreation(SMEDLValue *identities, SMEDLValue *params, void *aux);
int raise_Auctionmonitor_alarm_low_bid(SMEDLValue *identities, SMEDLValue *params, void *aux);
int raise_Auctionmonitor_alarm_sold_early(SMEDLValue *identities, SMEDLValue *params, void *aux);
int raise_Auctionmonitor_alarm_not_sold(SMEDLValue *identities, SMEDLValue *params, void *aux);
int raise_Auctionmonitor_alarm_action_after_end(SMEDLValue *identities, SMEDLValue *params, void *aux);
int raise_Auctionmonitor_alarm_action_before_start(SMEDLValue *identities, SMEDLValue *params, void *aux);

/* Global wrapper import interface - Called by the environment (other
 * synchronous sets, the target system) to import events into this global
 * wrapper. Each connection that this synchronous set receives has a separate
 * function.
 * Returns nonzero on success, zero on failure.
 *
 * Parameters:
 * identities - An array of the source monitor's identities. If the connection
 *   is from the target system, this parameter is ignored and can safely be set
 *   to NULL.
 * params - An array of the source event's parameters
 * aux - Extra data to be passed through unchanged */
int import_Auctionmonitor_ch1(SMEDLValue *identities, SMEDLValue *params, void *aux);
int import_Auctionmonitor_ch2(SMEDLValue *identities, SMEDLValue *params, void *aux);
int import_Auctionmonitor_ch3(SMEDLValue *identities, SMEDLValue *params, void *aux);
int import_Auctionmonitor_ch4(SMEDLValue *identities, SMEDLValue *params, void *aux);

/* Global wrapper callback interface - Used to register callback functions to be
 * called by this global wrapper when it has an event to export to the
 * environment (other synchronous sets, the target system).
 *
 * Parameters:
 * cb_func - A function pointer for the callback to register, or NULL to
 *   unregister a callback. Must accept three parameters: An array of SMEDLValue
 *   for the source monitor's identities (or NULL if the source monitor has
 *   none), another array of SMEDLValue for the source event's parameters, and
 *   a void * for passthrough data. */
void callback_Auctionmonitor_Auctionmonitor_alarm_recreation(SMEDLCallback cb_func);
void callback_Auctionmonitor_Auctionmonitor_alarm_low_bid(SMEDLCallback cb_func);
void callback_Auctionmonitor_Auctionmonitor_alarm_sold_early(SMEDLCallback cb_func);
void callback_Auctionmonitor_Auctionmonitor_alarm_not_sold(SMEDLCallback cb_func);
void callback_Auctionmonitor_Auctionmonitor_alarm_action_after_end(SMEDLCallback cb_func);
void callback_Auctionmonitor_Auctionmonitor_alarm_action_before_start(SMEDLCallback cb_func);

/******************************************************************************
 * End of External Interface                                                  *
 ******************************************************************************/

typedef enum {
    CHANNEL_Auctionmonitor_Auctionmonitor_alarm_recreation,
    CHANNEL_Auctionmonitor_Auctionmonitor_alarm_low_bid,
    CHANNEL_Auctionmonitor_Auctionmonitor_alarm_sold_early,
    CHANNEL_Auctionmonitor_Auctionmonitor_alarm_not_sold,
    CHANNEL_Auctionmonitor_Auctionmonitor_alarm_action_after_end,
    CHANNEL_Auctionmonitor_Auctionmonitor_alarm_action_before_start,
} AuctionmonitorChannelID;

/* Intra routing functions - Called by import interface functions and intra
 * queue processing function to route events to the local wrappers.
 * Return nonzero on success, zero on failure. */
int route_Auctionmonitor_ch1(SMEDLValue *identities, SMEDLValue *params, void *aux);
int route_Auctionmonitor_ch2(SMEDLValue *identities, SMEDLValue *params, void *aux);
int route_Auctionmonitor_ch3(SMEDLValue *identities, SMEDLValue *params, void *aux);
int route_Auctionmonitor_ch4(SMEDLValue *identities, SMEDLValue *params, void *aux);

#endif /* Auctionmonitor_GLOBAL_WRAPPER_H */

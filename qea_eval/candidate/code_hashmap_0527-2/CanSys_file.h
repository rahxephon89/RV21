#ifndef CanSys_FILE_H
#define CanSys_FILE_H

#include "file.h"

/* Current message format version. Increment the major version whenever making
 * a backward-incompatible change to the message format. Increment the minor
 * version whenever making a backward-compatible update to the message format.
 */
#define FMT_VERSION_MAJOR 2
#define FMT_VERSION_MINOR 0

/* Sample message formats:
 * {
 *     "fmt_version": [2, 0],
 *     "channel": "<channel name>",
 *     "event": "<event>",
 *     "params": [2.5, "some_other_string", 6],
 *     "aux": {
 *         "arbitrary": "json data"
 *      }
 * }
 *
 * {
 *     "fmt_version": [2, 0],
 *     "channel": "<channel name>",
 *     "event": "<monitor>.<event>",
 *     "identities": [1, "some_string"],
 *     "params": [2.5, "some_other_string", 6],
 *     "aux": {
 *         "arbitrary": "json data"
 *      }
 *  }
 */

/* System-wide channel enum */
typedef enum {
    SYSCHANNEL_ch1,
    SYSCHANNEL_ch2,
    SYSCHANNEL_ch3,
    SYSCHANNEL_ch7,
    SYSCHANNEL_ch6,
    SYSCHANNEL_ch8,
    SYSCHANNEL_ch9,
    SYSCHANNEL_ch5,
    SYSCHANNEL_ch10,
    SYSCHANNEL_ch11,
    SYSCHANNEL_Collect_result,
} ChannelID;

/* Queue processing function - Pop events off the queue and send them to the
 * proper synchronous sets (or to be written to the output file) until the
 * queue is empty */
int handle_queue();

/* "Callbacks" (not used as such) for events read from the input file and
 * callbacks for events exported from global wrappers.
 * Return nonzero on success, zero on failure. */
int enqueue_ch1(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_ch2(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_ch3(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_ch7(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_ch6(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_ch8(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_ch9(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_ch5(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_ch10(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_ch11(SMEDLValue *identities, SMEDLValue *params, void *aux);
int enqueue_Collect_result(SMEDLValue *identities, SMEDLValue *params, void *aux);

/* Output functions for events that are "sent back to the target system."
 * Return nonzero on success, zero on failure. */
int write_Collect_result(SMEDLValue *identities, SMEDLValue *params, void *aux);

/* Initialize the global wrappers and register callback functions with them.
 * Return nonzero on success, zero on failure. */
int init_global_wrappers();

/* Cleanup the global wrappers and the local wrappers and monitors within */
void free_global_wrappers();

/* Receive and process events from the provided JSON parser. Any malformed
 * events are skipped (with a warning printed to stderr). */
void read_events(JSONParser *parser);

/* Verify the fmt_version and retrieve the other necessary components
 * (channel, params, aux). Return nonzero if successful, zero if something is
 * missing or incorrect.
 * Note that aux is not required and will be set to NULL if not given.*/
int get_json_components(const char *str, jsmntok_t *msg,
        jsmntok_t **channel, jsmntok_t **params, jsmntok_t **aux);

#endif /* CanSys_FILE_H */

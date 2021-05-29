#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include "global_event_queue.h"
#include "file.h"
#include "json.h"
#include "CreateVec_global_wrapper.h"
#include "Unsafe_file.h"

static GlobalEventQueue queue = {0};

/* Queue processing function - Pop events off the queue and send them to the
 * proper synchronous sets (or to be written to the output file) until the
 * queue is empty */
int handle_queue() {
    int success = 1;
    int channel;
    SMEDLValue *identities, *params;
    void *aux;

    while (pop_global_event(&queue, &channel, &identities, &params, &aux)) {
        switch (channel) {
            case SYSCHANNEL_ch1:
                success = import_CreateVec_ch1(identities, params, aux) && success;
                break;
            case SYSCHANNEL_ch2:
                success = import_CreateVec_ch2(identities, params, aux) && success;
                break;
            case SYSCHANNEL_ch3:
                success = import_CreateVec_ch3(identities, params, aux) && success;
                break;
            case SYSCHANNEL_ch4:
                success = import_CreateVec_ch4(identities, params, aux) && success;
                break;
            case SYSCHANNEL_CreateVec_violation:
                success = write_CreateVec_violation(identities, params, aux) && success;
                break;
        }
        /* Event params and identities were malloc'd in the enqueue_*()
         * functions. They are no longer needed. (String and opaque data were
         * already free'd in the switch.) */
        free(identities);
        free(params);
    }

    return success;
}

/* "Callbacks" (not used as such) for events read from the input file and
 * callbacks for events exported from global wrappers.
 * Return nonzero on success, zero on failure. */

int enqueue_ch1(SMEDLValue *identities, SMEDLValue *params,
        void *aux) {
    SMEDLValue *ids_copy = NULL;
    SMEDLValue *params_copy = smedl_copy_array(params, 1);
    if (!push_global_event(&queue, SYSCHANNEL_ch1, ids_copy, params_copy, aux)) {
        /* malloc fail */
        smedl_free_array(params_copy, 1);
        return 0;
    }
    return 1;
}

int enqueue_ch2(SMEDLValue *identities, SMEDLValue *params,
        void *aux) {
    SMEDLValue *ids_copy = NULL;
    SMEDLValue *params_copy = smedl_copy_array(params, 1);
    if (!push_global_event(&queue, SYSCHANNEL_ch2, ids_copy, params_copy, aux)) {
        /* malloc fail */
        smedl_free_array(params_copy, 1);
        return 0;
    }
    return 1;
}

int enqueue_ch3(SMEDLValue *identities, SMEDLValue *params,
        void *aux) {
    SMEDLValue *ids_copy = NULL;
    SMEDLValue *params_copy = smedl_copy_array(params, 1);
    if (!push_global_event(&queue, SYSCHANNEL_ch3, ids_copy, params_copy, aux)) {
        /* malloc fail */
        smedl_free_array(params_copy, 1);
        return 0;
    }
    return 1;
}

int enqueue_ch4(SMEDLValue *identities, SMEDLValue *params,
        void *aux) {
    SMEDLValue *ids_copy = NULL;
    SMEDLValue *params_copy = smedl_copy_array(params, 1);
    if (!push_global_event(&queue, SYSCHANNEL_ch4, ids_copy, params_copy, aux)) {
        /* malloc fail */
        smedl_free_array(params_copy, 1);
        return 0;
    }
    return 1;
}

int enqueue_CreateVec_violation(SMEDLValue *identities, SMEDLValue *params,
        void *aux) {
    SMEDLValue *ids_copy = smedl_copy_array(identities, 1);
    SMEDLValue *params_copy = smedl_copy_array(params, 0);
    if (!push_global_event(&queue, SYSCHANNEL_CreateVec_violation, ids_copy, params_copy, aux)) {
        /* malloc fail */
        smedl_free_array(params_copy, 0);
        return 0;
    }
    return 1;
}

/* Output functions for events that are "sent back to the target system."
 * Return nonzero on success, zero on failure. */

int write_CreateVec_violation(SMEDLValue *identities, SMEDLValue *params, void *aux) {
    /*printf("{\n"
        "\t\"fmt_version\": [%d, %d],\n"
        "\t\"channel\": \"CreateVec_violation\",\n"
        "\t\"event\": \"CreateVec.violation\",\n"
        "\t\"identities\": [",
        FMT_VERSION_MAJOR, FMT_VERSION_MINOR);
    printf("%d", identities[0].v.i);
    printf("],\n"
        "\t\"params\": [");
    AuxData *aux_data = aux;
    printf("],\n"
        "\t\"aux\": %.*s\n", (int) aux_data->len, aux_data->data);
    printf("}\n");
    */
    printf("violation\n");
    return 1;
}

/* Verify the fmt_version and retrieve the other necessary components
 * (channel, params, aux). Return nonzero if successful, zero if something is
 * missing or incorrect.
 * Note that aux is not required and will be set to NULL if not given. */
int get_json_components(const char *str, jsmntok_t *msg,
        jsmntok_t **channel, jsmntok_t **params, jsmntok_t **aux) {
    /* Verify fmt_version */
    int version;
    jsmntok_t *fmt_version = json_lookup(str, msg, "fmt_version");
    if (fmt_version == NULL ||
            fmt_version->type != JSMN_ARRAY ||
            fmt_version->size != 2) {
#if DEBUG >= 2
        err("fmt_version not present, not array, or not size 2");
#endif
        return 0;
    }
    if (!json_to_int(str, fmt_version + 1, &version) ||
            version != FMT_VERSION_MAJOR) {
#if DEBUG >= 2
        err("Major fmt_version does not match");
#endif
        return 0;
    }
    if (!json_to_int(str, fmt_version + 2, &version) ||
            version < FMT_VERSION_MINOR) {
#if DEBUG >= 2
        err("Minor fmt_version too low");
#endif
        return 0;
    }

    /* Extract other components */
    *channel = json_lookup(NULL, NULL, "channel");
    if (*channel == NULL || (*channel)->type != JSMN_STRING) {
#if DEBUG >= 2
        err("channel not present or not a string");
#endif
        return 0;
    }
    *params = json_lookup(NULL, NULL, "params");
    if (*params == NULL || (*params)->type != JSMN_ARRAY) {
#if DEBUG >= 2
        err("params not present or not an array");
#endif
        return 0;
    }
    *aux = json_lookup(NULL, NULL, "aux");

    return 1;
}

/* Receive and process events from the provided JSON parser. Any malformed
 * events are skipped (with a warning printed to stderr). */
void read_events(JSONParser *parser) {
    jsmntok_t *msg;
    char *str;

    for (msg = next_message(parser, &str);
            msg != NULL;
            msg = next_message(parser, &str)) {
        /* Get components from JSON */
        jsmntok_t *chan_tok, *params_tok, *aux_tok;
        if (!get_json_components(str, msg, &chan_tok, &params_tok, &aux_tok)) {
            err("\nWarning: Message %d has incorrect format or incompatible "
                    "fmt_version\n",
                    parser->msg_count);
            continue;
        }

        /* Create aux struct */
        AuxData aux;
        aux.data = str + aux_tok->start;
        aux.len = aux_tok->end - aux_tok->start;
        if (aux_tok->type == JSMN_STRING) {
            aux.data--;
            aux.len += 2;
        }

        /* Import the event */
        char *chan;
        size_t chan_len;
        int ch_result = json_to_string_len(str, chan_tok, &chan, &chan_len);
        if (!ch_result) {
            err("\nStopping: Out of memory.");
            break;
        }
        if (chan_len == strlen("ch1") &&
                !strncmp(chan, "ch1", chan_len)) {
            /* Check param array length */
            if (params_tok->size < 1) {
            }
            /* Convert params to SMEDLValue array */
            int tmp_i;
            char *tmp_s;
            SMEDLValue params[1];
            params_tok++;
            if (json_to_int(str, params_tok, &params[0].v.i)) {
                params[0].t = SMEDL_INT;
            } else {
                err("\nWarning: Skipping message %d: Bad format, overflow, or "
                        "out-of-memory\n", parser->msg_count);
                smedl_free_array_contents(params, 0);
                continue;
            }

            /* Process the event */
            int result = enqueue_ch1(NULL, params, &aux);
            smedl_free_array_contents(params, 1);
            if (result) {
                if (!handle_queue()) {
                    err("\nWarning: Problem processing queue after message %d",
                            parser->msg_count);
                }
            } else {
                err("\nWarning: Skipping message %d: "
                        "enqueue_ch1() failed\n",
                        parser->msg_count);
            }
        } else if (chan_len == strlen("ch2") &&
                !strncmp(chan, "ch2", chan_len)) {
            /* Check param array length */
            if (params_tok->size < 1) {
            }
            /* Convert params to SMEDLValue array */
            int tmp_i;
            char *tmp_s;
            SMEDLValue params[1];
            params_tok++;
            if (json_to_int(str, params_tok, &params[0].v.i)) {
                params[0].t = SMEDL_INT;
            } else {
                err("\nWarning: Skipping message %d: Bad format, overflow, or "
                        "out-of-memory\n", parser->msg_count);
                smedl_free_array_contents(params, 0);
                continue;
            }

            /* Process the event */
            int result = enqueue_ch2(NULL, params, &aux);
            smedl_free_array_contents(params, 1);
            if (result) {
                if (!handle_queue()) {
                    err("\nWarning: Problem processing queue after message %d",
                            parser->msg_count);
                }
            } else {
                err("\nWarning: Skipping message %d: "
                        "enqueue_ch2() failed\n",
                        parser->msg_count);
            }
        } else if (chan_len == strlen("ch3") &&
                !strncmp(chan, "ch3", chan_len)) {
            /* Check param array length */
            if (params_tok->size < 1) {
            }
            /* Convert params to SMEDLValue array */
            int tmp_i;
            char *tmp_s;
            SMEDLValue params[1];
            params_tok++;
            if (json_to_int(str, params_tok, &params[0].v.i)) {
                params[0].t = SMEDL_INT;
            } else {
                err("\nWarning: Skipping message %d: Bad format, overflow, or "
                        "out-of-memory\n", parser->msg_count);
                smedl_free_array_contents(params, 0);
                continue;
            }

            /* Process the event */
            int result = enqueue_ch3(NULL, params, &aux);
            smedl_free_array_contents(params, 1);
            if (result) {
                if (!handle_queue()) {
                    err("\nWarning: Problem processing queue after message %d",
                            parser->msg_count);
                }
            } else {
                err("\nWarning: Skipping message %d: "
                        "enqueue_ch3() failed\n",
                        parser->msg_count);
            }
        } else if (chan_len == strlen("ch4") &&
                !strncmp(chan, "ch4", chan_len)) {
            /* Check param array length */
            if (params_tok->size < 1) {
            }
            /* Convert params to SMEDLValue array */
            int tmp_i;
            char *tmp_s;
            SMEDLValue params[1];
            params_tok++;
            if (json_to_int(str, params_tok, &params[0].v.i)) {
                params[0].t = SMEDL_INT;
            } else {
                err("\nWarning: Skipping message %d: Bad format, overflow, or "
                        "out-of-memory\n", parser->msg_count);
                smedl_free_array_contents(params, 0);
                continue;
            }

            /* Process the event */
            int result = enqueue_ch4(NULL, params, &aux);
            smedl_free_array_contents(params, 1);
            if (result) {
                if (!handle_queue()) {
                    err("\nWarning: Problem processing queue after message %d",
                            parser->msg_count);
                }
            } else {
                err("\nWarning: Skipping message %d: "
                        "enqueue_ch4() failed\n",
                        parser->msg_count);
            }
        }
        if (ch_result < 0) {
            free(chan);
        }
    }

    if (parser->status == JSONSTATUS_READERR) {
        err("\nStopping: Read error.");
    } else if (parser->status == JSONSTATUS_INVALID) {
        err("\nStopping: Encountered malformed message.");
    } else if (parser->status == JSONSTATUS_NOMEM) {
        err("\nStopping: Out of memory.");
    } else if (parser->status == JSONSTATUS_EOF) {
        err("\nFinished.");
    }
    err("Processed %d messages.", parser->msg_count);
}

/* Initialize the global wrappers and register callback functions with them.
 * Return nonzero on success, zero on failure. */
int init_global_wrappers() {
    /* CreateVec syncset */
    if (!init_CreateVec_syncset()) {
        goto fail_init_CreateVec;
    }
    callback_CreateVec_CreateVec_violation(enqueue_CreateVec_violation);

    return 1;

fail_init_CreateVec:
    return 0;
}

/* Cleanup the global wrappers and the local wrappers and monitors within */
void free_global_wrappers() {
    free_CreateVec_syncset();
}

/* Print a help message to stderr */
static void usage(const char *name) {
    err("Usage: %s [--] [input.json]", name);
    err("Read messages from the provided input file (or stdin if not provided) "
            "and print\nthe messages emitted back to the environment");
}


void call_monitor(int parameter, int type){
  SMEDLValue params[1];

  params[0].t = SMEDL_INT;
  params[0].v.i = parameter;


  switch(type){
   case 1: enqueue_ch1(NULL, params, NULL); break;
   case 2: enqueue_ch2(NULL, params, NULL); break;
   case 3: enqueue_ch3(NULL, params, NULL); break;
   case 4:  enqueue_ch4(NULL, params, NULL); break;
   default: break;
  }
  handle_queue();
}



int main1(int argc, char **argv) {
    /* Check for a file name argument */
    const char *fname = NULL;
    if (argc >= 2) {
        if (!strcmp(argv[1], "--help")) {
            usage(argv[0]);
            return 0;
        } else if (!strcmp(argv[1], "--")) {
            if (argc == 3) {
                fname = argv[2];
            } else {
                usage(argv[0]);
                return 1;
            }
        }
    }


    /* Initialize global wrappers */
    int result = init_global_wrappers();
    if (!result) {
        err("Could not initialize global wrappers");
        return 1;
    }

    /* Initialize the parser */
    JSONParser parser;
    result = init_parser(&parser, fname);
    if (!result) {
        err("Could not initialize JSON parser");
        return 1;
    }

    /* Start handling events */
    read_events(&parser);

    /* Cleanup the parser */
    result = free_parser(&parser);
    if (!result) {
        err("Could not clean up JSON parser");
        return 1;
    }
    return 0;
}

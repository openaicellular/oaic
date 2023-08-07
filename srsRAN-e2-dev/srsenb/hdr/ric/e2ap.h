#ifndef RIC_E2AP_H
#define RIC_E2AP_H

#include <list>
#include <cstdint>

/* RIC E2AP port. */
#define E2AP_PORT 36422

/* E2AP SCTP Payload Protocol Identifier (PPID) */
#define E2AP_SCTP_PPID 70

namespace ric {

/**
 * These are local function IDs.  Each service model might expose many
 * functions.  E2SM functions do not currently have global IDs, unless
 * you concat the E2SM OID and the function name.  There is no
 * requirement that function IDs be the same for different E2Setup/Reset
 * sessions, so we allow e2sm modules to register functions.
 */
typedef long ran_function_id_t;

typedef struct action {
  long id;
  long type;
  long error_cause;
  long error_cause_detail;
  size_t def_size;
  uint8_t *def_buf;
  long subsequent_action;
  long time_to_wait;
  bool enabled;
} action_t;

typedef struct event_trigger {
  uint8_t *buf;
  size_t size;
} event_trigger_t;

typedef struct subscription {
  long request_id;
  long instance_id;
  ran_function_id_t function_id;
  event_trigger_t event_trigger;

  bool enabled;
  std::list<action_t *> actions;

  void *model_data;
} subscription_t;

typedef enum control_request_ack {
    CONTROL_REQUEST_NONE = 0,
    CONTROL_REQUEST_ACK,
    CONTROL_REQUEST_NACK
} control_request_ack_t;

typedef struct control {
  long request_id;
  long instance_id;
  ran_function_id_t function_id;
  control_request_ack_t request_ack;
  uint8_t *header_buf;
  size_t header_len;
  uint8_t *message_buf;
  size_t message_len;
  void *model_data;
} control_t;

/**
 * These are generic service mechanisms.
 */
typedef enum {
  RIC_REPORT = 1,
  RIC_INSERT = 2,
  RIC_CONTROL = 3,
  RIC_POLICY = 4,
} service_t;

}
#endif

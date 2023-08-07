#ifndef RIC_AGENT_DEFS_H
#define RIC_AGENT_DEFS_H

namespace ric {

class agent;

typedef struct agent_args {
  bool        disabled;
  std::string remote_ipv4_addr;
  uint16_t    remote_port;
  std::string local_ipv4_addr;
  uint16_t    local_port;
  bool        no_reconnect;
  std::string functions_disabled;
  std::string log_level;
  int log_hex_limit;
} agent_args_t;

}

#endif

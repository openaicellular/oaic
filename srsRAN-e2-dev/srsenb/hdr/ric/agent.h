#ifndef RIC_AGENT_H
#define RIC_AGENT_H

#include <ctime>
#include <list>
#include <map>

#include <sys/socket.h>

#include "srsran/common/common.h" // common.h has been broken up. May need to include new libs
#include "srsran/common/byte_buffer.h" // New library included to support unique_byte_buffer_t
//#include "srsran/common/log.h"        //Does not exist
//#include "srsran/common/logger.h"	// Not present
//#include "srsran/common/log_filter.h"	// Not present
#include "srsran/srslog/srslog.h"
#include "srsran/common/network_utils.h" 
#include "srsran/common/multiqueue.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/ric/e2ap.h"
#include "srsenb/hdr/ric/e2sm.h"

namespace ric {

typedef enum {
  RIC_UNINITIALIZED = 0,
  RIC_INITIALIZED = 1,
  RIC_CONNECTED = 2,
  RIC_ESTABLISHED = 3,
  RIC_FAILURE = 4,
  RIC_DISCONNECTED = 5,
  RIC_DISABLED = 6,
} agent_state_t;

#define RIC_AGENT_RECONNECT_DELAY_INC 5
#define RIC_AGENT_RECONNECT_DELAY_MAX 60

class agent : public srsran::thread
{
public:
  agent(srslog::sink& log_sink,
	srsenb::enb_metrics_interface *enb__metrics_interface_    // Changed from srslte::logger *logger to srslog::sink& log_sink_
#ifdef ENABLE_SLICER
	,
	srsenb::enb_slicer_interface *enb__slicer_interface_
#endif
  );
  virtual ~agent();

  int init(const srsenb::all_args_t& args_,
	   srsenb::phy_cfg_t& phy_cfg_,
	   srsenb::rrc_cfg_t rrc_cfg_); // almost same. rrc_cfg_qci_t in rrc_cfg_t is an array in original
  void stop();
  int reset();
  bool send_sctp_data(uint8_t *buf,ssize_t len);
  bool is_function_enabled(std::string &function_name);
  void set_state(agent_state_t state_);
  bool is_state_stale(int seconds);
  void set_ric_id(uint32_t id,uint16_t mcc,uint16_t mnc);
  ric::ran_function_t *lookup_ran_function(ran_function_id_t function_id);
  ric::subscription_t *lookup_subscription(long request_id,long instance_id,
					   ric::ran_function_id_t function_id);
  template <typename FwdRef>
  void push_task(FwdRef&& value) {
    agent_queue.push(value); // agent_queue_id variable has been removed from srsRAN push in multiqueue.h. It's just a single argument function
  };
#ifdef ENABLE_SLICER
  void test_slicer_interface();
#endif

  srslog::sink& log_sink; // Remove initialozation to nullptr. change to srslog::sink&
 // struct log{
    srslog::basic_logger& ric; // change from log_filter to srslog::basic_logger&
    srslog::basic_logger& e2ap; // change from log_filter to srslog::basic_logger&
    srslog::basic_logger& e2sm; // change from log_filter to srslog::basic_logger&
//Below three lines are not needed since it is already incorporated above. We will be initializing them in agent.cc file.
    //srslte::LOG_LEVEL_ENUM ric_level;
    //srslte::LOG_LEVEL_ENUM e2ap_level;
    //srslte::LOG_LEVEL_ENUM e2sm_level;
  //}; // whole structure needs checking. Change to srsran::
  srsenb::all_args_t args;
  srsenb::phy_cfg_t phy_cfg;
  srsenb::rrc_cfg_t rrc_cfg;
  std::list<ric::service_model *> service_models;
  srsenb::enb_metrics_interface *enb_metrics_interface;
#ifdef ENABLE_SLICER
  srsenb::enb_slicer_interface *enb_slicer_interface;
#endif

private:
  void handle_connection_error();
  int connect();
  void disconnect(bool use_shutdown = true);
  int connection_reset(int delay = -1);
  void run_thread() override;
  void stop_impl();
  bool handle_message(srsran::unique_byte_buffer_t pdu,
		      const sockaddr_in &from,const sctp_sndrcvinfo &sri,
		      int flags); // need to include byte_buffer.h for 1st arg

  agent_state_t state;
  std::time_t state_time;
  std::map<ric::ran_function_id_t,ric::ran_function_t *> function_id_map;
  std::list<subscription_t *> subscriptions;
  srsran::unique_socket ric_socket;  // socket_handler_t renamed to unique_socket
  int current_reconnect_delay = 0;
  uint16_t ric_mcc,ric_mnc;
  uint32_t ric_id;
  struct sockaddr_in ric_sockaddr = {};
  std::unique_ptr<srsran::socket_manager> rx_sockets; // rx_multisocket_handler changed to socket_manager

  bool agent_thread_started = false;
  srsran::task_multiqueue pending_tasks;
  srsran::task_queue_handle agent_queue;
  srsran::timer_handler timers;

  std::list<std::string> functions_disabled;
  std::string remote_ipv4_addr;
  uint16_t remote_port;
  std::string local_ipv4_addr;
  uint16_t local_port;

  // file pointer for I/Q data
  FILE* f;
  uint8_t iq_buffer[11520000];
};

#define RIC_DEBUG(msg,args...) ric.debug(msg,##args)
#define RIC_INFO(msg,args...) ric.info(msg,##args)
#define RIC_WARN(msg,args...) ric.warning(msg,##args)
#define RIC_ERROR(msg,args...) ric.error(msg,##args)

#define E2AP_DEBUG(agent,msg,args...) agent->e2ap.debug(msg,##args)
#define E2AP_INFO(agent,msg,args...) agent->e2ap.info(msg,##args)
#define E2AP_WARN(agent,msg,args...) agent->e2ap.warning(msg,##args)
#define E2AP_ERROR(agent,msg,args...) agent->e2ap.error(msg,##args)

#define E2SM_DEBUG(agent,msg,args...) agent->e2sm.debug(msg,##args)
#define E2SM_INFO(agent,msg,args...) agent->e2sm.info(msg,##args)
#define E2SM_WARN(agent,msg,args...) agent->e2sm.warning(msg,##args)
#define E2SM_ERROR(agent,msg,args...) agent->e2sm.error(msg,##args)

}

#endif

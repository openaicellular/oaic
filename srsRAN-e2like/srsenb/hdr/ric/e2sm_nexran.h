#ifndef RIC_E2SM_NEXRAN_H
#define RIC_E2SM_NEXRAN_H

#include <list>
#include <map>
#include <queue>

#include "pthread.h"

//#include "srslte/common/timeout.h"
#include "srsenb/hdr/stack/upper/common_enb.h" // Not present
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsenb/hdr/stack/rrc/rrc_metrics.h"

#include "srsenb/hdr/ric/e2ap.h"
#include "srsenb/hdr/ric/e2sm.h"
#include "srsenb/hdr/ric/timer_queue.h"
#include "srsenb/hdr/stack/mac/slicer_defs.h"

namespace ric {

class nexran_model : public service_model
{
public:

  typedef struct subscription_model_data {
    long period;
    bool on_events;
    int timer_id;
  } subscription_model_data_t;

  nexran_model(ric::agent *agent_);
  int init();
  void stop();
  virtual ~nexran_model() { stop(); };
  static void *timer_callback(int timer_id,void *arg);
  void send_indications(int timer_id);
  int handle_subscription_add(ric::subscription_t *sub);
  int handle_subscription_del(ric::subscription_t *sub,int force,
			      long *cause,long *cause_detail);
  void handle_control(ric::control_t *control);

private:
  std::map<std::string,slicer::slice_config_t *> slices;
  std::map<std::string,std::list<std::string>> ues;
  pthread_mutex_t lock;
  long serial_number;
  std::list<ric::subscription_t *> subscriptions;
  timer_queue queue;
};

}

#endif

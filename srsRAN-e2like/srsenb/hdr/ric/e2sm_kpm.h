#ifndef RIC_E2SM_KPM_H
#define RIC_E2SM_KPM_H

#include <list>
#include <map>
#include <queue>

#include "pthread.h"

#include "srsran/common/timeout.h"
//#include "srsenb/hdr/stack/upper/common_enb.h" // Not present
#include "srsenb/hdr/common/common_enb.h" //common_enb.h moved to new folder in srsRAN codebase
#include "srsran/interfaces/enb_metrics_interface.h" 
#include "srsenb/hdr/stack/rrc/rrc_metrics.h"

#include "srsenb/hdr/ric/e2ap.h"
#include "srsenb/hdr/ric/e2sm.h"
#include "srsenb/hdr/ric/timer_queue.h"

#include "E2SM_KPM_RT-Period-IE.h"

namespace ric {

#define NUM_PERIODS (E2SM_KPM_RT_Period_IE_ms10240 + 1)

class kpm_model : public service_model
{
public:

  class metrics
  {
  public:

    metrics();
    metrics(srsenb::enb_metrics_t *em);
    void merge_diff(metrics &nm);
    void reset();

    bool have_bytes;
    bool have_prbs;
    long active_ue_count;
    uint64_t dl_bytes_by_qci[MAX_NOF_QCI];
    uint64_t ul_bytes_by_qci[MAX_NOF_QCI];
    uint64_t dl_prbs_by_qci[MAX_NOF_QCI];
    uint64_t ul_prbs_by_qci[MAX_NOF_QCI];
  };

  typedef struct subscription_model_data {
    std::list<int> periods;
  } subscription_model_data_t;

  typedef struct report_period {
    int ms;
    int timer_id;
    metrics last_metrics;
    std::list<ric::subscription_t *> subscriptions;
  } report_period_t;

  kpm_model(ric::agent *agent_);
  int init();
  void stop();
  virtual ~kpm_model() { stop(); };
  static void *timer_callback(int timer_id,void *arg);
  void send_indications(int timer_id);
  int handle_subscription_add(ric::subscription_t *sub);
  int handle_subscription_del(ric::subscription_t *sub,int force,
			      long *cause,long *cause_detail);
  void handle_control(ric::control_t *control);

private:
  pthread_mutex_t lock;
  std::list<ric::subscription_t *> subscriptions;
  report_period_t periods[NUM_PERIODS];
  long serial_number;
  timer_queue queue;
};

}

#endif

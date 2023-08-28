#include "srsran/common/common.h"

#include <srsenb/hdr/ric/e2sm.h>

namespace ric
{

ran_function_id_t next_ran_function_id = 0;

ran_function_id_t get_next_ran_function_id()
{
  return next_ran_function_id++;
}

// For now, SRSRAN_ERROR is not being recognized in this file. We will return -1 for now as a temporary fix
int service_model::init()
{
  return -1;
}

service_model::~service_model()
{
}

int service_model::handle_subscription_add(ric::subscription_t *sub)
{
  return -1;
}

int service_model::handle_subscription_del(
  ric::subscription_t *sub,int force,long *cause,long *cause_detail)
{
  return -1;
}

void service_model::handle_control(ric::control_t *control)
{
}

}

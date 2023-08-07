#ifndef RIC_E2SM_H
#define RIC_E2SM_H

#include <list>
#include <string>
#include <cstdint>

#include "srsran/common/common.h"
#include "srsenb/hdr/ric/e2ap.h"

#define E2SM_MAX_DEF_SIZE 16384

namespace ric
{

class agent;
class service_model;

typedef struct ran_function {
  ran_function_id_t function_id;
  long revision;
  std::string name;
  std::string description;

  uint8_t *enc_definition;
  ssize_t enc_definition_len;

  int enabled;
  void *definition;

  service_model *model;
} ran_function_t;

ran_function_id_t get_next_ran_function_id();

class service_model
{
public:
  service_model(ric::agent *agent_,const std::string name_,const std::string oid_):
    agent(agent_), name(name_), oid(oid_) {};
  virtual int init();
  virtual void stop() = 0;
  virtual ~service_model();
  virtual int handle_subscription_add(ric::subscription_t *sub);
  virtual int handle_subscription_del(ric::subscription_t *sub,int force,
				      long *cause,long *cause_detail);
  virtual void handle_control(ric::control_t *control);

  const std::string name;
  const std::string oid;
  std::list<ric::ran_function_t *> functions;

  ric::agent *agent;
};

}

#endif

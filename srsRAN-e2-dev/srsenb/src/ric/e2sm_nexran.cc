
#include <sys/time.h>

#include "srslte/interfaces/enb_metrics_interface.h"
#include "srsenb/hdr/stack/rrc/rrc_metrics.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srsenb/hdr/ric/e2ap.h"
#include "srsenb/hdr/ric/e2sm.h"
#include "srsenb/hdr/ric/agent.h"
#include "srsenb/hdr/ric/agent_asn1.h"
#include "srsenb/hdr/ric/e2sm_nexran.h"
#include "srsenb/hdr/ric/e2ap_encode.h"
#include "srsenb/hdr/ric/e2ap_decode.h"
#include "srsenb/hdr/ric/e2ap_generate.h"

#include "E2AP_Cause.h"
#include "E2AP_RICactionType.h"
#include "E2AP_RICindicationType.h"
#include "E2AP_RICcontrolStatus.h"
#include "E2SM_NEXRAN_RANfunction-Description.h"
#include "E2SM_NEXRAN_E2SM-NexRAN-ControlHeader.h"
#include "E2SM_NEXRAN_E2SM-NexRAN-ControlMessage.h"
#include "E2SM_NEXRAN_SliceConfig.h"
#include "E2SM_NEXRAN_UeStatus.h"
#include "E2SM_NEXRAN_SliceStatus.h"
#include "E2SM_NEXRAN_E2SM-NexRAN-EventTriggerDefinition.h"
#include "E2SM_NEXRAN_E2SM-NexRAN-IndicationHeader.h"
#include "E2SM_NEXRAN_E2SM-NexRAN-IndicationMessage.h"
#include "E2SM_NEXRAN_E2SM-NexRAN-ControlOutcome.h"

namespace ric {

nexran_model::nexran_model(ric::agent *agent_) :
  service_model(agent_,"ORAN-E2SM-NEXRAN","1.3.6.1.4.1.1.1.2.100"),
  serial_number(1), lock(PTHREAD_MUTEX_INITIALIZER)
{
}

int nexran_model::init()
{
  ric::ran_function_t *func;
  E2SM_NEXRAN_RANfunction_Description_t *func_def;

  E2SM_INFO(agent,"nexran: building function list\n");

  /* Create and encode our function list. */
  func = (ric::ran_function_t *)calloc(1,sizeof(*func));
  func->function_id = get_next_ran_function_id();
  func->model = this;
  func->revision = 0;
  func->name = "ORAN-E2SM-NEXRAN";
  func->description = "NexRAN API";

  func_def = (E2SM_NEXRAN_RANfunction_Description_t *) \
    calloc(1,sizeof(*func_def));

  func_def->ranFunction_Name.ranFunction_ShortName.buf = \
    (uint8_t *)strdup(func->name.c_str());
  func_def->ranFunction_Name.ranFunction_ShortName.size = \
    strlen(func->name.c_str());
  func_def->ranFunction_Name.ranFunction_E2SM_OID.buf = \
    (uint8_t *)strdup(func->model->oid.c_str());
  func_def->ranFunction_Name.ranFunction_E2SM_OID.size = \
    strlen(func->model->oid.c_str());
  func_def->ranFunction_Name.ranFunction_Description.buf = \
    (uint8_t *)strdup(func->description.c_str());
  func_def->ranFunction_Name.ranFunction_Description.size = \
    strlen(func->description.c_str());

  func->enc_definition_len = ric::e2ap::encode(
    &asn_DEF_E2SM_NEXRAN_RANfunction_Description,0,
    func_def,&func->enc_definition);
  if (func->enc_definition_len < 0) {
    E2SM_ERROR(agent,
      "failed to encode %s function %s!\n",
      name.c_str(),func->name.c_str());
    ASN_STRUCT_FREE_CONTENTS_ONLY(
      asn_DEF_E2SM_NEXRAN_RANfunction_Description,func_def);
    free(func_def);
    free(func);

    return -1;
  }

  func->enabled = 1;
  func->definition = func_def;

  functions.push_back(func);

  queue.start();

  return 0;
}

void nexran_model::stop()
{
  pthread_mutex_lock(&lock);
  queue.stop();
  for (auto it = subscriptions.begin(); it != subscriptions.end(); ++it) {
    ric::subscription_t *sub = *it;
    subscription_model_data *md = (subscription_model_data *)sub->model_data;
    delete md;
    sub->model_data = NULL;
  }
  subscriptions.clear();
  pthread_mutex_unlock(&lock);
}

int nexran_model::handle_subscription_add(ric::subscription_t *sub)
{
  E2SM_NEXRAN_E2SM_NexRAN_EventTriggerDefinition_t etdef;
  ric::action_t *action;
  bool any_actions_enabled = false;
  subscription_model_data *md;
  struct timeval tv;

  if (sub->event_trigger.size < 1 || sub->event_trigger.buf == NULL) {
    E2SM_ERROR(agent,"nexran: no report event trigger\n");
    return -1;
  }

  md = new subscription_model_data_t{};
  memset(&etdef,0,sizeof(etdef));
  if (ric::e2ap::decode(
	agent,&asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_EventTriggerDefinition,&etdef,
	sub->event_trigger.buf,sub->event_trigger.size)) {
    E2SM_ERROR(agent,"failed to decode e2sm nexran event trigger definition\n");
    goto errout;
  }
  if (etdef.present != E2SM_NEXRAN_E2SM_NexRAN_EventTriggerDefinition_PR_periodDefinition
      && etdef.present != E2SM_NEXRAN_E2SM_NexRAN_EventTriggerDefinition_PR_ranEventDefinition) {
    E2SM_ERROR(agent,"unrecognized nexran event trigger definition\n");
    goto errout;
  }
  if (etdef.present == E2SM_NEXRAN_E2SM_NexRAN_EventTriggerDefinition_PR_periodDefinition) {
    md->period = etdef.choice.periodDefinition.period;
    md->on_events = false;
  }
  else {
    // XXX: approximate events by 1pps stream
    md->period = 1000;
    md->on_events = true;
  }
  for (std::list<ric::action_t *>::iterator it = sub->actions.begin(); it != sub->actions.end(); ++it) {
    action = *it;
    if (action->type != E2AP_RICactionType_report) {
	E2SM_WARN(agent,"nexran: invalid actionType %ld; not enabling this action\n",
		  action->type);
      continue;
    }
    action->enabled = true;
    any_actions_enabled = true;
  }
  if (!any_actions_enabled) {
    E2SM_ERROR(agent,"nexran: no actions enabled; failing subscription\n");
    goto errout;
  }

  /* This is a valid subscription; add it. */
  pthread_mutex_lock(&lock);
  subscriptions.push_back(sub);

  tv = { md->period / 1000, (md->period % 1000) * 1000 };
  md->timer_id = queue.insert_periodic(tv,timer_callback,this);
  sub->model_data = md;

  pthread_mutex_unlock(&lock);

  return 0;

 errout:
  pthread_mutex_unlock(&lock);
  delete md;
  ASN_STRUCT_FREE_CONTENTS_ONLY(
    asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_EventTriggerDefinition,(&etdef));

  return -1;
}

int nexran_model::handle_subscription_del(
  ric::subscription_t *sub,int force,long *cause,long *cause_detail)
{
  subscription_model_data_t *md = (subscription_model_data_t *)sub->model_data;

  pthread_mutex_lock(&lock);

  queue.cancel(md->timer_id);
  delete md;
  sub->model_data = NULL;
  subscriptions.remove(sub);

  pthread_mutex_unlock(&lock);

  return 0;
}

void *nexran_model::timer_callback(int timer_id,void *arg)
{
  nexran_model *model = (nexran_model *)arg;
  model->agent->push_task([model,timer_id]() { model->send_indications(timer_id); });
  return NULL;
}

static void fill_slice_status_report(E2SM_NEXRAN_SliceStatusReport *report,
				     std::vector<slicer::slice_status_t> statuses)
{
  for (auto it = statuses.begin(); it != statuses.end(); ++it) {
    slicer::slice_status_t &status = *it;
    E2SM_NEXRAN_SliceStatus_t *item = (E2SM_NEXRAN_SliceStatus_t *)calloc(1,sizeof(*item));

    item->sliceName.buf = (uint8_t *)malloc(status.config.name.size() + 1);
    item->sliceName.size = status.config.name.size();
    strncpy((char *)item->sliceName.buf,status.config.name.c_str(),
	    item->sliceName.size + 1);

    item->schedPolicy.present = E2SM_NEXRAN_SchedPolicy_PR_proportionalAllocationPolicy;
    item->schedPolicy.choice.proportionalAllocationPolicy.share = \
	status.config.prop_alloc_policy.share;

    for (auto it2 = status.ue_list.begin(); it2 != status.ue_list.end(); ++it2) {
      E2SM_NEXRAN_UeStatus *ue_item = (E2SM_NEXRAN_UeStatus *)calloc(1,sizeof(*ue_item));

      char imsi_buf[32];
      snprintf(imsi_buf,sizeof(imsi_buf),"%lu",it2->imsi);
      imsi_buf[31] = '\0';
      ue_item->imsi.buf = (uint8_t *)strdup(imsi_buf);
      ue_item->imsi.size = strlen(imsi_buf);

      ue_item->connected = (int)it2->connected;
	
      ASN_SEQUENCE_ADD(&item->ueList.list,ue_item);
    }

    ASN_SEQUENCE_ADD(&report->sliceStatusList.list,item);
  }
}

void nexran_model::send_indications(int timer_id)
{
  uint8_t *buf = NULL;
  ssize_t buf_len = 0;
  ric::subscription_t *sub;
  subscription_model_data_t *md;
  std::list<ric::action_t *>::iterator it2;
  ric::action_t *action;
  E2SM_NEXRAN_E2SM_NexRAN_IndicationHeader_t ih;
  E2SM_NEXRAN_E2SM_NexRAN_IndicationMessage_t im;
  uint8_t *header_buf = NULL;
  ssize_t header_buf_len = 0;
  uint8_t *msg_buf = NULL;
  ssize_t msg_buf_len = 0;

  pthread_mutex_lock(&lock);

  for (auto it = subscriptions.begin(); it != subscriptions.end(); ++it) {
    sub = (ric::subscription_t *)*it;
    md = (subscription_model_data_t *)sub->model_data;
    if (timer_id == md->timer_id)
      break;
    sub = NULL;
    md = NULL;
  }
  if (sub == NULL) {
    E2SM_ERROR(agent,"nexran: bogus timer_id %d; ignoring!\n",timer_id);
    pthread_mutex_unlock(&lock);
    return;
  }

  E2SM_INFO(agent,"nexran: sending indications for period %ld ms\n",
	    md->period);

  std::vector<slicer::slice_status_t> statuses = \
    agent->enb_slicer_interface->slice_status(std::vector<std::string>());

  /*
   * NB: we really need this to be action-specific, because actions can
   * request a particular report style, but since we currently only
   * generate one report style, don't worry for now.
   */
  memset(&ih,0,sizeof(ih));
  ih.present = E2SM_NEXRAN_E2SM_NexRAN_IndicationHeader_PR_indicationType;
  ih.choice.indicationType = E2SM_NEXRAN_E2SM_NexRAN_IndicationType_sliceStatusReport;

  E2SM_DEBUG(agent,"nexran indication header:\n");
  E2SM_XER_PRINT(NULL,&asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_IndicationHeader,&ih);

  header_buf_len = ric::e2ap::encode(
    &asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_IndicationHeader,0,&ih,&header_buf);
  if (header_buf_len < 0) {
    E2SM_ERROR(agent,"failed to encode nexran indication header; aborting send_indication\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(
      asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_IndicationHeader,&ih);
    goto out;
  }

  memset(&im,0,sizeof(im));
  im.present = E2SM_NEXRAN_E2SM_NexRAN_IndicationMessage_PR_sliceStatusReport;
  fill_slice_status_report(&im.choice.sliceStatusReport,statuses);

  E2SM_DEBUG(agent,"indication message:\n");
  E2SM_XER_PRINT(NULL,&asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_IndicationMessage,&im);

  msg_buf_len = ric::e2ap::encode(
    &asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_IndicationMessage,0,&im,&msg_buf);
  if (msg_buf_len < 0) {
    E2SM_ERROR(agent,"failed to encode nexran indication msg; aborting send_indication\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(
      asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_IndicationMessage,&im);
    goto out;
  }

  for (it2 = sub->actions.begin(); it2 != sub->actions.end(); ++it2) {
    action = *it2;

    if (ric::e2ap::generate_indication(
          agent,sub->request_id,sub->instance_id,sub->function_id,
	  action->id,serial_number++,(int)E2AP_RICindicationType_report,
	  header_buf,header_buf_len,msg_buf,msg_buf_len,NULL,0,&buf,&buf_len)) {
      E2SM_ERROR(
        agent,"nexran: failed to generate indication (reqid=%ld,instid=%ld,funcid=%ld,actid=%ld)\n",
	sub->request_id,sub->instance_id,sub->function_id,action->id);
    }
    else {
      E2SM_DEBUG(
	agent,"nexran: sending indication (reqid=%ld,instid=%ld,funcid=%ld,actid=%ld)\n",
	sub->request_id,sub->instance_id,sub->function_id,action->id);
      agent->send_sctp_data(buf,buf_len);
    }
    free(buf);
    buf = NULL;
    buf_len = 0;
  }

 out:
  pthread_mutex_unlock(&lock);
  return;
}

void nexran_model::handle_control(ric::control_t *rc)
{
  E2SM_NEXRAN_E2SM_NexRAN_ControlHeader_t ch;
  E2SM_NEXRAN_E2SM_NexRAN_ControlMessage_t cm;
  long cause = 0;
  long cause_detail = 0;
  uint8_t *buf;
  ssize_t len;
  int ret;

  E2SM_DEBUG(agent,"nexran: handle_control\n");

  if (!rc->header_buf || rc->header_len < 1
      || !rc->message_buf || rc->message_len < 1) {
    E2SM_ERROR(agent,"e2sm nexran missing control header or message\n");
    cause = 1;
    cause = 8;
    goto errout;
  }

  memset(&ch,0,sizeof(ch));
  if (ric::e2ap::decode(
	agent,&asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_ControlHeader,&ch,
	rc->header_buf,rc->header_len)) {
    E2SM_ERROR(agent,"failed to decode e2sm nexran control header\n");
    cause = 1;
    cause_detail = 8;
    goto errout;
  }
  if (ch.present != E2SM_NEXRAN_E2SM_NexRAN_ControlHeader_PR_controlHeaderFormat1) {
    E2SM_ERROR(agent,"nexran only supports control header Format1\n");
    cause = 1;
    cause_detail = 8;
    goto errout;
  }

  E2SM_DEBUG(agent,"control header:\n");
  E2SM_XER_PRINT(NULL,&asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_ControlHeader,&ch);

  memset(&cm,0,sizeof(cm));
  if (ric::e2ap::decode(
	agent,&asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_ControlMessage,&cm,
	rc->message_buf,rc->message_len)) {
    E2SM_ERROR(agent,"failed to decode e2sm nexran control message\n");
    cause = 1;
    cause_detail = 8;
    goto errout;
  }
  if (cm.present != E2SM_NEXRAN_E2SM_NexRAN_ControlMessage_PR_controlMessageFormat1) {
    E2SM_ERROR(agent,"nexran only supports control message Format1\n");
    cause = 1;
    cause_detail = 8;
    goto errout;
  }
  if (cm.choice.controlMessageFormat1.present < E2SM_NEXRAN_E2SM_NexRAN_ControlMessage_Format1_PR_sliceConfigRequest
      || cm.choice.controlMessageFormat1.present > E2SM_NEXRAN_E2SM_NexRAN_ControlMessage_Format1_PR_sliceUeUnbindRequest) {
    E2SM_ERROR(agent,"unknown nexran control message\n");
    cause = 1;
    cause_detail = 8;
    goto errout;
  }

  E2SM_DEBUG(agent,"control message:\n");
  E2SM_XER_PRINT(NULL,&asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_ControlMessage,&cm);

  ret = 0;
  switch (cm.choice.controlMessageFormat1.present) {
  case E2SM_NEXRAN_E2SM_NexRAN_ControlMessage_Format1_PR_sliceConfigRequest:
    {
      E2SM_NEXRAN_SliceConfigRequest_t *req = \
	  &cm.choice.controlMessageFormat1.choice.sliceConfigRequest;
      std::vector<slicer::slice_config_t> slice_configs;
      for (int i = 0; i < req->sliceConfigList.list.count; ++i) {
        E2SM_NEXRAN_SliceConfig_t *sc = (E2SM_NEXRAN_SliceConfig_t *) \
	    req->sliceConfigList.list.array[i];
	std::string slice_name((char *)sc->sliceName.buf,
			       sc->sliceName.size);
	long share = sc->schedPolicy.choice.proportionalAllocationPolicy.share;
	slices[slice_name] = new slicer::slice_config_t {
	    slice_name, { (uint32_t)share } };
	slice_configs.push_back(*slices[slice_name]);
	if (!ues.count(slice_name)) {
	    ues[slice_name] = std::list<std::string>();
	}
	E2SM_DEBUG(agent,"configured slice %s share %u\n",
		   slice_name.c_str(),(uint32_t)share);
      }
      agent->enb_slicer_interface->slice_config(slice_configs);
      ret = 0;
    }
    break;
  case E2SM_NEXRAN_E2SM_NexRAN_ControlMessage_Format1_PR_sliceDeleteRequest:
    {
      E2SM_NEXRAN_SliceDeleteRequest_t *req = \
	  &cm.choice.controlMessageFormat1.choice.sliceDeleteRequest;
      // We need to delete all or none, so check for existence first.
      std::vector<std::string> deletes;
      for (int i = 0; i < req->sliceNameList.list.count; ++i) {
        E2SM_NEXRAN_SliceName_t *sn = (E2SM_NEXRAN_SliceName_t *) \
	    req->sliceNameList.list.array[i];
	std::string slice_name((char *)sn->buf,sn->size);
	deletes.push_back(slice_name);
	if (!slices.count(slice_name)) {
	  ret = 1;
	  break;
	}
      }
      if (ret)
	  break;
      for (auto it = deletes.begin(); it != deletes.end(); ++it) {
	  slices.erase(*it);
	  ues.erase(*it);
	  E2SM_DEBUG(agent,"deleted slice %s\n",it->c_str());
      }
      agent->enb_slicer_interface->slice_delete(deletes);
      ret = 0;
    }
    break;
  case E2SM_NEXRAN_E2SM_NexRAN_ControlMessage_Format1_PR_sliceStatusRequest:
    {
      E2SM_NEXRAN_SliceStatusRequest_t *req = \
	  &cm.choice.controlMessageFormat1.choice.sliceStatusRequest;
      // Don't worry about nonexistent slice names.
      std::vector<std::string> slice_names;
      for (int i = 0; i < req->sliceNameList.list.count; ++i) {
        E2SM_NEXRAN_SliceName_t *sn = (E2SM_NEXRAN_SliceName_t *) \
	  req->sliceNameList.list.array[i];
	std::string slice_name((char *)sn->buf,sn->size);
	slice_names.push_back(slice_name);
      }
      std::vector<slicer::slice_status_t> slice_statuses = \
	agent->enb_slicer_interface->slice_status(slice_names);
      
      ret = 0;
    }
    break;
  case E2SM_NEXRAN_E2SM_NexRAN_ControlMessage_Format1_PR_sliceUeBindRequest:
    {
      E2SM_NEXRAN_SliceUeBindRequest_t *req = \
	  &cm.choice.controlMessageFormat1.choice.sliceUeBindRequest;
      std::string slice_name((char *)req->sliceName.buf,req->sliceName.size);
      if (!slices.count(slice_name)) {
	  ret = 1;
	  break;
      }
      std::vector<std::string> binds;
      std::vector<uint64_t> imsi_uints;
      for (int i = 0; i < req->imsiList.list.count; ++i) {
        E2SM_NEXRAN_IMSI_t *imsi = (E2SM_NEXRAN_IMSI_t *) \
	    req->imsiList.list.array[i];
	std::string imsi_str((char *)imsi->buf,imsi->size);
	if (std::find(ues[slice_name].begin(),ues[slice_name].end(),imsi_str) != ues[slice_name].end()) {
	    ret = 1;
	    break;
	}
	binds.push_back(imsi_str);
	imsi_uints.push_back(std::stoul(imsi_str));
      }
      if (ret)
	  break;
      for (auto it = binds.begin(); it != binds.end(); ++it) {
	  ues[slice_name].push_back(*it);
	  E2SM_DEBUG(agent,"bound slice %s to IMSI %s\n",
		     slice_name.c_str(),it->c_str());
      }
      agent->enb_slicer_interface->slice_ue_bind(slice_name,imsi_uints);
      ret = 0;
    }
    break;
  case E2SM_NEXRAN_E2SM_NexRAN_ControlMessage_Format1_PR_sliceUeUnbindRequest:
    {
      E2SM_NEXRAN_SliceUeUnbindRequest_t *req = \
	  &cm.choice.controlMessageFormat1.choice.sliceUeUnbindRequest;
      std::string slice_name((char *)req->sliceName.buf,req->sliceName.size);
      if (!slices.count(slice_name)) {
	  ret = 1;
	  break;
      }
      std::vector<std::string> unbinds;
      std::vector<uint64_t> imsi_uints;
      for (int i = 0; i < req->imsiList.list.count; ++i) {
        E2SM_NEXRAN_IMSI_t *imsi = (E2SM_NEXRAN_IMSI_t *) \
	    req->imsiList.list.array[i];
	std::string imsi_str((char *)imsi->buf,imsi->size);
	if (std::find(ues[slice_name].begin(),ues[slice_name].end(),imsi_str) == ues[slice_name].end()) {
	    ret = 1;
	    break;
	}
	unbinds.push_back(imsi_str);
	imsi_uints.push_back(std::stoul(imsi_str));
      }
      if (ret)
	  break;

      ues[slice_name].erase(ues[slice_name].begin());
      for (auto it = ues[slice_name].begin(); it != ues[slice_name].end(); ++it) {
	  if (std::find(unbinds.begin(),unbinds.end(),*it) != unbinds.end()) {
	      ues[slice_name].erase(it);
	      E2SM_DEBUG(agent,"unbound slice %s from IMSI %s\n",
			 slice_name.c_str(),it->c_str());
	  }
      }
      agent->enb_slicer_interface->slice_ue_unbind(slice_name,imsi_uints);
      ret = 0;
    }
    break;
  default:
    E2SM_ERROR(agent,"unknown nexran control message\n");
    ret = 1;
    cause = 1;
    cause_detail = 8;
  }

  if (ret) {
    E2SM_ERROR(agent,"error while handling nexran control request (%d)\n",ret);
    cause = 1;
    cause_detail = 8;
    goto errout;
  }

  if (rc->request_ack == CONTROL_REQUEST_ACK) {
    E2SM_DEBUG(agent,
	       "building slice status report:\n");
    E2SM_NEXRAN_E2SM_NexRAN_ControlOutcome_t outcome;
    memset(&outcome,0,sizeof(outcome));
    outcome.present = E2SM_NEXRAN_E2SM_NexRAN_ControlOutcome_PR_controlOutcomeFormat1;
    outcome.choice.controlOutcomeFormat1.present = \
	E2SM_NEXRAN_E2SM_NexRAN_ControlOutcome_Format1_PR_sliceStatusReport;
    std::vector<slicer::slice_status_t> statuses = \
	agent->enb_slicer_interface->slice_status(std::vector<std::string>());
    fill_slice_status_report(
      &outcome.choice.controlOutcomeFormat1.choice.sliceStatusReport,
      statuses);
    E2SM_DEBUG(agent,"nexran slice status report:\n");
    E2SM_XER_PRINT(NULL,&asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_ControlOutcome,&outcome);
    uint8_t *enc_outcome = NULL;
    ssize_t enc_outcome_len = ric::e2ap::encode(
      &asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_ControlOutcome,0,
      &outcome,&enc_outcome);
    if (enc_outcome_len < 0 || !enc_outcome) {
      E2SM_WARN(agent,
		"failed to encode nexran slice status report!\n");
    }
    ASN_STRUCT_FREE_CONTENTS_ONLY(
      asn_DEF_E2SM_NEXRAN_E2SM_NexRAN_ControlOutcome,&outcome);
    ret = ric::e2ap::generate_ric_control_acknowledge(
      agent,rc,E2AP_RICcontrolStatus_success,enc_outcome,enc_outcome_len,&buf,&len);
    if (enc_outcome)
      free(enc_outcome);
    if (ret) {
      E2AP_ERROR(agent,"failed to generate RICcontrolFailure\n");
    }
    else {
      agent->send_sctp_data(buf,len);
    }
  }

  delete rc;
  return;

 errout:
  ret = ric::e2ap::generate_ric_control_failure(
    agent,rc,cause,cause_detail,NULL,0,&buf,&len);
  if (ret) {
    E2AP_ERROR(agent,"failed to generate RICcontrolFailure\n");
  }
  else {
    agent->send_sctp_data(buf,len);
  }
  delete rc;
  return;
}

}

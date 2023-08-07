
#include "srsenb/hdr/ric/e2ap_handle.h"
#include "srsenb/hdr/ric/e2ap_decode.h"
#include "srsenb/hdr/ric/e2ap_encode.h"
#include "srsenb/hdr/ric/e2ap_generate.h"
#include "srsenb/hdr/ric/agent.h"
#include "srsenb/hdr/ric/agent_asn1.h"
#include "srsenb/hdr/ric/e2sm.h"

#include "E2AP_Cause.h"
#include "E2AP_E2AP-PDU.h"
#include "E2AP_ProtocolIE-Field.h"
#include "E2AP_E2setupRequest.h"
#include "E2AP_RICsubsequentAction.h"

namespace ric {
namespace e2ap {

int handle_e2_setup_response(ric::agent *agent,uint32_t stream,
			     E2AP_E2AP_PDU_t *pdu)
{
  E2AP_E2setupResponse_t *resp;
  E2AP_E2setupResponseIEs_t *rie,**ptr;
  bool found = false;
  uint16_t ric_mcc,ric_mnc;
  uint32_t ric_id;

  resp = &pdu->choice.successfulOutcome.value.choice.E2setupResponse;

  E2AP_INFO(agent,"Received E2SetupResponse\n");

  for (ptr = resp->protocolIEs.list.array;
       ptr < &resp->protocolIEs.list.array[resp->protocolIEs.list.count];
       ptr++) {
    rie = (E2AP_E2setupResponseIEs_t *)*ptr;
    if (rie->id == E2AP_ProtocolIE_ID_id_GlobalRIC_ID) {
      ASN1_CONV_PLMNID_TO_MCC_MNC(
        &rie->value.choice.GlobalRIC_ID.pLMN_Identity,ric_mcc,ric_mnc);
      ASN1_CONV_MACRO_ENB_ID_TO_UINT32(
        &rie->value.choice.GlobalRIC_ID.ric_ID,ric_id);
      found = true;
    }
    /* XXX: handle RANfunction IEs once we have some E2SM support. */
  }
  if (!found) {
    E2AP_WARN(agent,"E2SetupResponse missing plmn and macro enb id!\n");
  }
  else {
    E2AP_INFO(agent,"E2SetupResponse from RIC (mcc=%u,mnc=%u,id=%u)\n",
	      ric_mcc,ric_mnc,ric_id);
    agent->set_ric_id(ric_id,ric_mcc,ric_mnc);
  }

  agent->set_state(ric::RIC_ESTABLISHED);

  return 0;
}

int handle_e2_setup_failure(ric::agent *agent,uint32_t stream,
			    E2AP_E2AP_PDU_t *pdu)
{
  E2AP_E2setupFailure_t *resp;
  E2AP_E2setupFailureIEs_t *rie,**ptr;
  long cause = -1,cause_detail = -1;

  resp = &pdu->choice.unsuccessfulOutcome.value.choice.E2setupFailure;

  E2AP_INFO(agent,"Received E2SetupFailure\n");

  for (ptr = resp->protocolIEs.list.array;
       ptr < &resp->protocolIEs.list.array[resp->protocolIEs.list.count];
       ptr++) {
    rie = (E2AP_E2setupFailureIEs_t *)*ptr;
    if (rie->id == E2AP_ProtocolIE_ID_id_Cause) {
	cause = rie->value.choice.Cause.present;
	cause_detail = rie->value.choice.Cause.choice.misc;
    }
    /* XXX: handle RANfunction IEs once we have some E2SM support. */
  }

  E2AP_INFO(agent,"E2SetupFailure from RIC (cause=%ld,detail=%ld)\n",
	    cause,cause_detail);

  agent->set_state(ric::RIC_FAILURE);

  return 0;
}

int handle_ric_subscription_request(ric::agent *agent,uint32_t stream,
				    E2AP_E2AP_PDU_t *pdu)
{
  E2AP_RICsubscriptionRequest_t *req;
  E2AP_RICsubscriptionRequest_IEs_t *rie,**ptr;
  ric::subscription_t *rs;
  ric::action_t *ra;
  int ret;
  uint8_t *buf;
  ssize_t len;
  ric::ran_function_t *func;
  std::list<ric::action_t *>::iterator it;
  bool any_actions_enabled = false;

  req = &pdu->choice.initiatingMessage.value.choice.RICsubscriptionRequest;

  E2AP_INFO(agent,"Received RICsubscriptionRequest\n");

  /* We need to create an ric_subscription to generate errors. */
  rs = new ric::subscription_t;
  rs->event_trigger.buf = NULL;
  rs->event_trigger.size = 0;

  for (ptr = req->protocolIEs.list.array;
       ptr < &req->protocolIEs.list.array[req->protocolIEs.list.count];
       ptr++) {
    rie = (E2AP_RICsubscriptionRequest_IEs_t *)*ptr;
    if (rie->id == E2AP_ProtocolIE_ID_id_RICrequestID) {
      rs->request_id = rie->value.choice.RICrequestID.ricRequestorID;
      rs->instance_id = rie->value.choice.RICrequestID.ricInstanceID;
    }
    else if (rie->id == E2AP_ProtocolIE_ID_id_RANfunctionID) {
      rs->function_id = rie->value.choice.RANfunctionID;
    }
    else if (rie->id == E2AP_ProtocolIE_ID_id_RICsubscriptionDetails) {
      E2AP_RICeventTriggerDefinition_t *rtd = &rie->value.choice.RICsubscriptionDetails.ricEventTriggerDefinition;
      E2AP_RICactions_ToBeSetup_List_t *ral = &rie->value.choice.RICsubscriptionDetails.ricAction_ToBeSetup_List;

      if (rtd->size > 0 && rtd->size < E2SM_MAX_DEF_SIZE) {
	rs->event_trigger.size = rie->value.choice.RICsubscriptionDetails.ricEventTriggerDefinition.size;
	rs->event_trigger.buf = (uint8_t *)malloc(rs->event_trigger.size);
	memcpy(rs->event_trigger.buf,
	       rie->value.choice.RICsubscriptionDetails.ricEventTriggerDefinition.buf,
	       rs->event_trigger.size);
      }
      else if (rtd->size > E2SM_MAX_DEF_SIZE) {
	E2AP_ERROR(agent,"RICsubscriptionRequest eventTriggerDefinition too long!");
	// XXX: protocol error?
      }

      for (int i = 0; i < ral->list.count; ++i) {
	E2AP_RICaction_ToBeSetup_ItemIEs_t *ptr2 = (E2AP_RICaction_ToBeSetup_ItemIEs_t *)ral->list.array[i];
	if (ptr2->id == E2AP_ProtocolIE_ID_id_RICaction_ToBeSetup_Item) {
	  E2AP_RICaction_ToBeSetup_Item_t *rai = \
	    (E2AP_RICaction_ToBeSetup_Item_t *)&ptr2->value.choice.RICaction_ToBeSetup_Item;
	  ra = (ric::action_t *)calloc(1,sizeof(*ra));
	  ra->id = rai->ricActionID;
	  ra->type = rai->ricActionType;
	  if (rai->ricActionDefinition && rai->ricActionDefinition->size > 0) {
	    ra->def_size = rai->ricActionDefinition->size;
	    ra->def_buf = (uint8_t *)malloc(ra->def_size);
	    memcpy(ra->def_buf,rai->ricActionDefinition->buf,ra->def_size);
	  }
	  if (rai->ricSubsequentAction) {
	    ra->subsequent_action = rai->ricSubsequentAction->ricSubsequentActionType;
	    ra->time_to_wait = rai->ricSubsequentAction->ricTimeToWait;
	  }

	  rs->actions.push_back(ra);
	}
      }
    }
  }

  func = agent->lookup_ran_function(rs->function_id);
  if (!func) {
    E2AP_ERROR(agent,"failed to find ran_function %ld\n",rs->function_id);
    goto errout;
  }

  ret = func->model->handle_subscription_add(rs);
  if (ret) {
    E2AP_ERROR(agent,"failed to subscribe to ran_function %ld\n",rs->function_id);
    goto errout;
  }

  /* If not one action is enabled, this subscription fails by definition. */
  for (it = rs->actions.begin(); it != rs->actions.end(); ++it) {
    ra = *it;
    if (ra->enabled) {
      any_actions_enabled = true;
      break;
    }
  }
  if (!any_actions_enabled) {
    E2AP_ERROR(agent,"no actions enabled; fails by definition\n");
    goto errout;
  }

  ret = generate_ric_subscription_response(agent,rs,&buf,&len);
  if (ret) {
    E2AP_ERROR(agent,"failed to generate RICsubscriptionResponse\n");
    goto errout;
  }
  else {
    agent->send_sctp_data(buf,len);
  }

  return 0;

 errout:
  ret = generate_ric_subscription_failure(agent,rs,&buf,&len);
  if (ret) {
    E2AP_ERROR(agent,"failed to generate RICsubscriptionFailure\n");
  }
  else {
    agent->send_sctp_data(buf,len);
  }

  for (it = rs->actions.begin(); it != rs->actions.end(); ++it) {
    ra = *it;
    if (ra->def_buf)
      free(ra->def_buf);
    free(ra);
  }
  if (rs->event_trigger.buf)
    free(rs->event_trigger.buf);
  delete rs;

  return ret;
}

int handle_ric_subscription_delete_request(ric::agent *agent,uint32_t stream,
					   E2AP_E2AP_PDU_t *pdu)
{
  E2AP_RICsubscriptionDeleteRequest_t *req;
  E2AP_RICsubscriptionDeleteRequest_IEs_t *rie,**ptr;
  long request_id = -1, instance_id = -1;
  ric::ran_function_id_t function_id = -1;
  ric::subscription_t *rs;
  int ret;
  uint8_t *buf;
  ssize_t len;
  ric::ran_function_t *func;
  long cause;
  long cause_detail;

  req = &pdu->choice.initiatingMessage.value.choice.RICsubscriptionDeleteRequest;

  E2AP_INFO(agent,"Received RICsubscriptionDeleteRequest\n");

  for (ptr = req->protocolIEs.list.array;
       ptr < &req->protocolIEs.list.array[req->protocolIEs.list.count];
       ptr++) {
    rie = (E2AP_RICsubscriptionDeleteRequest_IEs_t *)*ptr;
    if (rie->id == E2AP_ProtocolIE_ID_id_RICrequestID) {
      request_id = rie->value.choice.RICrequestID.ricRequestorID;
      instance_id = rie->value.choice.RICrequestID.ricInstanceID;
    }
    else if (rie->id == E2AP_ProtocolIE_ID_id_RANfunctionID) {
      function_id = rie->value.choice.RANfunctionID;
    }
  }
  if (function_id < 0) {
    E2AP_ERROR(agent,"ran_function not specified\n");
    cause = E2AP_Cause_PR_ricRequest;
    cause_detail = E2AP_CauseRIC_ran_function_id_Invalid;
    goto errout;
  }
  if (request_id < 0 || instance_id < 0) {
    E2AP_ERROR(agent,"request_id or instance_id not specified\n");
    cause = E2AP_Cause_PR_ricRequest;
    cause_detail = E2AP_CauseRIC_request_id_unknown;
    goto errout;
  }

  func = agent->lookup_ran_function(function_id);
  if (!func) {
    E2AP_ERROR(agent,"failed to find ran_function %ld\n",function_id);
    cause = E2AP_Cause_PR_ricRequest;
    cause_detail = E2AP_CauseRIC_ran_function_id_Invalid;
    goto errout;
  }

  rs = agent->lookup_subscription(request_id,instance_id,function_id);
  if (!rs) {
    E2AP_ERROR(agent,"failed to find subscription %ld/%ld/%ld\n",
	       request_id,instance_id,function_id);
    cause = E2AP_Cause_PR_ricRequest;
    cause_detail = E2AP_CauseRIC_request_id_unknown;
    goto errout;
  }

  ret = func->model->handle_subscription_del(rs,0,&cause,&cause_detail);
  if (ret) {
    E2AP_ERROR(agent,"failed to remove subscription to ran_function %ld\n",
	       rs->function_id);
    goto errout;
  }

  ret = generate_ric_subscription_delete_response(
    agent,request_id,instance_id,function_id,&buf,&len);
  if (ret) {
    E2AP_ERROR(agent,"failed to generate RICsubscriptionDeleteResponse\n");
    cause = E2AP_Cause_PR_protocol;
    cause_detail = E2AP_CauseProtocol_unspecified;
    goto errout;
  }
  else {
    agent->send_sctp_data(buf,len);
  }

  return 0;

 errout:
  ret = generate_ric_subscription_delete_failure(
    agent,request_id,instance_id,function_id,cause,cause_detail,&buf,&len);
  if (ret) {
    E2AP_ERROR(agent,"failed to generate RICsubscriptionDeleteFailure\n");
  }
  else {
    agent->send_sctp_data(buf,len);
  }

  return ret;
}

int handle_ric_service_query(ric::agent *agent,uint32_t stream,
			     E2AP_E2AP_PDU_t *pdu)
{
  uint8_t *buf;
  ssize_t len;
  int ret;

  E2AP_INFO(agent,"Received RICserviceQuery\n");

  /*
   * NB: we never add, modify, or remove service models or functions, so
   * this is a noop for us.
   */
  ret = generate_ric_service_update(agent,&buf,&len);
  if (ret) {
    E2AP_ERROR(agent,"failed to generate RICserviceUpdate\n");
    return -1;
  }
  else {
    agent->send_sctp_data(buf,len);
  }

  return 0;
}

int handle_reset_request(ric::agent *agent,uint32_t stream,
			 E2AP_E2AP_PDU_t *pdu)
{
  uint8_t *buf;
  ssize_t len;
  int ret;

  E2AP_INFO(agent,"Received RICresetRequest\n");

  agent->reset();

  ret = generate_reset_response(agent,&buf,&len);
  if (ret) {
    E2AP_ERROR(agent,"failed to generate RICresetResponse\n");
    return -1;
  }
  else {
    agent->send_sctp_data(buf,len);
  }

  return 0;
}

int handle_control(ric::agent *agent,uint32_t stream,
		   E2AP_E2AP_PDU_t *pdu)
{
  E2AP_RICcontrolRequest_t *req;
  E2AP_RICcontrolRequest_IEs_t *rie,**ptr;
  ric::control_t *rc;
  int ret;
  uint8_t *buf;
  ssize_t len;
  ric::ran_function_t *func;
  std::list<ric::action_t *>::iterator it;

  req = &pdu->choice.initiatingMessage.value.choice.RICcontrolRequest;

  E2AP_INFO(agent,"Received RICcontrolRequest\n");

  /* We need to create an ric_subscription to generate errors. */
  rc = new ric::control_t;

  for (ptr = req->protocolIEs.list.array;
       ptr < &req->protocolIEs.list.array[req->protocolIEs.list.count];
       ptr++) {
    rie = (E2AP_RICcontrolRequest_IEs_t *)*ptr;
    if (rie->id == E2AP_ProtocolIE_ID_id_RICrequestID) {
      rc->request_id = rie->value.choice.RICrequestID.ricRequestorID;
      rc->instance_id = rie->value.choice.RICrequestID.ricInstanceID;
    }
    else if (rie->id == E2AP_ProtocolIE_ID_id_RANfunctionID) {
      rc->function_id = rie->value.choice.RANfunctionID;
    }
    else if (rie->id == E2AP_ProtocolIE_ID_id_RICcontrolAckRequest) {
      rc->request_ack = (ric::control_request_ack_t)rie->value.choice.RICcontrolAckRequest;
    }
    else if (rie->id == E2AP_ProtocolIE_ID_id_RICcontrolHeader
	     && rie->value.choice.RICcontrolHeader.size > 0) {
      rc->header_len = rie->value.choice.RICcontrolHeader.size;
      rc->header_buf = (uint8_t *)malloc(rc->header_len);
      memcpy(rc->header_buf,rie->value.choice.RICcontrolHeader.buf,
	     rc->header_len);
    }
    else if (rie->id == E2AP_ProtocolIE_ID_id_RICcontrolMessage
	     && rie->value.choice.RICcontrolMessage.size > 0) {
      rc->message_len = rie->value.choice.RICcontrolMessage.size;
      rc->message_buf = (uint8_t *)malloc(rc->message_len);
      memcpy(rc->message_buf,rie->value.choice.RICcontrolMessage.buf,
	     rc->message_len);
    }
  }

  func = agent->lookup_ran_function(rc->function_id);
  if (!func) {
    E2AP_ERROR(agent,"failed to find ran_function %ld\n",rc->function_id);
    goto errout;
  }

  /*
   * NB: rc is owned by the model at this point, and model must send
   * control ack/nack if requested/required.  Control requests may take
   * an arbitrarily long amount of time, so we cannot tie up this thread
   * waiting for a response.
   */
  func->model->handle_control(rc);

  return 0;

 errout:
  ret = generate_ric_control_failure(
    agent,rc,1,0,NULL,0,&buf,&len);
  if (ret) {
    E2AP_ERROR(agent,"failed to generate RICcontrolFailure\n");
  }
  else {
    agent->send_sctp_data(buf,len);
  }
  delete rc;

  return ret;
}

int handle_message(ric::agent *agent,uint32_t stream,
		   const uint8_t * const buf,const uint32_t buflen)
{
  E2AP_E2AP_PDU_t pdu;
  int ret;

  memset(&pdu,0,sizeof(pdu));
  ret = decode_pdu(agent,&pdu,buf,buflen);
  if (ret < 0) {
    E2AP_ERROR(agent,"failed to decode PDU\n");
    return -1;
  }

  switch (pdu.present) {
  case E2AP_E2AP_PDU_PR_initiatingMessage:
    switch (pdu.choice.initiatingMessage.procedureCode) {
    case E2AP_ProcedureCode_id_RICsubscription:
      ret = handle_ric_subscription_request(agent,stream,&pdu);
      break;
    case E2AP_ProcedureCode_id_RICsubscriptionDelete:
      ret = handle_ric_subscription_delete_request(agent,stream,&pdu);
      break;
    case E2AP_ProcedureCode_id_RICserviceQuery:
      ret = handle_ric_service_query(agent,stream,&pdu);
      break;
    case E2AP_ProcedureCode_id_Reset:
      ret = handle_reset_request(agent,stream,&pdu);
      break;
    case E2AP_ProcedureCode_id_RICcontrol:
      ret = handle_control(agent,stream,&pdu);
      break;
    default:
      E2AP_WARN(agent,"unsupported initiatingMessage procedure %ld\n",
		pdu.choice.initiatingMessage.procedureCode);
      ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
      return -1;
    };
    break;
  case E2AP_E2AP_PDU_PR_successfulOutcome:
    switch (pdu.choice.successfulOutcome.procedureCode) {
    case E2AP_ProcedureCode_id_E2setup:
      ret = handle_e2_setup_response(agent,stream,&pdu);
      break;
    default:
      E2AP_WARN(agent,"unsupported successfulOutcome procedure %ld\n",
		pdu.choice.initiatingMessage.procedureCode);
      ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
      return -1;
    };
    break;
  case E2AP_E2AP_PDU_PR_unsuccessfulOutcome:
    switch (pdu.choice.unsuccessfulOutcome.procedureCode) {
    case E2AP_ProcedureCode_id_E2setup:
      ret = handle_e2_setup_failure(agent,stream,&pdu);
      break;
    default:
      E2AP_WARN(agent,"unsupported unsuccessfulOutcome procedure %ld\n",
		pdu.choice.initiatingMessage.procedureCode);
      ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
      return -1;
    };
    break;
  default:
    E2AP_ERROR(agent,"unsupported presence %u\n",pdu.present);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return -1;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return ret;
}

}
}


#include "srsran/common/common.h"
#include "srsenb/hdr/ric/e2ap.h"
#include "srsenb/hdr/ric/e2ap_encode.h"
#include "srsenb/hdr/ric/e2sm.h"
#include "srsenb/hdr/ric/agent.h"
#include "srsenb/hdr/ric/agent_asn1.h"

#include "E2AP_E2AP-PDU.h"
#include "E2AP_Cause.h"
#include "E2AP_ProtocolIE-Field.h"
#include "E2AP_InitiatingMessage.h"
#include "E2AP_SuccessfulOutcome.h"
#include "E2AP_UnsuccessfulOutcome.h"
#include "E2AP_E2setupRequest.h"

namespace ric {
namespace e2ap {

int generate_e2_setup_request(
  ric::agent *agent,uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;
  E2AP_E2setupRequest_t *req;
  E2AP_E2setupRequestIEs_t *ie;
  E2AP_RANfunction_ItemIEs_t *ran_function_item_ie;
  ric::ran_function_t *func;
  std::list<ric::service_model *>::iterator it;
  std::list<ric::ran_function_t *>::iterator it2;

  memset(&pdu,0,sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_initiatingMessage;
  pdu.choice.initiatingMessage.procedureCode = E2AP_ProcedureCode_id_E2setup;
  pdu.choice.initiatingMessage.criticality = E2AP_Criticality_reject;
  pdu.choice.initiatingMessage.value.present = E2AP_InitiatingMessage__value_PR_E2setupRequest;
  req = &pdu.choice.initiatingMessage.value.choice.E2setupRequest;

  ie = (E2AP_E2setupRequestIEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_GlobalE2node_ID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_E2setupRequestIEs__value_PR_GlobalE2node_ID;
  /* Hardcode LTE until we srslte has NG RAN stuff. */
  ie->value.choice.GlobalE2node_ID.present = E2AP_GlobalE2node_ID_PR_eNB;
  ASN1_MAKE_PLMNID(
    agent->args.stack.s1ap.mcc,agent->args.stack.s1ap.mnc,
    &ie->value.choice.GlobalE2node_ID.choice.eNB.global_eNB_ID.pLMN_Identity);
  ie->value.choice.GlobalE2node_ID.choice.eNB.global_eNB_ID.eNB_ID.present = \
    E2AP_ENB_ID_PR_macro_eNB_ID;
  ASN1_MAKE_MACRO_ENB_ID(
    agent->args.stack.s1ap.enb_id,
    &ie->value.choice.GlobalE2node_ID.choice.eNB.global_eNB_ID.eNB_ID.choice.macro_eNB_ID);
  ASN_SEQUENCE_ADD(&req->protocolIEs.list,ie);

  /* "Optional" RANfunctions_List. */
  ie = (E2AP_E2setupRequestIEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionsAdded;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_E2setupRequestIEs__value_PR_RANfunctions_List;

  for (it = agent->service_models.begin(); it != agent->service_models.end(); ++it) {
    for (it2 = (*it)->functions.begin(); it2 != (*it)->functions.end(); ++it2) {
      func = *it2;
      if (!(func->enabled))
	continue;

      ran_function_item_ie = (E2AP_RANfunction_ItemIEs_t *) \
	calloc(1,sizeof(*ran_function_item_ie));
      ran_function_item_ie->id = E2AP_ProtocolIE_ID_id_RANfunction_Item;
      ran_function_item_ie->criticality = E2AP_Criticality_reject;
      ran_function_item_ie->value.present = \
	E2AP_RANfunction_ItemIEs__value_PR_RANfunction_Item;
      ran_function_item_ie->value.choice.RANfunction_Item.ranFunctionID = \
	func->function_id;
      ran_function_item_ie->value.choice.RANfunction_Item.ranFunctionRevision = \
        func->revision;

      ran_function_item_ie->value.choice.RANfunction_Item.ranFunctionDefinition.buf = \
	(uint8_t *)malloc(func->enc_definition_len);
      memcpy(ran_function_item_ie->value.choice.RANfunction_Item.ranFunctionDefinition.buf,
	     func->enc_definition,func->enc_definition_len);
      ran_function_item_ie->value.choice.RANfunction_Item.ranFunctionDefinition.size = \
	func->enc_definition_len;
      ASN_SEQUENCE_ADD(&ie->value.choice.RANfunctions_List.list,
		       ran_function_item_ie);
    }
  }
  ASN_SEQUENCE_ADD(&req->protocolIEs.list,ie);

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode E2setupRequest\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  /*
  E2AP_E2AP_PDU_t pdud;
  memset(&pdud,0,sizeof(pdud));
  if (e2ap_decode_pdu(&pdud,*buffer,*len) < 0) {
    E2AP_WARN("Failed to encode E2setupRequest\n");
  }
  */

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

int generate_ric_subscription_response(
  ric::agent *agent,ric::subscription_t *rs,uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;
  E2AP_RICsubscriptionResponse_t *out;
  E2AP_RICsubscriptionResponse_IEs_t *ie;
  E2AP_RICaction_Admitted_ItemIEs_t *ai;
  E2AP_RICaction_NotAdmitted_ItemIEs_t *nai;
  std::list<ric::action_t *>::iterator it;
  ric::action_t *action;

  memset(&pdu, 0, sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_successfulOutcome;
  pdu.choice.successfulOutcome.procedureCode = E2AP_ProcedureCode_id_RICsubscription;
  pdu.choice.successfulOutcome.criticality = E2AP_Criticality_reject;
  pdu.choice.successfulOutcome.value.present = E2AP_SuccessfulOutcome__value_PR_RICsubscriptionResponse;
  out = &pdu.choice.successfulOutcome.value.choice.RICsubscriptionResponse;

  ie = (E2AP_RICsubscriptionResponse_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICrequestID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionResponse_IEs__value_PR_RICrequestID;
  ie->value.choice.RICrequestID.ricRequestorID = rs->request_id;
  ie->value.choice.RICrequestID.ricInstanceID = rs->instance_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICsubscriptionResponse_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionResponse_IEs__value_PR_RANfunctionID;
  ie->value.choice.RANfunctionID = rs->function_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICsubscriptionResponse_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICactions_Admitted;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionResponse_IEs__value_PR_RICaction_Admitted_List;
  for (it = rs->actions.begin(); it != rs->actions.end(); ++it) {
    action = *it;
    if (!action->enabled)
      continue;
    ai = (E2AP_RICaction_Admitted_ItemIEs_t *)calloc(1,sizeof(*ai));
    ai->id = E2AP_ProtocolIE_ID_id_RICaction_Admitted_Item;
    ai->criticality = E2AP_Criticality_reject;
    ai->value.present = E2AP_RICaction_Admitted_ItemIEs__value_PR_RICaction_Admitted_Item;
    ai->value.choice.RICaction_Admitted_Item.ricActionID = action->id;
    ASN_SEQUENCE_ADD(&ie->value.choice.RICaction_Admitted_List.list,ai);
  }
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICsubscriptionResponse_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICactions_NotAdmitted;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionResponse_IEs__value_PR_RICaction_NotAdmitted_List;
  for (it = rs->actions.begin(); it != rs->actions.end(); ++it) {
    action = *it;
    if (action->enabled)
      continue;
    nai = (E2AP_RICaction_NotAdmitted_ItemIEs_t *)calloc(1,sizeof(*nai));
    nai->id = E2AP_ProtocolIE_ID_id_RICaction_NotAdmitted_Item;
    nai->criticality = E2AP_Criticality_reject;
    nai->value.present = E2AP_RICaction_NotAdmitted_ItemIEs__value_PR_RICaction_NotAdmitted_Item;
    nai->value.choice.RICaction_NotAdmitted_Item.ricActionID = action->id;
    if (action->error_cause == 0) {
      action->error_cause = E2AP_Cause_PR_ricRequest;
      action->error_cause_detail = E2AP_CauseRIC_unspecified;
    }
    nai->value.choice.RICaction_NotAdmitted_Item.cause.present = (E2AP_Cause_PR)action->error_cause;
    switch (nai->value.choice.RICaction_NotAdmitted_Item.cause.present) {
    case E2AP_Cause_PR_NOTHING:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.ricRequest = 0;
      break;
    case E2AP_Cause_PR_ricRequest:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.ricRequest = action->error_cause_detail;
      break;
    case E2AP_Cause_PR_ricService:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.ricService = action->error_cause_detail;
      break;
    case E2AP_Cause_PR_transport:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.transport = action->error_cause_detail;
      break;
    case E2AP_Cause_PR_protocol:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.protocol = action->error_cause_detail;
      break;
    case E2AP_Cause_PR_misc:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.misc = action->error_cause_detail;
      break;
    default:
      break;
    }
    ASN_SEQUENCE_ADD(&ie->value.choice.RICaction_NotAdmitted_List.list,nai);
  }
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode RICsubscriptionResponse\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

int generate_ric_subscription_failure(
  ric::agent *agent,ric::subscription_t *rs,uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;
  E2AP_RICsubscriptionFailure_t *out;
  E2AP_RICsubscriptionFailure_IEs_t *ie;
  E2AP_RICaction_NotAdmitted_ItemIEs_t *nai;
  std::list<ric::action_t *>::iterator it;
  ric::action_t *action;

  memset(&pdu, 0, sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_unsuccessfulOutcome;
  pdu.choice.unsuccessfulOutcome.procedureCode = E2AP_ProcedureCode_id_RICsubscription;
  pdu.choice.unsuccessfulOutcome.criticality = E2AP_Criticality_reject;
  pdu.choice.unsuccessfulOutcome.value.present = E2AP_UnsuccessfulOutcome__value_PR_RICsubscriptionFailure;
  out = &pdu.choice.unsuccessfulOutcome.value.choice.RICsubscriptionFailure;

  ie = (E2AP_RICsubscriptionFailure_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICrequestID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionFailure_IEs__value_PR_RICrequestID;
  ie->value.choice.RICrequestID.ricRequestorID = rs->request_id;
  ie->value.choice.RICrequestID.ricInstanceID = rs->instance_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICsubscriptionFailure_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionFailure_IEs__value_PR_RANfunctionID;
  ie->value.choice.RANfunctionID = rs->function_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICsubscriptionFailure_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICactions_NotAdmitted;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionFailure_IEs__value_PR_RICaction_NotAdmitted_List;
  for (it = rs->actions.begin(); it != rs->actions.end(); ++it) {
    action = *it;
    nai = (E2AP_RICaction_NotAdmitted_ItemIEs_t *)calloc(1,sizeof(*nai));
    nai->id = E2AP_ProtocolIE_ID_id_RICaction_NotAdmitted_Item;
    nai->criticality = E2AP_Criticality_reject;
    nai->value.present = E2AP_RICaction_NotAdmitted_ItemIEs__value_PR_RICaction_NotAdmitted_Item;
    nai->value.choice.RICaction_NotAdmitted_Item.ricActionID = action->id;
    if (action->error_cause == 0) {
      action->error_cause = E2AP_Cause_PR_ricRequest;
      action->error_cause_detail = E2AP_CauseRIC_unspecified;
    }
    nai->value.choice.RICaction_NotAdmitted_Item.cause.present = (E2AP_Cause_PR)action->error_cause;
    switch (nai->value.choice.RICaction_NotAdmitted_Item.cause.present) {
    case E2AP_Cause_PR_NOTHING:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.ricRequest = 0;
      break;
    case E2AP_Cause_PR_ricRequest:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.ricRequest = action->error_cause_detail;
      break;
    case E2AP_Cause_PR_ricService:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.ricService = action->error_cause_detail;
      break;
    case E2AP_Cause_PR_transport:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.transport = action->error_cause_detail;
      break;
    case E2AP_Cause_PR_protocol:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.protocol = action->error_cause_detail;
      break;
    case E2AP_Cause_PR_misc:
      nai->value.choice.RICaction_NotAdmitted_Item.cause.choice.misc = action->error_cause_detail;
      break;
    default:
      break;
    }
    ASN_SEQUENCE_ADD(&ie->value.choice.RICaction_NotAdmitted_List.list,nai);
  }
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode RICsubscriptionFailure\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

int generate_ric_subscription_delete_response(
  ric::agent *agent,long request_id,long instance_id,
  ric::ran_function_id_t function_id,uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;
  E2AP_RICsubscriptionDeleteResponse_t *out;
  E2AP_RICsubscriptionDeleteResponse_IEs_t *ie;

  memset(&pdu, 0, sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_unsuccessfulOutcome;
  pdu.choice.successfulOutcome.procedureCode = E2AP_ProcedureCode_id_RICsubscription;
  pdu.choice.successfulOutcome.criticality = E2AP_Criticality_reject;
  pdu.choice.successfulOutcome.value.present = E2AP_SuccessfulOutcome__value_PR_RICsubscriptionDeleteResponse;
  out = &pdu.choice.successfulOutcome.value.choice.RICsubscriptionDeleteResponse;

  ie = (E2AP_RICsubscriptionDeleteResponse_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICrequestID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionDeleteResponse_IEs__value_PR_RICrequestID;
  ie->value.choice.RICrequestID.ricRequestorID = request_id;
  ie->value.choice.RICrequestID.ricInstanceID = instance_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICsubscriptionDeleteResponse_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionDeleteResponse_IEs__value_PR_RANfunctionID;
  ie->value.choice.RANfunctionID = function_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode RICsubscriptionDeleteResponse\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

int generate_ric_subscription_delete_failure(
  ric::agent *agent,long request_id,long instance_id,
  ric::ran_function_id_t function_id,long cause,long cause_detail,
  uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;
  E2AP_RICsubscriptionDeleteFailure_t *out;
  E2AP_RICsubscriptionDeleteFailure_IEs_t *ie;

  memset(&pdu, 0, sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_unsuccessfulOutcome;
  pdu.choice.unsuccessfulOutcome.procedureCode = E2AP_ProcedureCode_id_RICsubscription;
  pdu.choice.unsuccessfulOutcome.criticality = E2AP_Criticality_reject;
  pdu.choice.unsuccessfulOutcome.value.present = E2AP_UnsuccessfulOutcome__value_PR_RICsubscriptionDeleteFailure;
  out = &pdu.choice.unsuccessfulOutcome.value.choice.RICsubscriptionDeleteFailure;

  ie = (E2AP_RICsubscriptionDeleteFailure_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICrequestID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionDeleteFailure_IEs__value_PR_RICrequestID;
  ie->value.choice.RICrequestID.ricRequestorID = request_id;
  ie->value.choice.RICrequestID.ricInstanceID = instance_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICsubscriptionDeleteFailure_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionDeleteFailure_IEs__value_PR_RANfunctionID;
  ie->value.choice.RANfunctionID = function_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICsubscriptionDeleteFailure_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICactions_NotAdmitted;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICsubscriptionDeleteFailure_IEs__value_PR_Cause;
  ie->value.choice.Cause.present = (E2AP_Cause_PR)cause;
  switch (cause) {
  case E2AP_Cause_PR_NOTHING:
    break;
  case E2AP_Cause_PR_ricRequest:
    ie->value.choice.Cause.choice.ricRequest = cause_detail;
    break;
  case E2AP_Cause_PR_ricService:
    ie->value.choice.Cause.choice.ricService = cause_detail;
    break;
  case E2AP_Cause_PR_transport:
    ie->value.choice.Cause.choice.transport = cause_detail;
    break;
  case E2AP_Cause_PR_protocol:
    ie->value.choice.Cause.choice.protocol = cause_detail;
    break;
  case E2AP_Cause_PR_misc:
    ie->value.choice.Cause.choice.misc = cause_detail;
    break;
  default:
    break;
  }
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode RICsubscriptionDeleteFailure\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

int generate_ric_service_update(
  ric::agent *agent,uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;
  E2AP_RICserviceUpdate_t *out;
  E2AP_RICserviceUpdate_IEs_t *ie;

  /*
   * NB: we never add, modify, or remove ran functions, so this is a noop.
   */

  memset(&pdu, 0, sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_initiatingMessage;
  pdu.choice.initiatingMessage.procedureCode = E2AP_ProcedureCode_id_RICserviceUpdate;
  pdu.choice.initiatingMessage.criticality = E2AP_Criticality_reject;
  pdu.choice.initiatingMessage.value.present = E2AP_InitiatingMessage__value_PR_RICserviceUpdate;
  out = &pdu.choice.initiatingMessage.value.choice.RICserviceUpdate;

  ie = (E2AP_RICserviceUpdate_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionsAdded;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICserviceUpdate_IEs__value_PR_RANfunctions_List;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICserviceUpdate_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionsModified;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICserviceUpdate_IEs__value_PR_RANfunctions_List_1;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICserviceUpdate_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionsDeleted;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICserviceUpdate_IEs__value_PR_RANfunctionsID_List;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode RICserviceUpdate\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

int generate_reset_response(
  ric::agent *agent,uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;

  memset(&pdu, 0, sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_successfulOutcome;
  pdu.choice.successfulOutcome.procedureCode = E2AP_ProcedureCode_id_Reset;
  pdu.choice.successfulOutcome.criticality = E2AP_Criticality_reject;
  pdu.choice.successfulOutcome.value.present = E2AP_SuccessfulOutcome__value_PR_ResetResponse;

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode ResetResponse\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

int generate_indication(
  ric::agent *agent,long request_id,long instance_id,
  ric::ran_function_id_t function_id,long action_id,long serial_number,
  int type,
  const uint8_t *header_buf,ssize_t header_buf_len,
  const uint8_t *msg_buf,ssize_t msg_buf_len,
  const uint8_t *process_id,ssize_t process_id_len,
  uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;
  E2AP_RICindication_t *out;
  E2AP_RICindication_IEs_t *ie;

  memset(&pdu, 0, sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_initiatingMessage;
  pdu.choice.initiatingMessage.procedureCode = E2AP_ProcedureCode_id_RICindication;
  pdu.choice.initiatingMessage.criticality = E2AP_Criticality_reject;
  pdu.choice.initiatingMessage.value.present = E2AP_InitiatingMessage__value_PR_RICindication;
  out = &pdu.choice.initiatingMessage.value.choice.RICindication;

  ie = (E2AP_RICindication_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICrequestID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICindication_IEs__value_PR_RICrequestID;
  ie->value.choice.RICrequestID.ricRequestorID = request_id;
  ie->value.choice.RICrequestID.ricInstanceID = instance_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICindication_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICindication_IEs__value_PR_RANfunctionID;
  ie->value.choice.RANfunctionID = function_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICindication_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICactionID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICindication_IEs__value_PR_RICactionID;
  ie->value.choice.RICactionID = action_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  if (serial_number >= 0) {
    ie = (E2AP_RICindication_IEs_t *)calloc(1,sizeof(*ie));
    ie->id = E2AP_ProtocolIE_ID_id_RICindicationSN;
    ie->criticality = E2AP_Criticality_reject;
    ie->value.present = E2AP_RICindication_IEs__value_PR_RICindicationSN;
    ie->value.choice.RICindicationSN = serial_number;
    ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);
  }

  ie = (E2AP_RICindication_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICindicationType;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICindication_IEs__value_PR_RICindicationType;
  ie->value.choice.RICindicationType = (enum E2AP_RICindicationType)type;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  if (msg_buf != NULL && msg_buf_len > 0) {
    ie = (E2AP_RICindication_IEs_t *)calloc(1,sizeof(*ie));
    ie->id = E2AP_ProtocolIE_ID_id_RICindicationMessage;
    ie->criticality = E2AP_Criticality_reject;
    ie->value.present = E2AP_RICindication_IEs__value_PR_RICindicationMessage;
    ie->value.choice.RICindicationMessage.buf = (uint8_t *)malloc(msg_buf_len);
    memcpy(ie->value.choice.RICindicationMessage.buf,msg_buf,msg_buf_len);
    ie->value.choice.RICindicationMessage.size = msg_buf_len;
    ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);
  }

  if (header_buf != NULL && header_buf_len > 0) {
    ie = (E2AP_RICindication_IEs_t *)calloc(1,sizeof(*ie));
    ie->id = E2AP_ProtocolIE_ID_id_RICindicationHeader;
    ie->criticality = E2AP_Criticality_reject;
    ie->value.present = E2AP_RICindication_IEs__value_PR_RICindicationHeader;
    ie->value.choice.RICindicationHeader.buf = (uint8_t *)malloc(header_buf_len);
    memcpy(ie->value.choice.RICindicationHeader.buf,header_buf,header_buf_len);
    ie->value.choice.RICindicationHeader.size = header_buf_len;
    ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);
  }

  if (process_id != NULL && process_id_len > 0) {
    ie = (E2AP_RICindication_IEs_t *)calloc(1,sizeof(*ie));
    ie->id = E2AP_ProtocolIE_ID_id_RICcallProcessID;
    ie->criticality = E2AP_Criticality_reject;
    ie->value.present = E2AP_RICindication_IEs__value_PR_RICcallProcessID;
    ie->value.choice.RICcallProcessID.buf = (uint8_t *)malloc(process_id_len);
    memcpy(ie->value.choice.RICcallProcessID.buf,process_id,process_id_len);
    ie->value.choice.RICcallProcessID.size = process_id_len;
    ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);
  }

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode RICindication\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

int generate_ric_control_acknowledge(
  ric::agent *agent,ric::control_t *rc,long status,
  uint8_t *outcome,ssize_t outcome_len,
  uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;
  E2AP_RICcontrolAcknowledge_t *out;
  E2AP_RICcontrolAcknowledge_IEs_t *ie;

  memset(&pdu,0,sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_successfulOutcome;
  pdu.choice.successfulOutcome.procedureCode = E2AP_ProcedureCode_id_RICcontrol;
  pdu.choice.successfulOutcome.criticality = E2AP_Criticality_reject;
  pdu.choice.successfulOutcome.value.present = E2AP_SuccessfulOutcome__value_PR_RICcontrolAcknowledge;
  out = &pdu.choice.successfulOutcome.value.choice.RICcontrolAcknowledge;

  ie = (E2AP_RICcontrolAcknowledge_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICrequestID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICcontrolAcknowledge_IEs__value_PR_RICrequestID;
  ie->value.choice.RICrequestID.ricRequestorID = rc->request_id;
  ie->value.choice.RICrequestID.ricInstanceID = rc->instance_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICcontrolAcknowledge_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICcontrolAcknowledge_IEs__value_PR_RANfunctionID;
  ie->value.choice.RANfunctionID = rc->function_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICcontrolAcknowledge_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICcontrolStatus;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICcontrolAcknowledge_IEs__value_PR_RICcontrolStatus;
  ie->value.choice.RICcontrolStatus = status;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  if (outcome && outcome_len > 0) {
    ie = (E2AP_RICcontrolAcknowledge_IEs_t *)calloc(1,sizeof(*ie));
    ie->id = E2AP_ProtocolIE_ID_id_RICcontrolOutcome;
    ie->criticality = E2AP_Criticality_reject;
    ie->value.present = E2AP_RICcontrolAcknowledge_IEs__value_PR_RICcontrolOutcome;
    ie->value.choice.RICcontrolOutcome.size = outcome_len;
    ie->value.choice.RICcontrolOutcome.buf = (uint8_t *)malloc(outcome_len);
    memcpy(ie->value.choice.RICcontrolOutcome.buf,outcome,outcome_len);
    ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);
  }

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode RICcontrolAcknowledge\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

int generate_ric_control_failure(
  ric::agent *agent,ric::control_t *rc,long cause,long cause_detail,
  uint8_t *outcome,ssize_t outcome_len,
  uint8_t **buffer,ssize_t *len)
{
  E2AP_E2AP_PDU_t pdu;
  E2AP_RICcontrolFailure_t *out;
  E2AP_RICcontrolFailure_IEs_t *ie;

  memset(&pdu,0,sizeof(pdu));
  pdu.present = E2AP_E2AP_PDU_PR_unsuccessfulOutcome;
  pdu.choice.unsuccessfulOutcome.procedureCode = E2AP_ProcedureCode_id_RICcontrol;
  pdu.choice.unsuccessfulOutcome.criticality = E2AP_Criticality_reject;
  pdu.choice.unsuccessfulOutcome.value.present = E2AP_UnsuccessfulOutcome__value_PR_RICcontrolFailure;
  out = &pdu.choice.unsuccessfulOutcome.value.choice.RICcontrolFailure;

  ie = (E2AP_RICcontrolFailure_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RICrequestID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICcontrolFailure_IEs__value_PR_RICrequestID;
  ie->value.choice.RICrequestID.ricRequestorID = rc->request_id;
  ie->value.choice.RICrequestID.ricInstanceID = rc->instance_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICcontrolFailure_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_RANfunctionID;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICcontrolFailure_IEs__value_PR_RANfunctionID;
  ie->value.choice.RANfunctionID = rc->function_id;
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  ie = (E2AP_RICcontrolFailure_IEs_t *)calloc(1,sizeof(*ie));
  ie->id = E2AP_ProtocolIE_ID_id_Cause;
  ie->criticality = E2AP_Criticality_reject;
  ie->value.present = E2AP_RICcontrolFailure_IEs__value_PR_Cause;
  ie->value.choice.Cause.present = (E2AP_Cause_PR)cause;
  switch (cause) {
  case E2AP_Cause_PR_NOTHING:
    break;
  case E2AP_Cause_PR_ricRequest:
    ie->value.choice.Cause.choice.ricRequest = cause_detail;
    break;
  case E2AP_Cause_PR_ricService:
    ie->value.choice.Cause.choice.ricService = cause_detail;
    break;
  case E2AP_Cause_PR_transport:
    ie->value.choice.Cause.choice.transport = cause_detail;
    break;
  case E2AP_Cause_PR_protocol:
    ie->value.choice.Cause.choice.protocol = cause_detail;
    break;
  case E2AP_Cause_PR_misc:
    ie->value.choice.Cause.choice.misc = cause_detail;
    break;
  default:
    break;
  }
  ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);

  if (outcome && outcome_len > 0) {
    ie = (E2AP_RICcontrolFailure_IEs_t *)calloc(1,sizeof(*ie));
    ie->id = E2AP_ProtocolIE_ID_id_RICcontrolOutcome;
    ie->criticality = E2AP_Criticality_reject;
    ie->value.present = E2AP_RICcontrolFailure_IEs__value_PR_RICcontrolOutcome;
    ie->value.choice.RICcontrolOutcome.size = outcome_len;
    ie->value.choice.RICcontrolOutcome.buf = (uint8_t *)malloc(outcome_len);
    memcpy(ie->value.choice.RICcontrolOutcome.buf,outcome,outcome_len);
    ASN_SEQUENCE_ADD(&out->protocolIEs.list,ie);
  }

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,&pdu);

  if (encode_pdu(&pdu,buffer,len) < 0) {
    E2AP_ERROR(agent,"Failed to encode RICcontrolFailure\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
    return SRSRAN_ERROR;
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2AP_E2AP_PDU,&pdu);
  return SRSRAN_SUCCESS;
}

}
}

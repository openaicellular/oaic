
#include "srsenb/hdr/ric/e2ap_decode.h"
#include "srsenb/hdr/ric/agent.h"
#include "srsenb/hdr/ric/agent_asn1.h"

#include "E2AP_E2AP-PDU.h"
#include "E2AP_ProcedureCode.h"
#include "per_decoder.h"

#define CASE_E2AP_I(id,name)					\
    case id:							\
    E2AP_INFO(agent,"decoded initiating " #name " (%ld)\n",id);	\
    break

#define CASE_E2AP_S(id,name)						\
    case id:								\
    E2AP_INFO(agent,"decoded successful outcome " #name " (%ld)\n",id);	\
    break

#define CASE_E2AP_U(id,name)						\
    case id:								\
    E2AP_INFO(agent,"decoded unsuccessful outcome " #name " (%ld)\n",id); \
    break

namespace ric {
namespace e2ap {

int decode(ric::agent *agent,const struct asn_TYPE_descriptor_s *td,
	   void *ptr,const void *buf,const size_t len)
{
  asn_dec_rval_t dres;

  dres = aper_decode(NULL,td,&ptr,buf,len,0,0);
  if (dres.code != RC_OK) {
    E2AP_ERROR(agent,"failed to decode type (%d)\n",dres.code);
    return -1;
  }

  return 0;
}

int decode_pdu(ric::agent *agent,E2AP_E2AP_PDU_t *pdu,
	       const uint8_t * const buf,const size_t len)
{
  asn_dec_rval_t dres;

  dres = aper_decode(NULL,&asn_DEF_E2AP_E2AP_PDU,(void **)&pdu,buf,len,0,0);
  if (dres.code != RC_OK) {
    E2AP_ERROR(agent,"failed to decode PDU (%d)\n",dres.code);
    return -1;
  }

  E2AP_XER_PRINT(NULL,&asn_DEF_E2AP_E2AP_PDU,pdu);

  switch (pdu->present) {
  case E2AP_E2AP_PDU_PR_initiatingMessage:
    switch (pdu->choice.initiatingMessage.procedureCode) {
    CASE_E2AP_I(E2AP_ProcedureCode_id_Reset,Reset);
    CASE_E2AP_I(E2AP_ProcedureCode_id_RICsubscription,
		RICsubscription);
    CASE_E2AP_I(E2AP_ProcedureCode_id_RICsubscriptionDelete,
		RICsubscriptionDelete);
    CASE_E2AP_I(E2AP_ProcedureCode_id_RICcontrol,RICcontrol);
    CASE_E2AP_I(E2AP_ProcedureCode_id_RICserviceQuery,RICserviceQuery);
    CASE_E2AP_I(E2AP_ProcedureCode_id_ErrorIndication,ErrorIndication);
    default:
      E2AP_ERROR(agent,"unknown procedure ID (%d) for initiating message\n",
		 (int)pdu->choice.initiatingMessage.procedureCode);
      return -1;
    }
    break;
  case E2AP_E2AP_PDU_PR_successfulOutcome:
    switch (pdu->choice.successfulOutcome.procedureCode) {
    CASE_E2AP_S(E2AP_ProcedureCode_id_E2setup,E2SetupResponse);
    CASE_E2AP_S(E2AP_ProcedureCode_id_Reset,Reset);
    CASE_E2AP_S(E2AP_ProcedureCode_id_RICserviceUpdate,RICserviceUpdate);
    default:
      E2AP_ERROR(agent,"unknown procedure ID (%d) for successful outcome\n",
		 (int)pdu->choice.successfulOutcome.procedureCode);
      return -1;
    }
    break;
  case E2AP_E2AP_PDU_PR_unsuccessfulOutcome:
    switch (pdu->choice.unsuccessfulOutcome.procedureCode) {
    CASE_E2AP_U(E2AP_ProcedureCode_id_E2setup,E2setupFailure);
    CASE_E2AP_U(E2AP_ProcedureCode_id_RICserviceUpdate,RICserviceUpdate);
    default:
      E2AP_ERROR(agent,"unknown procedure ID (%d) for unsuccessful outcome\n",
		 (int)pdu->choice.unsuccessfulOutcome.procedureCode);
      return -1;
    }
    break;
  default:
    E2AP_ERROR(agent,"unknown presence (%d)\n",(int)pdu->present);
    return -1;
  }

  return 0;
}

}
}

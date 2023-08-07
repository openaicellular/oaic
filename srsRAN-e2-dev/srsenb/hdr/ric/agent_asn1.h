#ifndef RIC_AGENT_ASN1_H
#define RIC_AGENT_ASN1_H

//#include "srslte/common/log.h" // Not present. Have to import from elsewhere
#include "srsran/srslog/srslog.h"
#include "srsran/common/bcd_helpers.h" 
#include "srsran/interfaces/rrc_interface_types.h"
#include "srsenb/hdr/ric/agent.h"
#include "ANY.h"
#include "xer_support.h"

#include "stdio.h"
#include "assert.h"

namespace ric {
extern bool e2ap_xer_print;
extern bool e2sm_xer_print;
}

#define E2AP_XER_PRINT(stream,type,pdu)					\
    do {								\
	if (ric::e2ap_xer_print)					\
	    xer_fprint((stream == NULL) ? stderr : stream,type,pdu);	\
    } while (0);
#define E2SM_XER_PRINT(stream,type,pdu)					\
    do {								\
	if (ric::e2sm_xer_print)					\
	    xer_fprint((stream == NULL) ? stderr : stream,type,pdu);	\
    } while (0);

#define HUNDREDS(_val)	\
  ((_val) / 100)
#define TENS(_val)	\
  (((_val) / 10) % 10)
#define ONES(_val)	\
  ((_val) % 10)

/* Some of these were inspired by the OAI conversion macros. */
/* NB: this assumes that the 0xf prefix has been added to the mnc int. */
/*Change to srsran::*/
#define ASN1_MAKE_PLMNID(_mcc,_mnc,_asn1_octetstring)			\
  do {									\
    srsran::plmn_id_t plmnid;						\
    plmnid.from_number(_mcc,_mnc);					\
    (_asn1_octetstring)->buf = (uint8_t *)calloc(3,sizeof(uint8_t));	\
    (_asn1_octetstring)->size = 3;					\
    plmnid.to_s1ap_plmn_bytes((_asn1_octetstring)->buf);		\
  } while(0)

#define ASN1_MAKE_MACRO_ENB_ID(_enb_id,_asn1_bitstring)			\
  do {                                                    		\
    (_asn1_bitstring)->buf = (uint8_t *)calloc(3,sizeof(uint8_t));	\
    (_asn1_bitstring)->buf[0] = ((_enb_id) >> 12);			\
    (_asn1_bitstring)->buf[1] = (_enb_id) >> 4;				\
    (_asn1_bitstring)->buf[2] = ((_enb_id) & 0x0f) << 4;		\
    (_asn1_bitstring)->size = 3;					\
    (_asn1_bitstring)->bits_unused = 4;					\
  } while(0)

#define ASN1_CONV_MACRO_ENB_ID_TO_UINT32(_asn1_bitstring,_enb_id)	\
  do {                                                    		\
    assert((_asn1_bitstring)->size == 3);				\
    (_enb_id) = 0;							\
    (_enb_id) |= (_asn1_bitstring)->buf[0] << 12;			\
    (_enb_id) |= (_asn1_bitstring)->buf[1] << 4;			\
    (_enb_id) |= (_asn1_bitstring)->buf[2] >> 4;			\
  } while(0)

/* NB: this assumes that the 0xf prefix has been added to the mnc int. */
#define ASN1_CONV_PLMNID_TO_MCC_MNC(_asn1_bitstring,_mcc,_mnc)		\
  do {                                                    		\
    assert((_asn1_bitstring)->size == 3);				\
    (_mcc) = (((_asn1_bitstring)->buf[0] & 0xf) * 100)			\
	| (((_asn1_bitstring)->buf[0] >> 4) * 10)			\
	| (_asn1_bitstring)->buf[1];					\
    (_mnc) = (((_asn1_bitstring)->buf[1] >> 4) * 100)			\
	| (((_asn1_bitstring)->buf[2] & 0xf) * 10)			\
	| ((_asn1_bitstring)->buf[2] >> 4);				\
  } while(0)

#define ASN1_MAKE_NRCGI(_cell_id,_asn1_bitstring)			\
  do { \
    (_asn1_bitstring)->buf = (uint8_t *)calloc(5,sizeof(uint8_t));	\
    (_asn1_bitstring)->buf[0] = ((_cell_id) >> 28) & 0xff;		\
    (_asn1_bitstring)->buf[1] = ((_cell_id) >> 20) & 0xff;		\
    (_asn1_bitstring)->buf[2] = ((_cell_id) >> 12) & 0xff;		\
    (_asn1_bitstring)->buf[3] = ((_cell_id) >> 4) & 0xff;		\
    (_asn1_bitstring)->buf[4] = ((_cell_id) & 0xf) << 4;		\
    (_asn1_bitstring)->size = 5;					\
    (_asn1_bitstring)->bits_unused = 4;					\
  } while(0)

#endif

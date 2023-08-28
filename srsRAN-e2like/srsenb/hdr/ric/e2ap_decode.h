#ifndef _RIC_E2AP_DECODE_H
#define _RIC_E2AP_DECODE_H

#include "E2AP_E2AP-PDU.h"

namespace ric {

class agent;

namespace e2ap {

int decode(ric::agent *agent,const struct asn_TYPE_descriptor_s *td,
	   void *ptr,const void *buf,const size_t len);

int decode_pdu(ric::agent *agent,E2AP_E2AP_PDU_t *pdu,
	       const uint8_t * const buf,const size_t len);

}
}

#endif

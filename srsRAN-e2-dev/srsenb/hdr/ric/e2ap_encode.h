#ifndef RIC_E2AP_ENCODE_H
#define RIC_E2AP_ENCODE_H

#include "E2AP_E2AP-PDU.h"

namespace ric {
namespace e2ap {

ssize_t encode(
  const struct asn_TYPE_descriptor_s *td,
  const asn_per_constraints_t *constraints,void *sptr,uint8_t **buf)
  __attribute__ ((warn_unused_result));
ssize_t encode_pdu(
  E2AP_E2AP_PDU_t *pdu,uint8_t **buf,ssize_t *len)
  __attribute__ ((warn_unused_result));

}
}

#endif

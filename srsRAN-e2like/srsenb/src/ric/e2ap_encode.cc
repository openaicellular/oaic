
#include "srsenb/hdr/ric/e2ap_encode.h"
#include "srsenb/hdr/ric/agent.h"

#include "E2AP_E2AP-PDU.h"

namespace ric {
namespace e2ap {

ssize_t encode(
  const struct asn_TYPE_descriptor_s *td,
  const asn_per_constraints_t *constraints,void *sptr,uint8_t **buf)
{
  ssize_t encoded;

  encoded = aper_encode_to_new_buffer(td,constraints,sptr,(void **)buf);
  if (encoded < 0)
    return -1;

  ASN_STRUCT_FREE_CONTENTS_ONLY((*td),sptr);

  return encoded;
}

ssize_t encode_pdu(E2AP_E2AP_PDU_t *pdu,uint8_t **buf,ssize_t *len)
{
  ssize_t encoded;

  encoded = encode(&asn_DEF_E2AP_E2AP_PDU,0,pdu,buf);
  if (encoded < 0)
    return -1;

  *len = encoded;

  return encoded;
}

}
}

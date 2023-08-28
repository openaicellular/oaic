#ifndef RIC_E2AP_HANDLE_H
#define RIC_E2AP_HANDLE_H

#include "E2AP_E2AP-PDU.h"

namespace ric {

class agent;

namespace e2ap {

int handle_message(ric::agent *agent,uint32_t stream,
		   const uint8_t * const buf,const uint32_t buflen);

}
}
#endif

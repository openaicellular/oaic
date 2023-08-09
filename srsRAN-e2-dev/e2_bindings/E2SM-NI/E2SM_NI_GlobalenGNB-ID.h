/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-NI-IEs"
 * 	found in "/local/mnt/openairinterface5g/openair2/RIC_AGENT/MESSAGES/ASN1/R01/e2sm-ni-v01.00.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example -fno-include-deps -fincludes-quoted -D /local/mnt/openairinterface5g/cmake_targets/ran_build/build/CMakeFiles/E2SM-NI/`
 */

#ifndef	_E2SM_NI_GlobalenGNB_ID_H_
#define	_E2SM_NI_GlobalenGNB_ID_H_


#include "asn_application.h"

/* Including external dependencies */
#include "E2SM_NI_PLMN-Identity.h"
#include "E2SM_NI_ENGNB-ID.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* E2SM_NI_GlobalenGNB-ID */
typedef struct E2SM_NI_GlobalenGNB_ID {
	E2SM_NI_PLMN_Identity_t	 pLMN_Identity;
	E2SM_NI_ENGNB_ID_t	 gNB_ID;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} E2SM_NI_GlobalenGNB_ID_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_E2SM_NI_GlobalenGNB_ID;
extern asn_SEQUENCE_specifics_t asn_SPC_E2SM_NI_GlobalenGNB_ID_specs_1;
extern asn_TYPE_member_t asn_MBR_E2SM_NI_GlobalenGNB_ID_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _E2SM_NI_GlobalenGNB_ID_H_ */
#include "asn_internal.h"
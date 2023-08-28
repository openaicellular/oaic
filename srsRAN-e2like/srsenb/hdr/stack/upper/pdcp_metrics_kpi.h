/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSENB_PDCP_METRICS_KPI_H
#define SRSENB_PDCP_METRICS_KPI_H

#include "srsenb/hdr/common/common_enb.h"
#include "srsenb/hdr/stack/rrc/rrc_config.h"

namespace srsenb {

struct pdcp_ue_metrics_t {
  uint16_t rnti;
  uint64_t dl_bytes[SRSENB_N_RADIO_BEARERS];
  uint64_t dl_bytes_by_qci[MAX_NOF_QCI];
  uint64_t ul_bytes[SRSENB_N_RADIO_BEARERS];
  uint64_t ul_bytes_by_qci[MAX_NOF_QCI];
};

struct pdcp_metrics_kpm_t {
  uint16_t          n_ues;
  pdcp_ue_metrics_t ues[SRSENB_MAX_UES];  //ENB_METRICS_MAX_USERS not used in srslte codebase //replacing it with SRSENB_MAX_UES
};

} // namespace srsenb

#endif // SRSENB_PDCP_METRICS_KPI_H

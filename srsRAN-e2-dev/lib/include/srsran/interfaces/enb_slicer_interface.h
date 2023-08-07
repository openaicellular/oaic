/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_ENB_SLICER_INTERFACE_H
#define SRSRAN_ENB_SLICER_INTERFACE_H

#include "srsenb/hdr/stack/mac/slicer_defs.h"

namespace srsenb {

class enb_slicer_interface
{
public:
  virtual bool slice_config(std::vector<slicer::slice_config_t> slice_configs) = 0;
  virtual bool slice_delete(std::vector<std::string> slice_names) = 0;
  virtual std::vector<slicer::slice_status_t> slice_status(std::vector<std::string> slice_names) = 0;
  virtual bool slice_ue_bind(std::string slice_name, std::vector<uint64_t> imsi_list) = 0;
  virtual bool slice_ue_unbind(std::string slice_name, std::vector<uint64_t> imsi_list) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_SLICER_INTERFACE_H

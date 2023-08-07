# NB: asn1c generates code with warnings.  Hack around this for now.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
endif()

# Set global flags for build of generated asn.1 bindings.
#
# For debugging: add ASN_EMIT_DEBUG to see per-type debug asn.1
# encode/decode info (-DASN_EMIT_DEBUG=1).
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DASN_DISABLE_OER_SUPPORT=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASN_DISABLE_OER_SUPPORT=1")

# The source dir containing our asn.1 specification source files, if any.
set(RIC_ASN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/messages/asn1)

# Add compiler flags to keep intermediate files, so that we can use C2Py for our structs
# https://github.com/sagielevy/C2Py/blob/master/C2Py/tests/Makefile
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -save-temps")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps")

#
# Build the E2AP bindings.
#
if("${RIC_GENERATED_E2AP_BINDING_DIR}" STREQUAL "")
  set(E2AP_ASN_FILE ${RIC_ASN_DIR}/e2ap-v01.00.asn1)
  set(E2AP_C_DIR ${CMAKE_CURRENT_BINARY_DIR}/messages/E2AP/)

  # NB: we need to run an execute_process during makefile parsing, to
  # force population of the generated files, so that we can glob them
  # for dependencies.  When you don't know the file listing being
  # generated a priori, you must use this two-phase approach if you want
  # the bindings to be regenerated again if the specification source is
  # newer.

  execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2AP_C_DIR}" "${E2AP_ASN_FILE}" "E2AP_" -fno-include-deps -fincludes-quoted
    RESULT_VARIABLE ret)
  if (NOT ${ret} STREQUAL 0)
    message(FATAL_ERROR "${ret}: error")
  endif (NOT ${ret} STREQUAL 0)
else()
  set(E2AP_C_DIR "${RIC_GENERATED_E2AP_BINDING_DIR}")
endif()
file(GLOB E2AP_source ${E2AP_C_DIR}/*.c)

# NB: definition of this custom_command has to wait until we have the
# $E2AP_source glob from above, *after* we have run the execute process
# to make the bindings so that we have the dependent files to be used in
# $OUTPUT.
if("${RIC_GENERATED_E2AP_BINDING_DIR}" STREQUAL "")
  add_custom_command (
    OUTPUT ${E2AP_source}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2AP_C_DIR}" "${E2AP_ASN_FILE}" "E2AP_" -fno-include-deps -fincludes-quoted
    DEPENDS ${E2AP_ASN_FILE})
endif()

#
# Build the E2SM bindings.
#
if("${RIC_GENERATED_E2SM_GNB_NRT_BINDING_DIR}" STREQUAL "")
  set(E2SM_GNB_NRT_ASN_FILE ${RIC_ASN_DIR}/e2sm-gNB-NRT-v401.asn1)
  set(E2SM_GNB_NRT_C_DIR ${CMAKE_CURRENT_BINARY_DIR}/messages/E2SM-GNB-NRT/)
  execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2SM_GNB_NRT_C_DIR}" "${E2SM_GNB_NRT_ASN_FILE}" "E2SM_GNB_NRT_" -fno-include-deps -fincludes-quoted
    RESULT_VARIABLE ret)
  if (NOT ${ret} STREQUAL 0)
    message(FATAL_ERROR "${ret}: error")
  endif (NOT ${ret} STREQUAL 0)
else()
  set(E2SM_GNB_NRT_C_DIR "${RIC_GENERATED_E2SM_GNB_NRT_BINDING_DIR}")
endif()
file(GLOB E2SM_GNB_NRT_source ${E2SM_GNB_NRT_C_DIR}/*.c)
if("${RIC_GENERATED_E2SM_GNB_NRT_BINDING_DIR}" STREQUAL "")
  add_custom_command (
    OUTPUT ${E2SM_GNB_NRT_source}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2SM_GNB_NRT_C_DIR}" "${E2SM_GNB_NRT_ASN_FILE}" "E2SM_GNB_NRT_" -fno-include-deps -fincludes-quoted
    DEPENDS ${E2SM_GNB_NRT_ASN_FILE})
endif()

if(ENABLE_RIC_AGENT_KPM)
if("${RIC_GENERATED_E2SM_KPM_BINDING_DIR}" STREQUAL "")
  set(E2SM_KPM_ASN_FILE ${RIC_ASN_DIR}/e2sm-kpm-v01.00.asn1)
  set(E2SM_KPM_C_DIR ${CMAKE_CURRENT_BINARY_DIR}/messages/E2SM-KPM/)
  execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2SM_KPM_C_DIR}" "${E2SM_KPM_ASN_FILE}" "E2SM_KPM_" -fno-include-deps -fincludes-quoted
    RESULT_VARIABLE ret)
  if (NOT ${ret} STREQUAL 0)
    message(FATAL_ERROR "${ret}: error")
  endif (NOT ${ret} STREQUAL 0)
else()
  set(E2SM_KPM_C_DIR "${RIC_GENERATED_E2SM_KPM_BINDING_DIR}")
endif()
file(GLOB E2SM_KPM_source ${E2SM_KPM_C_DIR}/*.c)
if("${RIC_GENERATED_E2SM_KPM_BINDING_DIR}" STREQUAL "")
  add_custom_command (
    OUTPUT ${E2SM_KPM_source}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2SM_KPM_C_DIR}" "${E2SM_KPM_ASN_FILE}" "E2SM_KPM_" -fno-include-deps -fincludes-quoted
    DEPENDS ${E2SM_KPM_ASN_FILE})
endif()
endif()

if(ENABLE_SLICER)
if("${RIC_GENERATED_E2SM_NEXRAN_BINDING_DIR}" STREQUAL "")
  set(E2SM_NEXRAN_ASN_FILE ${RIC_ASN_DIR}/e2sm-nexran-v01.00.asn1)
  set(E2SM_NEXRAN_C_DIR ${CMAKE_CURRENT_BINARY_DIR}/messages/E2SM-NEXRAN/)
  execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2SM_NEXRAN_C_DIR}" "${E2SM_NEXRAN_ASN_FILE}" "E2SM_NEXRAN_" -fno-include-deps -fincludes-quoted
    RESULT_VARIABLE ret)
  if (NOT ${ret} STREQUAL 0)
    message(FATAL_ERROR "${ret}: error")
  endif (NOT ${ret} STREQUAL 0)
else()
  set(E2SM_NEXRAN_C_DIR "${RIC_GENERATED_E2SM_NEXRAN_BINDING_DIR}")
endif()
file(GLOB E2SM_NEXRAN_source ${E2SM_NEXRAN_C_DIR}/*.c)
if("${RIC_GENERATED_E2SM_NEXRAN_BINDING_DIR}" STREQUAL "")
  add_custom_command (
    OUTPUT ${E2SM_NEXRAN_source}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2SM_NEXRAN_C_DIR}" "${E2SM_NEXRAN_ASN_FILE}" "E2SM_NEXRAN_" -fno-include-deps -fincludes-quoted
    DEPENDS ${E2SM_NEXRAN_ASN_FILE})
endif()
endif()

if(ENABLE_JSON)
if("${RIC_GENERATED_E2SM_JSON_BINDING_DIR}" STREQUAL "")
  set(E2SM_JSON_ASN_FILE ${RIC_ASN_DIR}/e2sm-json-v01.00.asn1)
  set(E2SM_JSON_C_DIR ${CMAKE_CURRENT_BINARY_DIR}/messages/E2SM-JSON/)
  execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2SM_JSON_C_DIR}" "${E2SM_JSON_ASN_FILE}" "E2SM_JSON_" -fno-include-deps -fincludes-quoted
    RESULT_VARIABLE ret)
  if (NOT ${ret} STREQUAL 0)
    message(FATAL_ERROR "${ret}: error")
  endif (NOT ${ret} STREQUAL 0)
else()
  set(E2SM_JSON_C_DIR "${RIC_GENERATED_E2SM_JSON_BINDING_DIR}")
endif()
file(GLOB E2SM_JSON_source ${E2SM_JSON_C_DIR}/*.c)
if("${RIC_GENERATED_E2SM_JSON_BINDING_DIR}" STREQUAL "")
  add_custom_command (
    OUTPUT ${E2SM_JSON_source}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_asn1c_includes.sh "${E2SM_JSON_C_DIR}" "${E2SM_JSON_ASN_FILE}" "E2SM_JSON_" -fno-include-deps -fincludes-quoted
    DEPENDS ${E2SM_JSON_ASN_FILE})
endif()
endif()

set(RIC_SOURCES
  ${E2AP_source}
  ${E2SM_GNB_NRT_source}
  e2ap_encode.cc
  e2ap_generate.cc
  e2ap_decode.cc
  e2ap_handle.cc
  timer_queue.cc
  e2sm.cc
  e2sm_gnb_nrt.cc
  agent.cc)

if(ENABLE_RIC_AGENT_KPM)
set(RIC_SOURCES
  ${RIC_SOURCES}
  ${E2SM_KPM_source}
  e2sm_kpm.cc)
endif()

if(ENABLE_SLICER)
set(RIC_SOURCES
  ${RIC_SOURCES}
  ${E2SM_NEXRAN_source}
  e2sm_nexran.cc)
endif()

if(ENABLE_JSON)
set(RIC_SOURCES
  ${RIC_SOURCES}
  ${E2SM_JSON_source}
  e2sm_json.cc)
endif()

add_library(srsenb_ric STATIC ${RIC_SOURCES})
target_include_directories(srsenb_ric BEFORE PUBLIC ${E2SM_GNB_NRT_C_DIR})
if(ENABLE_RIC_AGENT_KPM)
  target_include_directories(srsenb_ric BEFORE PUBLIC ${E2SM_KPM_C_DIR})
endif()
if(ENABLE_SLICER)
  target_include_directories(srsenb_ric BEFORE PUBLIC ${E2SM_NEXRAN_C_DIR})
endif()
if(ENABLE_JSON)
  target_include_directories(srsenb_ric BEFORE PUBLIC ${E2SM_JSON_C_DIR})
endif()
target_include_directories(srsenb_ric BEFORE PUBLIC ${E2AP_C_DIR})

The O-RAN srsLTE Agent
======================

To build the O-RAN srsLTE agent with `srsenb`, you will need to download
either the pre-generated O-RAN protocol bindings, or download the O-RAN
specifications in source form, so that you can generate the bindings
yourself with the `asn1c` compiler.  Generated bindings are not used
automatically; you must tell cmake about them when you build srslte.
Source specification files will be used, automatically, if they are
present, and not overridden by generated bindings arguments.  You must
have at least the E2AP core bindings; but the agent is useless without
at least some E2SM service model support.

Downloading the Generated Bindings
==================================

Grab these URLs, and extract each into
/tmp/srslte-ric/srsenb/src/ric/messages/generated (assuming your
checkout of this repo is in `/tmp/srslte-ric`).

    https://www.emulab.net/downloads/johnsond/profile-oai-oran/E2AP-generated-bindings.tar.gz
    https://www.emulab.net/downloads/johnsond/profile-oai-oran/E2SM-KPM-generated-bindings.tar.gz
    https://www.emulab.net/downloads/johnsond/profile-oai-oran/E2SM-NI-generated-bindings.tar.gz
    https://www.emulab.net/downloads/johnsond/profile-oai-oran/E2SM-GNB-NRT-generated-bindings.tar.gz

Then when you run `cmake` to build `srslte`, you will need to pass a few
additional variable definitions.  You must define these as absolute
paths, and not relative to the build tree.  Assuming your source
checkout is in `/tmp/srslte-ric` as above:

    cmake ../ \
        -DRIC_GENERATED_E2AP_BINDING_DIR=/tmp/srslte-ric/srsenb/src/ric/messages/generated/E2AP \
        -DRIC_GENERATED_E2SM_KPM_BINDING_DIR=/tmp/srslte-ric/srsenb/src/ric/messages/generated/E2SM-KPM \
        -DRIC_GENERATED_E2SM_NI_BINDING_DIR=/tmp/srslte-ric/srsenb/src/ric/messages/generated/E2SM-NI \
        -DRIC_GENERATED_E2SM_GNB_NRT_BINDING_DIR=/tmp/srslte-ric/srsenb/src/ric/messages/generated/E2SM-GNB-NRT

Building from Source Specifications
===================================

Read the instructions in messages/asn1/README.md .

Obtaining O-RAN WG3 E2 Specifications
=====================================

Before you can build the srsLTE O-RAN RIC agent against the source ASN.1
specifications, you will need to accept the O-RAN Alliance Adopter
License: https://www.o-ran.org/specifications .  Note that you cannot
currently redistribute the specifications.  You will also need a
modified version of the `asn1c` compiler (see ../../README.md).

Once you have an account, download all specifications in `.docx` form
from WG3: E2AP, E2SM-NI, and E2SM-KPM.  Place them in a temporary
directory on a machine that has the `srsLTE` repository.

For each .docx file, open and save as a plain .txt file with the same name.

You can manually extract the ASN.1 definitions from each document -- but
it's easier to use a tool from OpenAirInterface (OAI).  Download
https://gitlab.flux.utah.edu/powderrenewpublic/oai-ric/-/raw/develop-oran-ric/cmake_targets/tools/extract_asn1_from_spec.pl?inline=false
and save to `extract_asn1_from_spec.pl`.

Then run these commands (assuming your checkout of this source tree is
in `srslte-ric`):

extract_asn1_from_spec.pl \
    < ORAN-WG3.E2AP-v01.00.txt > srslte-ric/srsenb/src/ric/messages/asn1/e2ap-v01.00.asn1
extract_asn1_from_spec.pl \
    < ORAN-WG3.E2SM-NI-v01.00.txt > srslte-ric/srsenb/src/ric/messages/asn1/e2sm-ni-v01.00.asn1
extract_asn1_from_spec.pl \
    < ORAN-WG3.E2SM-KPM-v01.00.txt > srslte-ric/srsenb/src/ric/messages/asn1/e2sm-kpm-v01.00.asn1

At this point, the build will work on these .asn1 files and generate the
necessary stubs via the `asn1c` compiler.

Obtaining an `asn1c` Compiler
=============================

We've had best success with a version of `asn1c` as modified by
OpenAirInterface.  Try to install and use that first.

    git clone https://gitlab.eurecom.fr/oai/asn1c.git
    cd asn1c
    git checkout f12568d617dbf48497588f8e227d70388fa217c9
    autoreconf -iv
    ./configure 
    make
    make install

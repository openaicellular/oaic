==================================
Hardware and Software Requirements
==================================



Hardware Requirements:
======================

Integrated O-RAN Solution
-------------------------

O-RAN system:
~~~~~~~~~~~~~

* 1 computer with 10 Gbps NIC
* 1 SDR (X or N for 5G)

Each UE:
~~~~~~~~

* 1 computer with 10 Gbps NIC
* 1 SDR (X or N for 5G) 
        - e.g. X310 or N310 USRP

Software Requirements: 
=======================

OS and Packages
---------------

Operating System: Ubuntu 20.04 
Major Packages: Docker,Kubernetes, Helm


Software Framework Components:
------------------------------

Near Real Time RIC - based on ORAN-SC E release:

https://github.com/openaicellular/RIC-Deployment/tree/e_rel_xapp_onboarder_support

E2 Termination for Near Real Time RIC:

https://github.com/openaicellular/ric-plt-e2/tree/e-release

Cellular Stack - Based on Software Radio Systems RAN (srsRAN):

https://github.com/openaicellular/srsRAN-e2/tree/master

A1 Mediator:

https://github.com/openaicellular/ric-plt-a1


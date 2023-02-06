=======================================
srsRAN with E2 Agent Installation Guide
=======================================

.. note:: 

   Before you begin, please clone the parent `oaic <https://github.com/openaicellular/oaic>`_ directory as outlined in :ref:`Getting Started <gettingstarted>`.

srsRAN is a 4G/5G software radio suite developed by `SRS <http://www.srs.io)>`_. This is a modified version of `srsRAN 21.10 <https://github.com/srsran/srsRAN>`_ and `POWDER's E2 agent enabled srsLTE <https://gitlab.flux.utah.edu/powderrenewpublic/srslte-ric>`_. 

See the `srsRAN project pages <https://www.srsran.com>`_ for information, guides and project news.

The srsRAN suite includes:
  - srsUE - a full-stack SDR 4G/5G-NSA UE application (5G-SA coming soon)
  - srsENB - a full-stack SDR 4G/5G-NSA eNodeB application (5G-SA coming soon)
  - srsEPC - a light-weight 4G core network implementation with MME, HSS and S/P-GW

For application features, build instructions and user guides see the `srsRAN documentation <https://docs.srsran.com>`_.


For license details, see LICENSE file - *Needs to be modified.*

Pre-requisites
--------------

.. admonition:: TODO

   Create a system specification table for different cellular configurations

- System Requirements - 4 core CPU (3 - 5 GHz)
- Operating system - Ubuntu 20.04
- E2 Agent Integration - E2 Bindings, asn1c Compiler, O-RAN Specification documents(optional)
- Simulated 1 UE 1 eNB/gNB setup - ZeroMQ libraries, Single Host machine/VM
- USRP frontend - UHD version 4.1, At least two host machines/VMs
- Multiple simulated UE and eNB/gNB support : GNU Radio companion 3.9

Dependencies Installation
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash
    
    sudo apt-get install build-essential cmake libfftw3-dev libmbedtls-dev libboost-program-options-dev libconfig++-dev libsctp-dev libtool autoconf


ZeroMQ Installation
-------------------

srsRAN software suite includes virtual radios which uses the ZeroMQ networking library to transfer radio samples between applications. This approach is very useful for development, testing, debugging, CI/CD or for teaching and demonstrating. Natively, ZeroMQ with srsRAN supports only one eNB/gNB and one UE configuration but it can be extended to support multiple UEs using GNU Radio. Have a look at the tutorial to do this :ref:`here <multiple_ue>`.


Getting ZeroMQ development Libraries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

`ZeroMQ Installation Instructions <https://docs.srsran.com/en/latest/app_notes/source/zeromq/source/index.html>`_

**Package Installation**

.. code-block:: bash

    sudo apt-get install libzmq3-dev



.. _uhd_installation:

UHD 4.1 Installation
-------------------- 

.. warning::
   Make sure you don't have UHD already installed in your system. The system might not work if there are multiple versions of UHD

`Ettus UHD Binary Installation Manual`_

.. _Ettus UHD Binary Installation Manual: https://files.ettus.com/manual/page_install.html

Using package manager
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

    sudo add-apt-repository ppa:ettusresearch/uhd
    sudo apt-get update
    sudo apt-get install libuhd-dev libuhd4.1.0 uhd-host



asn1c Compiler Installation
---------------------------

We will be using the modified asn1c compiler (for RAN and CN) that is hosted by Open Air Interface (OAI)

.. code-block:: bash

    cd ../..
    sudo apt install libtool autoconf
    git clone https://gitlab.eurecom.fr/oai/asn1c.git
    cd asn1c
    git checkout velichkov_s1ap_plus_option_group
    autoreconf -iv
    ./configure
    make -j`nproc`
    sudo make install
    sudo ldconfig
    cd ..

srsRAN with E2 agent Installation
---------------------------------
 
Installation from Source
~~~~~~~~~~~~~~~~~~~~~~~~

.. warning::
   This step will have to be performed again if any of the above librarires change. For example, if UHD is updated, you will have to re-build srsRAN to make use of the updated library.

.. code-block:: bash

    cd srsRAN-e2
    mkdir build
    export SRS=`realpath .`
    cd build
    cmake ../ -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DRIC_GENERATED_E2AP_BINDING_DIR=${SRS}/e2_bindings/E2AP-v01.01 \
        -DRIC_GENERATED_E2SM_KPM_BINDING_DIR=${SRS}/e2_bindings/E2SM-KPM \
        -DRIC_GENERATED_E2SM_GNB_NRT_BINDING_DIR=${SRS}/e2_bindings/E2SM-GNB-NRT
    make -j`nproc`   
    sudo make install
    sudo ldconfig
    srsran_install_configs.sh user --force
    cd ../../

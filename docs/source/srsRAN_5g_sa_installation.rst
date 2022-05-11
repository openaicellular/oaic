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

.. code-block:: rst
    
    sudo apt-get install build-essential cmake libfftw3-dev libmbedtls-dev libboost-program-options-dev libconfig++-dev libsctp-dev libtool autoconf

Open5GS Installation & Configuration
------------------------------------

`Open5GS Overview and Installation <https://open5gs.org/open5gs/docs/guide/01-quickstart/>`_


Using Package Managers
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: rst
	
	sudo apt update
	sudo apt install software-properties-common
	sudo add-apt-repository ppa:open5gs/latest
	sudo apt update
	sudo apt install open5gs

WebUI of Open5GS
~~~~~~~~~~~~~~~~

The WebUI allows you to interactively edit subscriber data. A `commnand line tool <https://github.com/open5gs/open5gs/blob/main/misc/db/open5gs-dbctl>`_

.. code-block:: rst
	
	sudo apt update
	sudo apt install curl
	curl -fsSL https://deb.nodesource.com/setup_14.x | sudo -E bash -
	sudo apt install nodejs
	curl -fsSL https://open5gs.org/open5gs/assets/webui/install | sudo -E bash -

Configure Open5GS to run as a 5G Core
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We will need to enter subscriber data into your HSS/ UDR. You will also need to set some IP Table rules to bridge the PGWU/UPF to the WAN. 

You will need to modify your 5G AMF config to support your PLMN and TAC. The international test PLMN is 001/01, and the international private network PLMN is 999/99. You should stick to using either of these PLMNs unless you have been issued a PLMN by your national regulator. (This PLMN will need to be configured in your gNB).

If you are aiming to connect an external gNB to your core, you will also need to change the NGAP bind address of the AMF and the GTPU bind address of the UPF. If you are running an gNB stack locally, you will not need to make these changes.

Modify ``/etc/open5gs/amf.yaml`` to set the NGAP IP address, PLMN ID, TAC and NSSAI.

Replace ``/etc/open5gs/amf.yaml`` and ``/etc/open5gs/upf.yaml``	with the given ``amf.yaml`` and ``upf.yaml`` files in the ``/config_files/5g_sa_zmq`` directory.

.. code-block:: rst

	sudo rm /etc/open5gs/amf.yaml /etc/open5gs/upf.yaml
	sudo cp srsRAN/config_files/5g_sa_zmq/amf.yaml /etc/open5gs/
	sudo cp srsRAN/config_files/5g_sa_zmq/upf.yaml /etc/open5gs/

Restart the daemon service for ``AMF`` and ``UPF``.

.. code-block:: rst

	sudo systemctl restart open5gs-amfd
	sudo systemctl restart open5gs-upfd

Next, we have to enter the subscriber information using Open5GS WebGUI

Connect to http://localhost:3000 and login with admin account.

.. code-block:: rst 

	Username : admin
	Password : 1423
	

ZeroMQ Installation
-------------------

srsRAN software suite includes virtual radios which uses the ZeroMQ networking library to transfer radio samples between applications. This approach is very useful for development, testing, debugging, CI/CD or for teaching and demonstrating. Natively, ZeroMQ with srsRAN supports only one eNB/gNB and one UE configuration but it can be extended to support multiple UEs using GNU Radio. Have a look at the tutorial to do this :ref:`here <multiple_ue>`.


Getting ZeroMQ development Libraries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

`ZeroMQ Installation Instructions <https://docs.srsran.com/en/latest/app_notes/source/zeromq/source/index.html>`_

**Package Installation**

.. code-block:: rst

    sudo apt-get install libzmq3-dev



.. _uhd_installation:

UHD 4.1 Installation
-------------------- 

.. warning::
   Make sure you don't have UHD already installed in your system.

`Ettus UHD Binary Installation Manual`_

.. _Ettus UHD Binary Installation Manual: https://files.ettus.com/manual/page_install.html

Using package manager
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: rst

    sudo add-apt-repository ppa:ettusresearch/uhd
    sudo apt-get update
    sudo apt-get install libuhd-dev libuhd4.1.0 uhd-host



asn1c Compiler Installation
---------------------------

We will be using the modified asn1c compiler (for RAN and CN) that is hosted by Open Air Interface (OAI)

.. code-block:: rst

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

.. code-block:: rst

    https://github.com/srsran/srsRAN.git
    cd srsRAN
    mkdir build
    cd build
    cmake ../ 
    make -j`nproc`
    sudo make install
    sudo ldconfig
    srsran_install_configs.sh user
    cd ../../

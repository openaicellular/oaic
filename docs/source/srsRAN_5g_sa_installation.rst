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

    git clone https://github.com/srsran/srsRAN.git
    cd srsRAN
    mkdir build
    cd build
    cmake ../ 
    make -j`nproc`
    sudo make install
    sudo ldconfig
    srsran_install_configs.sh user
    cd ../../

Configure Open5GS to run as a 5G Core
-------------------------------------

We will need to enter subscriber data into your HSS/ UDR. You will also need to set some IP Table rules to bridge the PGWU/UPF to the WAN. 

You will need to modify your 5G AMF config to support your PLMN and TAC. The international test PLMN is 001/01, and the international private network PLMN is 999/99. You should stick to using either of these PLMNs unless you have been issued a PLMN by your national regulator. (This PLMN will need to be configured in your gNB).

If you are aiming to connect an external gNB to your core, you will also need to change the NGAP bind address of the AMF and the GTPU bind address of the UPF. If you are running an gNB stack locally, you will not need to make these changes.

Modify ``/etc/open5gs/amf.yaml`` to set the NGAP IP address, PLMN ID, TAC and NSSAI.

Replace ``/etc/open5gs/amf.yaml`` and ``/etc/open5gs/upf.yaml``	with the given ``amf.yaml`` and ``upf.yaml`` files in the ``/config_files/5g_sa_zmq`` directory.

.. code-block:: rst

	sudo rm /etc/open5gs/amf.yaml /etc/open5gs/upf.yaml
	sudo cp srsRAN/config_files/core_config_files/amf.yaml /etc/open5gs/
	sudo cp srsRAN/config_files/core_config_files/upf.yaml /etc/open5gs/

Restart the daemon service for ``AMF`` and ``UPF``.

.. code-block:: rst

	sudo systemctl restart open5gs-amfd
	sudo systemctl restart open5gs-upfd

Enter UE Information in the Open5GS Subscriber database
-------------------------------------------------------

If we are using zeroMQ, we need to create a namespace for the UE, otherwise the UE will not be recognized by the Core. 

.. code-block:: rst

	sudo ip netns add ue1
	sudo ip netns list

.. note::

	If using X310 Radio Frontend skip the above step.

Next, we have to enter the subscriber information using Open5GS WebGUI

Connect to http://localhost:3000 and login with admin account.

.. code-block:: rst 

	Username : admin
	Password : 1423

Once logged in, Click on Add Subcriber and fill in the values as given below. The rest of the options can be left unmodified.

.. note::
	
	The values for each field should match what is in the UE config file, under the [USIM] section. Make sure to correctly configure the APN, if this is not done correctly the UE will not be assigned 		an IP and won't connect to the internet. 

.. code-block:: rst

	IMSI: 901700123456789
	Subscriber Key (K): 00112233445566778899aabbccddeeff 
	USIM Type: OPc
	Operator Key (OPc): 63BFA50EE6523365FF14C1F45F88737D
	DNN/APN: srsapn 
	Type: IPv4v6
	Capability: Enabled

Save the above configuration and proceed to run the gNB and UE.


Setup Your Own 5G Network
=========================


Core
----

The core does not need to be started directly, as it will run in the background by default. srsENB will automatically connect to it on start-up.

.. tip::

	If any changes are made to Open5GS config files, it is important to restart the AMF and UPF daemons.


srsENB
------

.. tip::

	The Core and the gNB are considered to be on the same machine for this example (for both SDR and zMQ). To configure them to operate on separate machines, the eNB/gNB had to be given the proper IP 		address to connect to 	the core.	

.. note:: 

	Within the ``enb.conf`` file change the path of the ``sib_config``, ``rr_config`` and ``rb_config`` to reflect the current path of the respective config files.
	The RF gains depend on the channel conditions in your area and will have to be modified as required.

.. code-block:: rst

	sudo srsenb <path_to_enb_config_file>/enb.conf

If the srsENB connects successfully, the ``NG connection successful`` message will be displayed.

srsUE
-----

Now we can run srsUE similar to how we ran srsENB.

.. tip:: 

	If you are using SDRs, you will have to run UE in a separate machine. 
	With ZeroMQ, UE has to be run on the same machine as gNB/core.

.. code-block:: rst

	sudo srsue <path_to_ue_config_file>/ue.conf

Once the UE connects successfully to the network, the UE will be assigned an IP. This is seen in ``PDU Session Establishment successful. IP: 10.45.0.2``. The NR connection is then confirmed with the ``RRC NR reconfiguration successful`` message.

.. tip:: 

	The IP of the UE might change each time it re-connects to the network. So it is best practice to always double check the latest IP assigned by reading it from the console before running the traffic.


Testing the Network
===================

We outline testing the network through ``ping`` and ``iperf``.

PING
----

This is the simplest way to test the network. This will test whether or not the UE and core can successfully communicate.

Uplink
~~~~~~

If you are using zeroMQ, you will need to execute the ping command from the UE's network space

.. code-block:: rst

	sudo ip netns exec ue1 ping 10.45.0.1

If you are using SDRs, then we can directly run ping from a new terminal on the UE machine.

.. code-block:: rst

	sudo ping 10.45.0.1

Downlink
~~~~~~~~

For Downlink it does not matter on which machine the UE is running.

.. code-block:: rst

	sudo ping 10.45.0.2

.. tip::

	Check the IP address of the UE before issuing the ping command.

iPerf3
------

In this scenario, client will run on the UE side with the server on the network side (core). UDP traffic will be generated at 10Mbps for 60 seconds. It is important to start the server first, and then the client.

.. note::

	If using ZeroMQ, both the client and server should be run on the same machine but on different terminal windows.

Network Side
~~~~~~~~~~~~

.. code-block:: rst

	iperf3 -s -i 1

UE-Side
~~~~~~~

If using zeroMQ, the iperf client should be run from the UE's network namespace.

.. code-block:: rst

	sudo ip netns exec ue1 iperf3 -c 10.45.0.1 -b 10M -i 1 -t 60

If using SDRs, the iperf client can be run on the UE machine using,

.. code-block:: rst

	sudo iperf3 -c 10.45.0.1 -b 10M -i 1 -t 60












	



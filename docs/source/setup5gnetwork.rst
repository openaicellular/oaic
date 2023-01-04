.. _setup5Gnet:

=========================
Setup your own 5G Network
=========================

In this example, the EPC and en-gNB (NSA mode) and near-RT RIC are assumed to run on the same machine/VM (Machine 1). The UE, if using USRPs should be run on a different machine/VM (Machine 2). But if zeroMQ mode is used, a single machine suffices for all components. Refer to GNU radio integration for instructions to include multiple simulated UEs. 

Running the EPC
===============


Before we start the EPC, we need to create a separate network namespace for the UE since all components are running on the same machine. 

.. code-block:: rst

	sudo ip netns add ue1
	sudo ip netns list

Now, in a new command window on Machine 1 run srsRAN EPC:

.. code-block:: rst

        sudo srsepc

.. note::
	
	The EPC can be run on a different machine and the eNB can connect to it based on its IP address. This IP address has to be specified in the eNB config file.

Here we outline two options to bring up the en-gNB and the UE - one with  zeroMQ simulation and the other with USRP X310.

en-gNB and UE in ZeroMQ Mode
============================

Before we proceed further it would be worthwhile to open the logs of ``E2 Manager, E2 Termination, Subscription Manager and Application Manager`` to trace the flow of messages. The instructions to do this can be found in :ref:`Kubectl Commands <kubectl_commands>` section.

Running the en-gNB & connecting to near-RT RIC
----------------------------------------------

.. note::

	If you want an xApp to communicate with the RAN, make sure that the xApp is not already deployed on the near-RT RIC platform. 

.. Instructions to do this can be found in :ref:`Deploying example xApp <kpimon_deployment>` and :ref:`Genral guidelines to Deploy an xApp <xappdeployment>` sections.

In a new command window on Machine 1 run srsRAN en-gNB. 

But before we start the en-gNB, we need to get the current machine's IP address and the IP address of the E2 Termination service at the near-RT RIC.

.. code-block:: rst 
	
	export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
	export E2NODE_PORT=5006
	export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`


srsENB in ZeroMQ mode
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: rst

        sudo srsenb --enb.n_prb=50 --enb.name=enb1 --enb.enb_id=0x19B \
        --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101,id=enb,base_srate=23.04e6" \
        --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT}

Once the en-gNB is up and successfully connected to the near-RT RIC, you will see **E2 Setup** and **E2 Response** messages on the console. You will also see ``RIC Connection Initialized`` and ``RIC state established`` messages.


srsUE in ZeroMQ mode
--------------------

This command uses the default config file. The following message ``RRC NR reconfiguration successful`` confirms that the UE has connected to the NR cell. This will be used for the data link, while the LTE cell will be used for control messaging. On a new command terminal on the same machine run,
 

.. code-block:: rst

	sudo srsue --gw.netns=ue1

Once the UE connects successfully to the network, the UE will be assigned an IP. 

.. tip::

	You can find configuration files and database file related to ‘ue1’, ‘enb’, ‘epc’ in /home/.config/srsran directory. If you want to change the contents of the ‘ue.config’ and ‘user_db.csv’ files 	such as IMSI and other values, make sure to first kill the EPC process, make modifications and then restart EPC. The EPC process updates this file when it exits.


Exchanging Traffic
------------------

We outline testing the network through ``ping`` and ``iperf``.

PING
~~~~

This is the simplest way to test the network. This will test whether or not the UE and core can successfully communicate.

Uplink
******

When using zeroMQ, the ping command should be executed on a new terminal from the UE's network space

.. code-block:: rst

	sudo ip netns exec ue1 ping 172.16.0.1


Downlink
********

For Downlink, on a new terminal run

.. code-block:: rst

	sudo ping <ue_ip>

.. tip::

	Check the IP address of the UE before issuing the ping command.

iPerf3
~~~~~~

In this scenario, client will run on the UE side with the server on the network side (core). UDP traffic will be generated at 10Mbps for 60 seconds. It is important to start the server first, and then the client.

.. note::

	Both the client and server should be run on the same machine but on different terminal windows.

Network Side
************

.. code-block:: rst

	iperf3 -s -i 1

UE-Side
*******

Again, since we are using zeroMQ, the iperf client should be run from the UE's network namespace.


.. code-block:: rst

	sudo ip netns exec ue1 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 60

en-gNB and UE in USRP Mode
==========================


en-gNB in UHD USRP (X310) Mode
------------------------------

The srsENB should be run on the same machine as the EPC.

In a new command window on Machine 1 run srsRAN en-gNB. 

Before we start the en-gNB, we need to get the current machine's IP address and the IP address of the E2 Termination service at the near-RT RIC.

.. code-block:: rst 
	
	export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
	export E2NODE_PORT=5006
	export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`


.. code-block:: rst


        sudo srsenb --enb.name=enb1 --enb.enb_id=0x19B \
        --rf.device_name=uhd --rf.device_args="type=x300,clock=internal,sampling_rate=11.52e6,lo_freq_offset_hz=23.04e6,send_frame_size=8000,recv_frame_size=8000,num_send_frames=64,num_recv_frames=64,None" \
        --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT}


.. tip::

	If the near-RT RIC is hosted on a separate machine, we can input the IP address of the machine as the E2 Termination's IP. The IP address of the machine can be found by running ``ifconfig``


Running the srsUE
-----------------

For this mode, the UE should run on Machine 2 (can be a VM).

.. code-block:: rst

	sudo srsue --rf.device_name=uhd --rf.device_args="type=x300,clock=internal,sampling_rate=11.52e6,lo_freq_offset_hz=23.04e6,send_frame_size=8000,recv_frame_size=8000,num_send_frames=64,num_recv_frames=64,None" \
	--rf.srate=11.52e6 --rrc.release=15 --rat.nr.bands=3 --rat.nr.nof_carriers=1

.. tip:: 

	If youhave multiple USRPs with different IP addresses connected to a single machine, and you want to indicate particular USRPs to be used, you can do so by adding ``addr=<ip_aadress_of_usrp>`` to the ``rf.device_args`` argument. 


Testing the Network
-------------------

We outline testing the network through ``ping`` and ``iperf``.

PING
~~~~

This is the simplest way to test the network. This will test whether or not the UE and core can successfully communicate.

Uplink
******


When using SDRs, we can directly run ping from a new terminal on the UE machine.

.. code-block:: rst

	sudo ping 172.16.0.1

Downlink
********

For Downlink, on the machine running EPC, run the following command on a new terminal.

.. code-block:: rst

	sudo ping <ue_ip_addr>

.. tip::

	Check the IP address of the UE before issuing the ping command.

iPerf3
~~~~~~

In this scenario, client will run on the UE side with the server on the network side (core). UDP traffic will be generated at 10Mbps for 60 seconds. It is important to start the server first, and then the client.


Network Side
************

.. code-block:: rst

	sudo iperf3 -s -i 1

UE-Side
*******


If using SDRs, the iperf client can be run on the UE machine using on a new terminal,

.. code-block:: rst

	sudo iperf3 -c 172.16.0.1 -b 10M -i 1 -t 60



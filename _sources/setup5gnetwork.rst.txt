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

en-gNB and UE in ZeroMQ Mode
============================


Running the en-gNB & connecting to near-RT RIC
----------------------------------------------

.. note::

	If you want an xApp to communicate with the RAN, make sure that the xApp is already deployed on the near-RT RIC platform. Instructions to do this can be found in :ref:`Deploying example xApp <kpimon_deployment>` and :ref:`Genral guidelines to Deploy an xApp <xappdeployment>`sections.

In a new command window on Machine 1 run srsRAN en-gNB. Here we outline two options - one with USRP X310 and the other with zeroMQ simulation.

But before we start the en-gNB, we need to get the current machine's IP address and the IP address of the E2 Termination service at the near-RT RIC.

.. code-block::rst 
	
	export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
	export E2NODE_PORT=5006
	export E2TERM_IP=`kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`

srsENB in ZeroMQ mode
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: rst


        sudo srsenb --enb.n_prb=15 --enb.name=enb1 --enb.enb_id=0x19B \
        --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101,id=enb,base_srate=23.04e6" \
        --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT}

Once the en-gNB is up and successfully connected to the near-RT RIC, you will see **E2 Setup** and **E2 Response** messages on the console. You will also see ``RIC Connection Initialized`` and ``RIC state established`` messages.

srsUE in ZeroMQ mode
--------------------

This command uses the default config file 
.. code-block:: rst

    sudo srsue

Exchanging traffic
------------------


en-gNB in UHD USRP (X310) Mode
==============================

.. code-block:: rst


        sudo srsenb --enb.name=enb1 --enb.enb_id=0x19B \
        --rf.device_name=uhd --rf.device_args="type=x300,clock=internal,sampling_rate=11.52e6,lo_freq_offset_hz=23.04e6,send_frame_size=8000,recv_frame_size=8000,num_send_frames=64,num_recv_frames=64,None" \
        --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT}


.. tip::

	If the near-RT RIC is hosted on a separate machine, we can input the IP address of the machine as the E2 Termination's IP. The IP address of the machine can be found by running ``ifconfig``

Running the srsUE
=================



.. tip::

	You can find configuration files and database file related to ‘ue1’, ‘enb’, ‘epc’ in /home/.config/srsran directory. If you want to change the contents of the ‘ue.config’ and ‘user_db.csv’ files 	such as IMSI and other values, make sure to first kill the EPC process, make modifications and then restart EPC. The EPC process updates this file when it exits.

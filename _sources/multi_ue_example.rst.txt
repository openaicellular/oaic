==================================
Multiple UEs with ZMQ Example
==================================

This example uses a gnuradio flowgraph (broker) to establish the connection between 5 UEs.

Below is a flowgraph for gnuradio that makes the connection among the uplink and downlink channels for each UE. You will need the free and open source software Gnu Radio which can be obtained on Ubuntu by running:


.. code-block:: bash

   sudo apt install gnuradio 


Please download this file to be run later in the example: 
https://github.com/openaicellular/main-file-repo/blob/master/OAIC_FIVE_UEs.grc



Setup 
======


edit user_db.csv in /rrot/.config/srsran, with the following: 

.. code-block:: bash


    ue2,xor,001010123456780,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,000000001590,7,dynamic
    ue3,xor,001010123456781,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8002,000000001488,7,dynamic
    ue4,xor,001010123456782,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8003,000000001446,7,dynamic
    ue5,xor,001010123456783,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8004,000000001467,7,dynamic
    ue1,xor,001010123456789,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,9001,000000001656,7,dynamic


In a terminal run the following commands for each UE

.. code-block:: bash

    sudo ip netns add ue1
    sudo ip netns add ue2
    sudo ip netns add ue3
    sudo ip netns add ue4
    sudo ip netns add ue5


Set the environment variables

.. code-block:: bash

    export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
    export E2NODE_PORT=5006
    export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`



Running the example 
===================


Terminal 1

.. code-block:: bash

    sudo srsepc

Terminal 2

.. code-block:: bash

    sudo srsenb --enb.n_prb=50 --enb.name=enb1 --enb.enb_id=0x19B --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2009,tx_port1=tcp://*:2200,rx_port1=tcp://localhost:2201,id=enb,base_srate=23.04e6" --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT}

Terminal 3

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2010,rx_port=tcp://localhost:2008,id=ue1,base_srate=23.04e6" --gw.netns=ue1 --usim.algo=xor --usim.imsi=001010123456789

Terminal 4

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2007,rx_port=tcp://localhost:2006,id=ue2,base_srate=23.04e6" --gw.netns=ue2 --usim.algo=xor --usim.imsi=001010123456780

Terminal 5

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2301,rx_port=tcp://localhost:2300,id=ue3,base_srate=23.04e6" --gw.netns=ue3 --usim.algo=xor --usim.imsi=001010123456781

Terminal 6

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2401,rx_port=tcp://localhost:2400,id=ue4,base_srate=23.04e6" --gw.netns=ue4 --usim.algo=xor --usim.imsi=001010123456782

Terminal 7

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2501,rx_port=tcp://localhost:2500,id=ue5,base_srate=23.04e6" --gw.netns=ue5 --usim.algo=xor --usim.imsi=001010123456783


Open gnuradio companion and run the provided grc file by opening the file and pressing the play button.

Ping and Iperf test can now be performed to measure performance or generate traffic to each connected UE.

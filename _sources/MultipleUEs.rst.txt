=================
Multiples UEs ZMQ
=================

Before you begin, git clone the main-file-repo repository. This should provide the files for running the multiple UEs

.. code-block:: bash

    cd ~/
    git clone https://github.com/openaicellular/main-file-repo

Instructions for setting a 4G Network with Multiple UEs
=======================================================

1. Edit Configurations
----------------------

We need to change some settings in the srsRan config files to use ZMQ for the multiple UEs. Access the command line as superuser:

.. code-block:: bash

    sudo -i

Open ``rr.conf`` configuration file with the following command:

.. code-block:: bash

    vim /root/.config/srsran/rr.conf

Make sure the following lines of text are commented with ``//``:

.. code-block:: none

    nr_cell_list =
    (
        //   {
        //    rf_port = 1;
        //    cell_id = 0x02;
        //    tac = 0x0007;
        //    pci = 500;
        //    root_seq_idx = 204;

        // TDD:
        //dl_arfcn = 634240;
        //band = 78;

        // FDD:
        //    dl_arfcn = 368500;
        //    band = 3;
        //  }
    );

Save and close the file. Then open ``ue.conf`` configuration file with the following command:

.. code-block:: bash

    vim .config/srsran/ue.conf

Comment out the ``[rat.eutra]`` section:

.. code-block:: none

    #[rat.eutra]
    #dl_earfcn = 3350

Comment out Lte release under the ``[rrc]`` section:

.. code-block:: none

    #release = 15

Make sure ``[rat.nr]`` section is commented out as well:

.. code-block:: none

    #[rat.nr]
    #bands = 3,78
    #nof_carriers = 1

Save and close the file. Then open ``enb.conf`` configuration file with the following command:

.. code-block:: bash

    vim .config/srsran/enb.conf

Make sure ``device_name`` and ``device_args`` are commented out:

.. code-block:: none

    #device_name = zmq
    #device_args = fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101,id=enb,base_srate=23.04e6

Finally, we exit the root:

.. code-block:: bash

    exit

2. Start a multi-UE network
---------------------------

Here, we use two UEs as an example.

Terminal 1: Add two UEs:

.. code-block:: bash

    sudo ip netns add ue1
    sudo ip netns add ue2

Check the results:

.. code-block:: bash

    sudo ip netns list

If two UEs are listed, start the Core Network:

.. code-block:: bash

    sudo srsepc

Terminal 2: Set up Environment Variables and Base Station:

.. code-block:: bash

    export E2NODE_IP=`hostname -I | cut -f1 -d' '`
    export E2NODE_PORT=5006
    export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`

    sudo srsenb --enb.n_prb=100 --enb.name=enb1 --enb.enb_id=0x19B --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2000,rx_port=tcp://localhost:2009,id=enb,base_srate=23.04e6" --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT}

Terminal 3: Set up the first UE:

.. code-block:: bash

    sudo srsue \
      --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2010,rx_port=tcp://localhost:2008,id=ue,base_srate=23.04e6" \
    --usim.algo=xor --usim.imsi=001010123456789 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 \
    --log.all_level=warn --log.filename=stdout --gw.netns=ue1

Terminal 4: Set up the second UE:

.. code-block:: bash

    sudo srsue \
      --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2007,rx_port=tcp://localhost:2006,id=ue,base_srate=23.04e6" \
    --usim.algo=xor --usim.imsi=001010123456780 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 \
    --log.all_level=warn --log.filename=stdout --gw.netns=ue2

Terminal 5: Attach the two UEs to eNB:

.. code-block:: bash

    cd ~/main-file-repo
    python3 2UES_4G.py

Generate traffic by iperf3:
In Terminal 6 and 7:

.. code-block:: bash

    iperf3 -s -i 1 -p 5201
    iperf3 -s -i 1 -p 5301

In Terminal 8 and 9:

.. code-block:: bash

    sudo ip netns exec ue1 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 600 -p 5201
    sudo ip netns exec ue2 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 600 -p 5301
    
-----
    
Instructions for setting a 5G Network with Multiple UEs
========================================================

1. Setup
-----------------------------
Access the command line as superuser:

.. code-block:: bash

    sudo -i
    
Open user_db.csv file with the following command:

.. code-block:: bash

    vim /root/.config/srsran/user_db.csv
	
edit the file with the following:

.. code-block:: none

    ue2,xor,001010123456780,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,000000001590,7,dynamic
    ue3,xor,001010123456781,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8002,000000001488,7,dynamic
    ue4,xor,001010123456782,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8003,000000001446,7,dynamic
    ue5,xor,001010123456783,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8004,000000001467,7,dynamic
    ue1,xor,001010123456789,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,9001,000000001656,7,dynamic

Terminal 1:open a terminal run the following commands for each UE:

.. code-block:: bash

    sudo ip netns add ue1
    sudo ip netns add ue2
    sudo ip netns add ue3
    sudo ip netns add ue4
    sudo ip netns add ue5
    
    
Check the results:

.. code-block:: bash

    sudo ip netns list

If five UEs are listed, start the Core Network:

.. code-block:: bash

    sudo srsepc

Terminal 2: Set up Environment Variables and Base Station:

.. code-block:: bash

    export E2NODE_IP=`hostname -I | cut -f1 -d' '`
    export E2NODE_PORT=5006
    export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`

    sudo srsenb --enb.n_prb=50 --enb.name=enb1 --enb.enb_id=0x19B --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101,id=enb,base_srate=23.04e6" --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT}

Terminal 3: Set up the first UE:

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2002,rx_port0=tcp://localhost:2052,tx_port1=tcp://*:2102,rx_port1=tcp://localhost:2152,id=ue1,base_srate=23.04e6" --gw.netns=ue1 --usim.algo=xor --usim.imsi=001010123456789

Terminal 4: Set up the second UE:

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2003,rx_port0=tcp://localhost:2053,tx_port1=tcp://*:2103,rx_port1=tcp://localhost:2153,id=ue2,base_srate=23.04e6" --gw.netns=ue2 --usim.algo=xor --usim.imsi=001010123456780
    
Terminal 5: Set up the third UE:

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2004,rx_port0=tcp://localhost:2054,tx_port1=tcp://*:2104,rx_port1=tcp://localhost:2154,id=ue3,base_srate=23.04e6" --gw.netns=ue3 --usim.algo=xor --usim.imsi=001010123456781
    
Terminal 6: Set up the fourth UE:

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2005,rx_port0=tcp://localhost:2055,tx_port1=tcp://*:2105,rx_port1=tcp://localhost:2155,id=ue4,base_srate=23.04e6" --gw.netns=ue4 --usim.algo=xor --usim.imsi=001010123456782
    
Terminal 7: Set up the fifth UE:

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2006,rx_port0=tcp://localhost:2056,tx_port1=tcp://*:2106,rx_port1=tcp://localhost:2156,id=ue5,base_srate=23.04e6" --gw.netns=ue5 --usim.algo=xor --usim.imsi=001010123456783
    
    
Terminal 8: Attach the five UEs to eNB:

.. code-block:: bash

    cd ~/main-file-repo
    python3 5UES_5g.py

Generate traffic by iperf3: 
In Terminal 9, 10, 11, 12, and 13:

.. code-block:: bash

    iperf3 -s -i 1 -p 5201
    iperf3 -s -i 1 -p 5301
    iperf3 -s -i 1 -p 5401
    iperf3 -s -i 1 -p 5501
    iperf3 -s -i 1 -p 5601

In Terminal 14, 15, 16, 17, 18:

.. code-block:: bash

    sudo ip netns exec ue1 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 600 -p 5201
    sudo ip netns exec ue2 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 600 -p 5301
    sudo ip netns exec ue2 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 600 -p 5401
    sudo ip netns exec ue2 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 600 -p 5501
    sudo ip netns exec ue2 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 600 -p 5601
        


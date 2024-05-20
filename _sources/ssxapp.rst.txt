Running the SS Xapp 
===================

Setup 
=====

1. Install Near-Real Time RIC

    Follow instructions on the OAIC documentation: https://openaicellular.github.io/oaic/oran_installation.html

2. Next install all the required dependencies, ZMQ, UHD and asn1c Compiler

    Follow instructions on OAIC documentation for installation of dependencies, ZMQ and as1nc compiler: https://openaicellular.github.io/oaic/srsRAN_installation.html

    You will need to build UHD-3.15-LTS from source. Follow instructions on: https://files.ettus.com/manual/page_build_guide.html

3. Clone down the srslte modified with e2

.. code-block:: bash

    cd oaic
    git clone https://github.com/openaicellular/srslte-e2
    cd srslte-e2

.. code-block:: bash
    
    rm -rf build
    mkdir build
    export SRS=`realpath .`
    cd build
    cmake ../ -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DRIC_GENERATED_E2AP_BINDING_DIR=${SRS}/e2_bindings/E2AP-v01.01 \
        -DRIC_GENERATED_E2SM_KPM_BINDING_DIR=${SRS}/e2_bindings/E2SM-KPM \
        -DRIC_GENERATED_E2SM_GNB_NRT_BINDING_DIR=${SRS}/e2_bindings/E2SM-GNB-NRT
    make -j`nproc`
    make test
    sudo make install
    sudo ldconfig
    sudo srslte_install_configs.sh user --force
    cd ../../

We can now clone the repositories we will need to run build the xapp and onboard it as well as run multiple UEs.

.. warning::
    Make sure you open a new terminal before you run these following commands. This prevents you from downloading anything under the root directory.

Under the oaic directory, run the following commands

.. code-block:: bash

    git clone https://github.com/openaicellular/ss-xapp.git

Now we are going to build the xapp from the dockerfile

Before deploying the xApp, we need to host the config file (xApp descriptor) provided in the web server we have created already. 
Follow the instructions to create a Web server from here:  https://openaicellular.github.io/oaic/xapp_deployment.html

You need to follow Configuring the Nginx Server and Hosting Config Files portion of the documentation. The config file is located within the repository you just cloned.

.. code-block:: bash

    cd ~/oaic/ss-xapp  
    sudo docker build . -t xApp-registry.local:5008/ss:0.1.0

Paste the following in the ``ss-xapp-onboard.url`` file. Substitute the ``<machine_ip_addr>`` with the IP address of your machine. You can find this out through ``ifconfig``.

.. code-block:: bash

    {"config-file.json_url":"http://<machine_ip_addr>:5010/config_files/config-file.json"}
    
Running the SS xApp
===================

.. warning::

    If you already have SS xApp deployed on your system, you need to restart the pod using the command below before running the rest of the commands. If you are freshly deploying the xApp, you can skip this step.

.. code-block:: bash

    sudo kubectl -n ricxapp rollout restart deployment ricxapp-ss


Starting the network with ZMQ
-----------------------------

Terminal 1: Start the Core Network/Add Ues to Network Namespace

.. code-block:: bash

    sudo ip netns add ue1
    sudo ip netns add ue2
    sudo ip netns add ue3
    sudo ip netns list    
    sudo srsepc 

Terminal 2: Set up Environment Variables and Base Station

.. code-block:: bash

    export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
    export E2NODE_PORT=5006
    export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`
    
.. code-block:: bash
       
    sudo srsenb --enb.n_prb=100 --enb.name=enb1 --enb.enb_id=0x19B \
    --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2000,rx_port=tcp://localhost:2009,id=enb,base_srate=23.04e6" --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT} --slicer.enable=1 --slicer.workshare=0

Terminal 3: Set up the first UE

.. code-block:: bash

    sudo srsue \
    --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2010,rx_port=tcp://localhost:2300,id=ue,base_srate=23.04e6" --usim.algo=xor --usim.imsi=001010123456789 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 --log.all_level=warn --log.filename=stdout --gw.netns=ue1

Terminal 4: Set up the second UE

.. code-block:: bash

    sudo srsue \
    --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2007,rx_port=tcp://localhost:2400,id=ue,base_srate=23.04e6" --usim.algo=xor --usim.imsi=001010123456780 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 --log.all_level=warn --log.filename=stdout --gw.netns=ue2
    
Terminal 5: Set up the second UE

.. code-block:: bash

    sudo srsue \
    --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2008,rx_port=tcp://localhost:2500,id=ue,base_srate=23.04e6" --usim.algo=xor --usim.imsi=001010123456781 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 --log.all_level=warn --log.filename=stdout --gw.netns=ue3
    
Terminal 6: Start the gnuradio flowgraph

.. code-block:: bash

    python3 multi_ue.py

Terminal 6 & 7: Set up iperf3 test on the server side

.. code-block:: bash
   
   iperf3 -s -B 172.16.0.1 -p 5006 -i 1
   iperf3 -s -B 172.16.0.1 -p 5020 -i 1 
   iperf3 -s -B 172.16.0.1 -p 5020 -i 1

Terminal 8 & 9: Set up iperf3 test on the client side

We add an additional bandwidth argument "-b xxM" on each iperf3 test on client side to create a scenario of UEs trying to access more or less of resources on the network. If a UE surpasses the pre-determined threshold for amount of data packets transmitted, it is considered as malicious by the SS xApp.

.. code-block:: bash

   sudo ip netns exec ue1 iperf3 -c 172.16.0.1 -p 5006 -i 1 -t 36000 -R -b 40M
   sudo ip netns exec ue2 iperf3 -c 172.16.0.1 -p 5020 -i 1 -t 36000 -R -b 10M
   sudo ip netns exec ue2 iperf3 -c 172.16.0.1 -p 5020 -i 1 -t 36000 -R -b 15M

You should notice traffic flow on both the server and client side for both UEs. Move on to deploying the xApp.

Starting the network with USRPs
-------------------------------

For better performance of USRPs, we use the CDA-2990 Octaclock as an external clock reference source for the B210s. To ensure that the B210s recognize the source, we have to add an extra argument "clock=external" on the eNodeB and UE commands.

.. image:: b210octaclock.png
    :width: 80%
    :alt: B210 with Octaclock Setup

Before starting the network, check to see if each device recognizes the USRPs.

.. code-block:: bash

    uhd_find_devices

Terminal 1: Start the Core Network on server side (eNodeB server)

.. code-block:: bash
  
    sudo srsepc 

Terminal 2: Set up Environment Variables and Base Station

.. code-block:: bash

    export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
    export E2NODE_PORT=5006
    export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`
    
.. code-block:: bash
       
    sudo srsenb --enb.n_prb=100 --enb.name=enb1 --enb.enb_id=0x19B --rf.device_name=uhd --rf.device_args="clock=external" --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT} --slicer.enable=1 --slicer.workshare=0

Terminal 3: Set up the first UE

.. code-block:: bash
    
    sudo srsue \
    --rf.device_name=uhd --rf.device_args="clock=external" --usim.algo=xor --usim.imsi=001010123456789 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310  --log.all_level=warn --log.filename=stdout

Terminal 4: Set up the second UE

.. code-block:: bash

    sudo srsue \
    --rf.device_name=uhd --rf.device_args="clock=external" --usim.algo=xor --usim.imsi=001010123456780 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310  --log.all_level=warn --log.filename=stdout
    
Terminal 5: Set up the third UE

.. code-block:: bash

    sudo srsue \
    --rf.device_name=uhd --rf.device_args="clock=external" --usim.algo=xor --usim.imsi=001010123456781 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310  --log.all_level=warn --log.filename=stdout

Terminal 6 & 7: Set up iperf3 test on the server side (eNodeB server)

.. code-block:: bash
   
   iperf3 -s -B 172.16.0.1 -p 5006 -i 1
   iperf3 -s -B 172.16.0.1 -p 5020 -i 1 
   iperf3 -s -B 172.16.0.1 -p 5020 -i 1

Terminal 8 & 9: Set up iperf3 test on the client side (UE servers)

We add an additional bandwidth argument "-b xxM" on each iperf3 test on client side to create a scenario of UEs trying to access more or less of resources on the network. If a UE surpasses the pre-determined threshold for amount of data packets transmitted, it is considered as malicious by the SS xApp.

.. code-block:: bash

   sudo iperf3 -c 172.16.0.1 -p 5006 -i 1 -t 36000 -R -b 40M
   sudo iperf3 -c 172.16.0.1 -p 5020 -i 1 -t 36000 -R -b 10M
   sudo iperf3 -c 172.16.0.1 -p 5020 -i 1 -t 36000 -R -b 15M

You should notice traffic flow on both the server and client side for both UEs. Move on to the next step.


Deploying the xApp
==================

Terminal 10

.. code-block:: bash
    
    cd ss-xapp
    export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
    export E2MGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2mgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export E2TERM_SCTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`
    export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export RTMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-rtmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`

Deploying the xApp
------------------

.. code-block:: bash

    curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@ss-xapp-onboard.url"
    curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"
    curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "ss"}'

Add another terminal to print the logs

.. code-block:: bash

    sudo kubectl logs -f -n ricxapp -l app=ricxapp-ss

.. warning::
    Before running the rest of the commands, detach two of the terminals with the iperf3 test running for 2 UEs to observe the downlink traffic.
    Also, detach the terminal with the logs.

Now run the test script with the following commands. You have to access the test script through the root directory to execute the commands in the script. The test script has commands for creating NodeB, UEs, and slices within the xApp, as well as binding the UEs to the slices. The xApp runs it's authentication mechanism for identifying authorized UEs during the creation of UEs.

.. code-block:: bash

    chmod +x zmqthreeue.sh
    ./zmqthreeue.sh

After a short time you can observe through the logs that UE1 will be considered malicious and moved to a different slice. You also observe the traffic exchange for UE1 will significantly decrease. 
		


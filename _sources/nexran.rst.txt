=======================
Running the Nexran Xapp 
=======================

Overview
========

Powder’s Nexran xapp implements ran slicing by sending instructions to RAN nodes, that performs custom resource allocation and we bind UE’s to those slices of allocated resources.

The xapp has a southbound interface to the RIC where it can send commands down to the nodes and a northbound interface where high level commands can be issued by the user.

.. image:: nexran_xapp_diagram.png
   :width: 80%
   :alt: Nexran Xapp diagram




Setup 
=====

First we have to turn on some extra functionality in srsRan and recompile. To do this, follow the steps below:

.. code-block:: bash

    cd srsRAN-e2
    vim CMakeLists.txt

Make sure the following is set to on:

.. code-block:: bash

    option(ENABLE_RIC_AGENT_KPM    "Enable RIC E2 Agent KPM metrics"  ON)
    option(ENABLE_RIC_AGENT_NEXRAN "Enable RIC E2 Agent NexRAN model" ON)
    option(ENABLE_SLICER           "Enable NexRAN Slicer"             ON)

Save and close the txt file then follow the steps below:

.. code-block:: bash

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
    sudo srsran_install_configs.sh user --force
    cd ../../

Editing Configurations
----------------------

We need to change some settings in the srsRan config files to use ZMQ in 4G

Access the command line as superuser:

.. code-block:: bash

    sudo -i
    
Open rr.conf configuration file with the following command:

.. code-block:: bash
    
    vim .config/srsran/rr.conf

Make sure the following lines of text is commented with “// ”

.. code-block:: bash

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

Save and close the file. Then open ue.conf configuration file with the following command:

.. code-block:: bash

    vim .config/srsran/ue.conf
    
Comment out the [rat.eutra] section
  
.. code-block:: bash
  
    #[rat.eutra]
    #dl_earfcn = 3350

Comment out Lte release under the [rrc] section:

.. code-block:: bash

    #release           = 15

Make sure [rat.nr] section is commented out as well:

.. code-block:: bash

    #[rat.nr]
    #bands = 3,78
    #nof_carriers = 1

Save and close the file. Then open enb.conf configuration file with the following command:

.. code-block:: bash

    vim .config/srsran/enb.conf
    
Make sure device_name and device_args is commented out:

.. code-block:: bash

    #device_name = zmq
    #device_args = fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101,id=enb,base_srate=23.04e6


We can now clone the repositories we will need to run build the xapp and onboard it as well as run multiple ues

.. warning::
    Make sure you open a new terminal before you run these following commands. This prevents you from downloading anything under the root directory

Under the oaic directory, run the following commands

.. code-block:: bash

    git clone https://github.com/openaicellular/main-file-repo.git
    git clone https://github.com/openaicellular/nexran.git


We need to change the ip address in the config file to match the local machine

First get the ip address of your machine

.. code-block:: bash

    echo `hostname  -I | cut -f1 -d' '`

Now update the config file

.. code-block:: bash

    cd nexran/
    git checkout e2ap-v1
    cd etc/
    vim nexran-onboard.url

Replace the highlighted with that address

.. code-block:: bash

    {"config-file.json_url":"http://<machine_ip_address>:5010/config_files/config-file.json"}
    
Configuring the Nginx Web server
--------------------------------

First, we need to install Nginx and check if it is in active (running) state.

.. code-block:: bash

    sudo apt-get install nginx
    sudo systemctl status nginx
 
Unlink the default Configuration file and check if it is unlinked

.. code-block:: bash

    cd /etc/nginx/sites-enabled
    sudo unlink default
    cd ../
 
Now we create some directories which can be accessed by the server and where the config files can be hosted

.. code-block:: bash

    cd ../../var/www
    sudo mkdir xApp_config.local
    cd xApp_config.local/
    sudo mkdir config_files
 
Create a Custom Configuration File and define file locations

.. code-block:: bash

    cd ../../../etc/nginx/conf.d
    sudo vim xApp_config.local.conf
 
Paste the following content in the conf file.

.. code-block:: bash

    server {
        listen 5010 default_server;
        server_name xApp_config.local;
        location /config_files/ {

            root /var/www/xApp_config.local/;
        }

    }
 
Save the configuration file and check if there are any errors in the configuration file.

.. code-block:: bash

    nginx -t
 
Hosting the config Files
------------------------

Copy the xApp config file to this directory. Reload Nginx once this has been done

.. code-block:: bash

    sudo cp <path_to_config_file>/config-file.json /var/www/xApp_config.local/config_files/
    sudo systemctl reload nginx
 
Now, you can check if the config file can be accessed from the newly created server. Place all files you want to host in the config_files directory

.. code-block:: bash

    curl http://<machine_ip_addr>:5010/config_files/config-file.json
 
Now we are going to build the xapp from the dockerfile and onboard it

.. code-block:: bash

    cd ~/oaic/nexran
    sudo docker build . -t xApp-registry.local:5008/nexran:0.1.0

xApp Onboarder Deployment
=========================

Get Variables
-------------

.. code-block:: bash

    export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
    export E2MGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2mgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export E2TERM_SCTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`
    export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export RTMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-rtmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`



In a terminal run the following commands for each UE

.. code-block:: bash

    sudo ip netns add ue1
    sudo ip netns add ue2

Running the xApp
================

Terminal 1: Start the Core Network

.. code-block:: bash

    sudo srsepc --spgw.sgi_if_addr=192.168.0.1
    
Terminal 2: Set up Environment Variables and Base Station

.. code-block:: bash

    export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
    export E2NODE_PORT=5006
    export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`

.. code-block:: bash

    sudo srsenb --enb.n_prb=50 --enb.name=enb1 --enb.enb_id=0x19B --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2009,tx_port1=tcp://*:2200,rx_port1=tcp://localhost:2201,id=enb,base_srate=23.04e6" --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT} --slicer.enable=1 --slicer.workshare=0

Terminal 3: Set up the first UE

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2010,rx_port=tcp://localhost:2008,id=ue1,base_srate=23.04e6" --gw.netns=ue1 --usim.algo=xor --usim.imsi=001010123456789
 

Terminal 4: Set up the second UE

.. code-block:: bash

    sudo srsue --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2007,rx_port=tcp://localhost:2006,id=ue2,base_srate=23.04e6" --gw.netns=ue2 --usim.algo=xor --usim.imsi=001010123456780
 

Terminal 5: Start the gnuradio flowgraph

.. code-block:: bash

    cd main-file-repo
    python3 2ue4g.py




Deploying the xApp
------------------

.. code-block:: bash

    curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@nexran-onboard.url"
    curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"
    curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "nexran"}'



Terminal 6: Checking the xApp logs

.. code-block:: bash

    kubectl logs -f -n ricxapp -l app=ricxapp-nexran

Terminal 7

.. code-block:: bash

    sudo -i

.. code-block:: bash

    export NEXRAN_XAPP=`kubectl get svc -n ricxapp --field-selector metadata.name=service-ricxapp-nexran-rmr -o jsonpath='{.items[0].spec.clusterIP}'` ; echo $NEXRAN_XAPP

.. code-block:: bash

    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/version ; echo ; echo

.. code-block:: bash

    iperf -s -p 5010 -i 1

Terminal 8

.. code-block:: bash

    sudo iperf -s -p 5011 -i 1

Terminal 9

.. code-block:: bash

    iperf -c 192.168.0.2 -p 5010 -t 36000
    iperf -c 192.168.0.3 -p 5010 -t 36000



.. code-block:: bash

    echo NEXRAN_XAPP=$NEXRAN_XAPP ; echo

List nodeb and slices our xapp knows of. Should not be any yet

.. code-block:: bash

    echo Listing NodeBs: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/nodebs ; echo ; echo
    echo Listing Slices: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo
    echo Listing Ues: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo

Tell the nexran app about our nodeB

.. code-block:: bash

    echo "Creating NodeB (id=1):" ; echo
    curl -i -X POST -H "Content-type: application/json" -d '{"type":"eNB","id":411,"mcc":"001","mnc":"01"}' http://${NEXRAN_XAPP}:8000/v1/nodebs ; echo ; echo
    echo Listing NodeBs: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/nodebs ; echo ; echo

Create the slices “Fast and Slow” that can be bound per UE

.. code-block:: bash

    echo "Creating Slice (name=fast)": ; echo
    curl -i -X POST -H "Content-type: application/json" -d '{"name":"fast","allocation_policy":{"type":"proportional","share":1024}}' http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo
    echo Listing Slices: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo

.. code-block:: bash

    echo "Creating Slice (name=slow)": ; echo
    curl -i -X POST -H "Content-type: application/json" -d '{"name":"slow","allocation_policy":{"type":"proportional","share":256}}' http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo
    echo Listing Slices: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo

Bind those slices to the created nodeB

.. code-block:: bash

    echo "Binding Slice to NodeB (name=fast):" ; echo
    curl -i -X POST http://${NEXRAN_XAPP}:8000/v1/nodebs/enB_macro_001_001_0019b0/slices/fast ; echo ; echo
    echo "Getting NodeB (name=enB_macro_001_001_0019b0):" ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/nodebs/enB_macro_001_001_0019b0 ; echo ; echo

.. code-block:: bash

    echo "Binding Slice to NodeB (name=slow):" ; echo
    curl -i -X POST http://${NEXRAN_XAPP}:8000/v1/nodebs/enB_macro_001_001_0019b0/slices/slow ; echo ; echo
    echo "Getting NodeB (name=enB_macro_001_001_0019b0):" ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/nodebs/enB_macro_001_001_0019b0 ; echo ; echo

Create the UEs

.. code-block:: bash

    echo "Creating Ue (ue=001010123456789)" ; echo
    curl -i -X POST -H "Content-type: application/json" -d '{"imsi":"001010123456789"}' http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo
    echo Listing Ues: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo

.. code-block:: bash

    echo "Creating Ue (ue=001010123456788)" ; echo
    curl -i -X POST -H "Content-type: application/json" -d '{"imsi":"001010123456780"}' http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo
    echo Listing Ues: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo

Bind the fast slice to the UE 1

.. code-block:: bash

    echo "Binding Ue to Slice fast (imsi=001010123456789):" ; echo
    curl -i -X POST http://${NEXRAN_XAPP}:8000/v1/slices/fast/ues/001010123456789 ; echo ; echo
    echo "Getting Slice (name=fast):" ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices/fast ; echo ; echo

Bind the slow slice to the UE 2

.. code-block:: bash

    echo "Binding Ue (imsi=001010123456780):" ; echo
    curl -i -X POST http://${NEXRAN_XAPP}:8000/v1/slices/slow/ues/001010123456788 ; echo ; echo
    echo "Getting Slice (name=slow):" ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices/slow ; echo ; echo

Invert priority of fast and slow slices 

.. code-block:: bash

    echo "Inverting priority of fast and slow slices:" ; echo
    
.. code-block:: bash

    curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":1024}}' http://${NEXRAN_XAPP}:8000/v1/slices/slow ; echo ; echo ;

.. code-block:: bash

    curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":256}}' http://${NEXRAN_XAPP}:8000/v1/slices/fast ; echo ; echo

Give more or less share to each slice

.. code-block:: bash

    curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":1024}}' http://${NEXRAN_XAPP}:8000/v1/slices/slow ; echo ; echo ;
    curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":256}}' http://${NEXRAN_XAPP}:8000/v1/slices/fast ; echo ; echo

.. code-block:: bash

    curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":1024}}' http://${NEXRAN_XAPP}:8000/v1/slices/fast ; echo ; echo
    

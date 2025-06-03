==================================
Deploying the Intrusion Detection xApp
==================================

Table of Contents
=================

* :ref:`xApp-Description`
* :ref:`Prerequisites`
* :ref:`RIC-Setup`
* :ref:`Setup-Srslte`
* :ref:`Setup-Nginx`
* :ref:`Setup-ID`
* :ref:`Setup-5G`
* :ref:`Deploying`
* :ref:`Running-The-xApp`
* :ref:`Enabling-CUDA`


.. _xApp-Description:

Xapp Description
================

the Intrusion Detection xApp is designed to identify malicious User Equipments (UEs) within a simulated 5G network environment. It operates by monitoring the data rates of connected UEs. The xApp can first train a model on normal network traffic patterns; then, during inference mode, if a UE's data rate surpasses this trained threshold and registers at least two anomalous events, it is flagged as malicious. The system utilizes iperf3 to generate network traffic for testing, and a specific script (iperfThrottleTest.sh) can be used to simulate a UE behaving maliciously by causing its traffic to exceed expected levels, thereby triggering a detection by the xApp. 


.. _Prerequisites:

Prerequisites
=============

System Requirements
-------------------

* OS: `Ubuntu Desktop 20.04 LTS (Focal Fossa) <https://www.releases.ubuntu.com/focal/ubuntu-20.04.6-desktop-amd64.iso>`_ Baremetal Preferred
* `Low Latency Kernel recommended <https://unix.stackexchange.com/questions/739769/how-do-you-install-the-linux-lowlatency-kernel-and-why-does-it-stops-at-version>`_
* `Performance mode setting <https://askubuntu.com/questions/604720/setting-to-high-performance>`_
* CPU(s): 12 vCPUs (Threads)
* RAM: 16 GB minimum
* Storage: 100 GB

Install packages

.. code-block:: bash


    sudo apt update -y
    sudo apt install git vim tmux build-essential cmake libfftw3-dev libmbedtls-dev libboost-program-options-dev libconfig++-dev libsctp-dev libtool autoconf gnuradio python3-pip iperf3 libzmq3-dev -y

.. _RIC-Setup:

Setup
=====

Setup - Near-Realtime RIC
-------------------------

Clone OAIC and Install Submodules

.. code-block:: bash


    cd ~/
    git clone https://github.com/openaicellular/oaic.git
    cd oaic
    git submodule update --init --recursive --remote

Install Kubernetes, Docker, and Helm
------------------------------------

.. code-block:: bash


    cd ~/oaic/RIC-Deployment/tools/k8s/bin
    git checkout kubernetes-fix
    ./gen-cloud-init.sh
    sudo ./k8s-1node-cloud-init-k_1_16-h_2_17-d_cur.sh

You can check if all the pods in the Kubernetes Cluster are in “Running” state by using,

.. code-block:: bash


    sudo kubectl get pods -A

NOTE: There is an issue with with this setup at the moment. Uninstalling docker and installing an older verison will fix it

.. code-block:: bash


    sudo apt-get purge -y docker-engine docker docker.io docker-ce docker-ce-cli
    sudo apt-get autoremove -y --purge docker-engine docker docker.io docker-ce
    sudo rm -rf /var/lib/docker /etc/docker
    apt-cache madison docker-ce

This will list the available versions for install. Install the oldest verion listed.

.. code-block:: bash


    sudo apt-get install docker-ce=<VERSION> docker-ce-cli=<VERSION> containerd.io


One time setup for Influxdb
---------------------------
Create a ricinfra namespace

.. code-block:: bash


    sudo kubectl create ns ricinfra

Install the nfs-common package for Influxdb setup

.. code-block:: bash


    sudo helm install stable/nfs-server-provisioner --namespace ricinfra --name nfs-release-1
    sudo kubectl patch storageclass nfs -p '{"metadata": {"annotations":{"storageclass.kubernetes.io/is-default-class":"true"}}}'
    sudo apt install nfs-common -y

When the RIC platform is undeployed, the ``ricinfra`` namespace will also be removed. So, you will need to run the one-time setup procedure again when re-deploying the RIC.

Pull Modified E2 docker image
-----------------------------
You're required a local docker registry to host docker images. You can create one using this command:

.. code-block:: bash


    sudo docker run -d -p 5001:5000 --restart=always --name ric registry:2

Pull the E2 termination docker image from the OAIC's DockerHub and retagging it to be hosted in the local registry

.. code-block:: bash


    sudo docker pull oaic/e2:5.5.0
    sudo docker tag oaic/e2:5.5.0 localhost:5001/ric-plt-e2:5.5.0
    sudo docker push localhost:5001/ric-plt-e2:5.5.0

Deploy Near-Realtime RIC
========================

Deploy the Near-Realtime RIC with the commands below

.. code-block:: bash


    cd ~/oaic/RIC-Deployment/RECIPE_EXAMPLE/PLATFORM/
    git checkout e_rel_xapp_onboarder_support
    cd ~/oaic/RIC-Deployment/bin
    sudo ./deploy-ric-platform -f ../RECIPE_EXAMPLE/PLATFORM/example_recipe_oran_e_release_modified_e2.yaml

Use this command to check all the Kubernetes pods


.. code-block:: bash


    sudo kubectl get pods -A


**Make sure that All pods (besides tiller generator) are in the ``1/1 Running`` state before proceeding**


.. _Setup-Srslte:

Setup - Srslte with E2 Agent
============================

Installing Asn1c compiler
------------------------

.. code-block:: bash


    cd ~/oaic/asn1c
    git checkout velichkov_s1ap_plus_option_group
    autoreconf -iv
    ./configure
    make -j`nproc`
    sudo make install
    sudo ldconfig
    cd ..

Install Srslte from source
--------------------------

Checkout srslte-e2 to ss-xapp-support. This is where the code is updated for the kpm indication for the ss-xapp to work.

.. code-block:: bash


    cd ~/oaic
    git clone https://github.com/openaicellular/srslte-e2
    cd srslte-e2
    git checkout ss-xapp-support
    rm -rf build
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
    sudo srslte_install_configs.sh user --force

.. _Setup-Nginx:


Setup - Nginx Web Server
------------------------

For this workshop Nginx web server is required to setup a web server for hosting xApp configuration files for xApp deployment

First install Nginx package

.. code-block:: bash


    sudo apt-get install nginx -y

Check to see if Nginx is in the ``active (running)`` state

.. code-block:: bash


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

Paste the following content in the *conf* file.

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


    sudo nginx -t

.. _Setup-ID:

Setup - Intrusion Detection with xApp
====================================

Modify Srslte
-------------

You will need to modify srslte to be able to connect with 2 user equipments (UEs). To do this open the ``user_db.csv`` file with your preferred text editor

.. code-block:: bash


    sudo -i
    cd .config/srslte
    vim user_db.csv

Replace these lines of code with text given below. This allows the ENB to be able to connect with 2 UEs. *Format of these lines: "Name, Auth, IMSI, Key, OP_Type, OP/OPc, AMF, SQN, QCI, IP_alloc".*

.. code-block:: bash


    ue2,xor,001010123456780,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,00000000173d,7,dynamic
    ue3,xor,001010123456781,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8002,0000000015f3,7,dynamic
    ue4,xor,001010123456782,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8003,0000000015f4,7,dynamic
    ue1,xor,001010123456789,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,9001,000000001404,7,dynamic

Exit out of sudo once you are done editing the file

    exit

Installing xApp
--------------

Clone down the xApp

.. code-block:: bash


    cd ~/oaic
    git clone https://github.com/openaicellular/intrusion-detection-xapp.git

Create a symlink from the xApp’s config file. This can be replaced by another symlink in the future. Reload Nginx once this has been done.

.. code-block:: bash


    cd ~/oaic/intrusion-detection-xapp
    sudo cp config-file.json /var/www/xApp_config.local/config_files/
    sudo systemctl reload nginx

Now we are going to build the xapp from the ``DockerFile``.

.. code-block:: bash


    cd ~/oaic/intrusion-detection-xapp
    sudo docker build . -t xApp-registry.local:5008/ss:0.1.0

Paste the following in the ``ss-xapp-onboard.url`` file located in the ``intrusion-detection-xapp`` directory. Substitute the ``<machine_ip_addr>`` with the IP address of your machine. You can find this by pasting the command ``hostname -I | cut -f1 -d' '`` in the terminal.

.. code-block:: bash


    cd ~/oaic/intrusion-detection-xapp
    vim ss-xapp-onboard.url

Paste the following in url file. **Remember to change Ip address**

    {"config-file.json_url":"http://<machine_ip_addr>:5010/config_files/config-file.json"}


.. Setup-5G:

Setup - 5G Network
====================

Srsepc
-----

**Terminal 1**: Start the Core Network/Add UEs to Network Namespace

.. code-block:: bash


    sudo ip netns add ue1
    sudo ip netns add ue2
    sudo ip netns add ue3
    sudo ip netns add ue4
    sudo ip netns list
    sudo srsepc

Srsenb
------

**Terminal 2**: Set up Environment Variables and Base Station

.. code-block:: bash


    export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
    export E2NODE_PORT=5006
    export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`

.. code-block:: bash


    sudo srsenb --enb.n_prb=100 --enb.name=enb1 --enb.enb_id=0x19B \
    --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2000,rx_port=tcp://localhost:2009,id=enb,base_srate=23.04e6" --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT} --slicer.enable=1 --slicer.workshare=0



**Make sure you can see the eNB have a RIC State Establish message as shown in the image below before proceeding.**


If you are not getting a RIC State Established message, you need to restart the e2term-alpha pod by using the command: ``sudo kubectl delete pod -n ricplt -l app=ricplt-e2term-alpha``. If that still doesn't work, undeploy and redeploy the near-realtime RIC.

Srsue
-----

**Terminal 3**: Set up the first UE

.. code-block:: bash


    sudo srsue \
    --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2010,rx_port=tcp://localhost:2300,id=ue,base_srate=23.04e6" --usim.algo=xor --usim.imsi=001010123456789 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 --log.all_level=warn --log.filename=stdout --gw.netns=ue1

**Terminal 4**: Set up the second UE

.. code-block:: bash


    sudo srsue \
    --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2007,rx_port=tcp://localhost:2400,id=ue,base_srate=23.04e6" --usim.algo=xor --usim.imsi=001010123456780 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 --log.all_level=warn --log.filename=stdout --gw.netns=ue2
    
**Terminal 5**: Set up the third UE

.. code-block:: bash


    sudo srsue \
    --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2008,rx_port=tcp://localhost:2500,id=ue,base_srate=23.04e6" --usim.algo=xor --usim.imsi=001010123456781 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 --log.all_level=warn --log.filename=stdout --gw.netns=ue3
    
**Terminal 6**: Set up the fourth UE

.. code-block:: bash


    sudo srsue \
    --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2011,rx_port=tcp://localhost:2600,id=ue,base_srate=23.04e6" --usim.algo=xor --usim.imsi=001010123456782 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 --log.all_level=warn --log.filename=stdout --gw.netns=ue4

Gnuradio
--------

**Terminal 7**: Run the Gnuradio script

.. code-block:: bash


    cd ~/oaic/intrusion-detection-xapp
    python3 four_ue.py

**Make sure the UEs are connected with an Ip address before proceeding**

Iperf3
------

**Execute the next commands each in a separate terminal**

**Terminal 8-11**: Set up iperf3 test on the server side


.. code-block:: bash


    iperf3 -s -B 172.16.0.1 -p 5006 -i 1
.. code-block:: bash


    iperf3 -s -B 172.16.0.1 -p 5020 -i 1
.. code-block:: bash


    iperf3 -s -B 172.16.0.1 -p 5021 -i 1
.. code-block:: bash


    iperf3 -s -B 172.16.0.1 -p 5022 -i 1


**Execute the next commands each in separate terminals as well**

**Terminal 12-15**: Set up iperf3 test on the client side

We add an additional bandwidth argument "-b xxM" on each iperf3 test on client side to create a scenario of UEs trying to access more or less of resources on the network. If a UE surpasses the trained data rate, it is considered as malicious by the xApp.

.. code-block:: bash


    sudo ip netns exec ue1 iperf3 -c 172.16.0.1 -p 5006 -i 1 -t 36000 -R -b 1M
.. code-block:: bash


    sudo ip netns exec ue2 iperf3 -c 172.16.0.1 -p 5020 -i 1 -t 36000 -R -b 1M
.. code-block:: bash


    sudo ip netns exec ue3 iperf3 -c 172.16.0.1 -p 5021 -i 1 -t 36000 -R -b 1M
.. code-block:: bash


    sudo ip netns exec ue4 iperf3 -c 172.16.0.1 -p 5022 -i 1 -t 36000 -R -b 1M

**You should notice traffic flow on both the server and client side for all UEs.**

.. _Deploying:

Deploying the xApp
=====================

.. code-block:: bash


    cd ~/oaic/intrusion-detection-xapp
.. code-block:: bash


    export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
    export E2MGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2mgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export E2TERM_SCTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`
    export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export RTMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-rtmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
.. code-block:: bash


    curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@ss-xapp-onboard.url"
    curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"
    curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "ss"}'

**If you are repeating an experiement, you may want to restart the pod using the command below. By doing so, you do not have to undeploy and redeploy the xApp again.**
.. code-block:: bash


    sudo kubectl -n ricxapp rollout restart deployment ricxapp-ss

.. _Running-The-xApp:

Running the xApp
================

**Terminal 16**: In your EPC & eNB server's terminal, print the logs for the SS xApp

**The xApp has to be deployed in order for this to work.**
.. code-block:: bash


    sudo kubectl logs -f -n ricxapp -l app=ricxapp-ss

**Terminal 17**: Now run the test script with the following commands on a separate terminal.

.. code-block:: bash


    cd ~/oaic/intrusion-detection-xapp
    sudo chmod +x zmqfourue.sh
    sudo ./zmqfourue.sh

To run the script again, you have to restart the xApp and redeploy the network again.

.. code-block:: bash


    sudo kubectl -n ricxapp rollout restart deployment ricxapp-ss
    
**Rerunning the xapp now requires deleting the pods manually if CUDA has been enabled**

Use ``sudo kubectl get pods -A`` to see the active pods

Use ``sudo kubectl delete pod ricxapp-ss-XXX -n ricxapp`` change the XXX the correct pod name

The correct pod to delete should be in the "running" state
    
There is a flag in the intrustionDetection.py file called "trained" that is set to false by default. When false the model will train before doing
the inference for the intrusion detection. If you only want to do the inference set it to "True".

If running the intrustion detection with the iperf3 commands above, there should be no malicious ue. Two anomalies are required
per ue for it to be considered malicious. There is a script called iperfThrottleTest.sh that will throttle the iperf3 client and cause a malicious
UE to appear.

Replace:
.. code-block:: bash


    sudo ip netns exec ue1 iperf3 -c 172.16.0.1 -p 5006 -i 1 -t 36000 -R -b 1M

With. While the xappp is in inference mode:
.. code-block:: bash


    cd oaic/intrusion-detection-xapp
    ./iperfThrottleTest.sh
    
Getting CUDA On The App
=======================

To run the xApp with GPU support follow these steps

Make sure the correct NVIDIA drivers are installed and install NVIDIA container toolkit:
.. code-block:: bash


    curl -fsSL https://nvidia.github.io/libnvidia-container/gpgkey | sudo gpg --dearmor -o /usr/share/keyrings/nvidia-container-toolkit-keyring.gpg \
    && curl -s -L https://nvidia.github.io/libnvidia-container/stable/deb/nvidia-container-toolkit.list | \
    sed 's#deb https://#deb [signed-by=/usr/share/keyrings/nvidia-container-toolkit-keyring.gpg] https://#g' | \
    sudo tee /etc/apt/sources.list.d/nvidia-container-toolkit.list
    
    sudo apt-get update
    sudo apt-get install -y nvidia-container-toolkit


.. Enabling-CUDA:

Configure Docker to Run with CUDA
=================================
.. code-block:: bash


    sudo nvidia-ctk runtime configure --runtime=docker
    sudo systemctl daemon-reload
    sudo systemctl restart docker
    
The kubernetes node should now show the GPU as allocatable. Run the commands below to check
.. code-block:: bash


    sudo kubectl get nodes
    sudo kubectl describe node <node-name>
    
You should see something along the lines of:

    allocatable:
        nvidia.com/gpu: 1
If the GPU is allocatable run:
.. code-block:: bash


    sudo kubectl get deployment ricxapp-ss -n ricxapp -o yaml > deploymentGPU.yaml
    nano deploymentGPU.yaml

Update the the resources limits requests and add:
.. code-block:: bash


    resources:
        requests:
          nvidia.com/gpu: 1
        limits:
          nvidia.com/gpu: 1

Run the following command to update the configuration:

.. code-block:: bash


    kubectl apply -f deploymentGPU.yaml

The Intrustion Detection program should now detect the GPU

**NOTE: do not replace what is already there**

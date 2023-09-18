
OAIC Installation (ZeroMQ version)
==================================

This documentation provides a consolidated overview of downloading, installing, and
running OAIC-C on Ubuntu 20.04 using ZeroMQ for communication between radio nodes.
The instructions are divided into five steps:

1. Download the source code and install dependencies
2. Install O-RAN and the near real-time RIC
3. Install srsRAN with e2 interface
4. Start a 5G network
5. Deploy an xApp (deprecated)


Step 1. Clone OAIC and Install Dependencies
-------------------------------------------

Clone OAIC with all relevant submodules:

    git clone https://github.com/openaicellular/oaic.git

    cd oaic    

    git submodule update --init --recursive --remote

Install Dependencies:

.. code-block:: bash

    sudo apt-get install -y build-essential cmake libfftw3-dev libmbedtls-dev libboost-program-options-dev libconfig++-dev libsctp-dev libtool autoconf
    sudo apt-get install -y libzmq3-dev
    sudo apt-get install -y nginx


Step 2. ORAN Installation
-------------------------

The ``RIC-Deployment`` directory contains the deployment scripts and pre generated helm charts for each of the RIC components. This repository also contains some “demo” scripts which can be run after complete installation.

.. code-block:: bash

    cd RIC-Deployment/tools/k8s/bin

Executing the below command will output a shell script called ``k8s-1node-cloud-init-k_1_16-h_2_17-d_cur.sh``. The file name indicates that we are installing Kubernetes v1.16 (k_1_16), Helm v2.17 (h_2_17) and the latest version of docker (d_cur).

.. code-block:: bash

    ./gen-cloud-init.sh

Executing the generated script ``k8s-1node-cloud-init-k_1_16-h_2_17-d_cur.sh`` will install Kubernetes, Docker and Helm with version specified in the k8s/etc/infra.c. This also installs some pods which help cluster creation, service creation and internetworking between services. Running this script will replace any existing installation of Docker host, Kubernetes, and Helm on the VM. The script will reboot the machine upon successful completion. This will take some time (approx. 15-20 mins).

.. code-block:: bash

    sudo ./k8s-1node-cloud-init-k_1_16-h_2_17-d_cur.sh

Once the machine is back up, check if all the pods in the newly installed Kubernetes Cluster are in “Running” state using,

.. code-block:: bash

    sudo kubectl get pods -A 

The next three commands installs the nfs-common package for kubernetes through helm in the “ricinfra” namespace and for the system.

.. code-block:: bash

    sudo helm install stable/nfs-server-provisioner --namespace ricinfra --name nfs-release-1
    sudo kubectl patch storageclass nfs -p '{"metadata": {"annotations":{"storageclass.kubernetes.io/is-default-class":"true"}}}'
    sudo apt install nfs-common 

Local docker registry to host docker images. You can create one using, (You will need "super user" permissions)

.. code-block:: bash

    sudo docker run -d -p 5001:5000 --restart=always --name ric registry:2

Navigate to ``ric-plt-e2`` directory.

.. code-block:: bash
   
   cd ../../../../ric-plt-e2

The code in this repo needs to be packaged as a docker container.
We make use of the existing Dockerfile in ``RIC-E2-TERMINATION`` to do this.
Execute the following commands in the given order 

.. code-block:: bash

    cd RIC-E2-TERMINATION
    sudo docker build -f Dockerfile -t localhost:5001/ric-plt-e2:5.5.0 .
    sudo docker push localhost:5001/ric-plt-e2:5.5.0
    cd ../../

Once the Kubernetes clusters are deployed, it is now time for us to deploy the near-real time RIC cluster.

.. code-block:: bash

    cd RIC-Deployment/bin
    sudo ./deploy-ric-platform -f ../RECIPE_EXAMPLE/PLATFORM/example_recipe_oran_e_release_modified_e2.yaml


Step 3. srsRAN installation
---------------------------

We will be using the modified ``asn1c compiler`` (for RAN and CN) that is hosted by Open Air Interface (OAI)

.. code-block:: bash

    cd ../../asn1c
    # git checkout velichkov_s1ap_plus_option_group
    autoreconf -iv
    ./configure
    make -j4
    sudo make install
    sudo ldconfig
    cd ..

srsRAN with E2 agent Installation

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
    sudo srsran_install_configs.sh service
    cd ../../


Step 4. Setup your own 5G Network
---------------------------------

Start srsRAN EPC:

.. code-block:: bash

    sudo ip netns add ue1
    sudo srsepc

run srsRAN en-gNB:

.. code-block:: bash

    export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
    export E2NODE_PORT=5006
    export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`

    sudo srsenb --enb.n_prb=50 --enb.name=enb1 --enb.enb_id=0x19B \
    --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101,id=enb,base_srate=23.04e6" \
    --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=enbLog.txt --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT}

    echo "Waiting for RIC state to establish"
    sleep 45
    
Start srsUE

.. code-block:: bash

    sudo srsue --gw.netns=ue1

Check for connectivity

.. code-block:: bash

    sudo ip netns exec ue1 ping 172.16.0.1 -c3


Step 5. Deploy the KPIMON xApp (deprecated)
-------------------------------------------

Start nginx

.. code-block:: bash
    
    sudo systemctl status nginx
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

    cd /etc/nginx/conf.d

    sudo sh -c "echo 'server {
    listen 5010 default_server;
		server_name xApp_config.local;
		location /config_files/ {

            root /var/www/xApp_config.local/;
		}

	}' >xApp_config.local.conf"

Save the configuration file and check if there are any errors in the configuration file.


.. code-block:: bash

    sudo nginx -t

Create a symlink from the xapp’s config file (in this case kpimon).This can be replaced by another symlink in the future. Reload Nginx once this has been done.

.. code-block:: bash
    
    sudo cp ~/oaic/ric-scp-kpimon/scp-kpimon-config-file.json /var/www/xApp_config.local/config_files/
    sudo systemctl reload nginx

Now, you can check if the config file can be accessed from the newly created server. Place all files you want to host in the config_files directory

.. code-block:: bash
    
    export MACHINE_IP=`hostname  -I | cut -f1 -d' '`
    curl http://${MACHINE_IP}:5010/config_files/scp-kpimon-config-file.json

Now, we create a docker image of the KPIMON xApp using the given docker file.



.. code-block:: bash

    cd ~/oaic/ric-scp-kpimon
    sudo docker build . -t xApp-registry.local:5008/scp-kpimon:1.0.1

First, we need to get some variables of RIC Platform ready. The following variables represent the IP addresses of the services running on the RIC Platform.

.. code-block:: bash

    export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
    export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`

Check for Helm charts, there should be none.

.. code-block:: bash

    curl --location --request GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"

Next, we need to create a .url file to point the xApp-onboarder to the Ngnix server to get the xApp descriptor file and use it to create a helm chart and deploy the xApp.

.. code-block:: bash

    export MACHINE_IP=`hostname  -I | cut -f1 -d' '`
    echo {\"config-file.json_url\":\"http://"${MACHINE_IP}":5010/config_files/scp-kpimon-config-file.json\"} > scp-kpimon-onboard.url

Deploy the xApp

.. code-block:: bash

    curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@scp-kpimon-onboard.url"
    curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"
    curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "scp-kpimon"}'

Verify the xApp deployment

.. code-block:: bash

    sleep 15
    sudo kubectl get pods -A | grep 'kpimon' | grep 'Running'

Check for logs

.. code-block:: bash

    sudo timeout 5 sudo kubectl logs -f -n ricxapp -l app=ricxapp-scp-kpimon

    rc=$? #124 for succesful ping

    if [ $rc -ne 124 ] ; then exit -1 ; fi

    echo 'Successful: KPIMON xApp up and running'

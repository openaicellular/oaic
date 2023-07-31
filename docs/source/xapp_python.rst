.. _xapppython:

==================================
Developing a custom xApp in Python
==================================

This document describes how to write an xApp in Python and how to deploy it on the RIC platform.

Background
----------

An xApp is simply an application that is deployed to the RIC and is capable of communicating to the RAN.
An xApp can be developed in any programming language, but to be O-RAN compliant, it needs to be able to communicate over the E2 interface to E2 nodes.
An E2 Node refers to a component of the RAN that can interface with the RIC via E2, usually referring to the base station (DU/CU).

For this demonstration, we will simplify the xApp development process such that we utilize an E2-like interface, enabling us to focus on the functionality of our application rather than the specifications of the protocol (encoding and decoding of ASN.1 messages, subscription processes, etc.).

To understand how an xApp works, first we must look at how an O-RAN network is implemented.
The RAN Intelligent Controller (RIC) is capable of dynamically controlling the RAN.
The RIC consists of a Kubernetes cluster which hosts several deployments which contain services and pods.
An application such as an xApp is executed in the RIC using a deployment, which consists of the instructions and components needed to run the application.

Inside the deployment, there are pods which contain different microservices, which are small containers running different software and make up the application. These containers are created and deployed using Docker, using an image that we prepare beforehand that consists of everything needed for the application to start instantly. For this xApp, we will use a single Kubernetes deployment with a single pod, running a single Docker container.

For more information on Kubernetes and Docker, see the `Background on Docker, Kubernetes & Helm` section.


Development
-----------

First, we use a Python file called ``app.py`` to store the main code of our xApp. In this file we will setup an SCTP connection and run a constant loop to accept a connection from a nodeB (base station), receive I/Q data and send control messages to change the RAN's behavior.

When using the E2-like interface, the xApp acts as an SCTP server and the nodeB is a client.

Here are some excerpts of the code:

.. code-block:: python

    def init_e2(self):
        global server

        # This will automatically find a correct IP address to use, which can change depending on where the xApp is deployed.
        ip_addr = socket.gethostbyname(socket.gethostname())
        port = 5000

        # Start the SCTP server and bind to the address and port
        server = sctp.sctpsocket_tcp(socket.AF_INET)
        server.bind((ip_addr, port)) 
        server.listen()

    def entry(self):
        global current_iq_data, last_cmd, server

        # Initialize the E2-like interface
        init_e2(self)

        # E2-like interface main loop
        while True:
            try:
                # Accept SCTP connections
                conn, addr = server.accept()

                log_info(self, f'Connected by {addr}')

                initial = time.time()  # initial timestamp

                # Loop which runs if an SCTP connection is established
                while True:
                    # Send an E2-like request within the first second to ask nodeB to send I/Q data
                    if initial - time.time() < 1.0:
                        conn.send(f"E2-like request at {datetime.now().strftime('%H:%M:%S')}".encode('utf-8'))
                        log_info(self, "Sent E2-like request")

                    # Sending too much SCTP data in a single message will freeze the connection up, so we have srsRAN split our data
                    # into chunks of 16384 bytes. The data in this case is I/Q data sourced from the RU (radio unit).
                    # This section of code will receive enough I/Q data to make one 10ms spectrogram.
                    data = conn.recv(16384)
                    if data:
                        log_info(self, f"Receiving I/Q data...")
                        while len(data) < spectrogram_size:
                            data += conn.recv(16384)
                    
                        log_info(self, f"Received buffer size {len(data)}")
                        log_info(self, f"Finished receiving message, processing")
                        
                        # Point our global variable to the I/Q data we just received, and use our machine learning model
                        # to make a prediction.
                        current_iq_data = data
                        result = run_prediction(self)

                        # If there is interference, send a command to turn the base station off.
                        # We can do this by setting the transmit (TX) gain of the radio to a very weak amount (-10000dB)
                        # If the interference goes away, send a command to turn it back on.
                        if result == 'Interference':
                            log_info(self, "Interference signal detected, sending control message to turn nodeB off")
                            conn.send(cmds['BASE_STATION_OFF'])
                            #last_cmd = cmds['BASE_STATION_OFF']
                        elif result in ('5G', 'LTE'): #and last_cmd == cmds['BASE_STATION_OFF']:
                            log_info(self, "Interference signal no longer detected, sending control message to turn nodeB on")
                            conn.send(cmds['BASE_STATION_ON'])
                            #last_cmd = cmds['BASE_STATION_ON']

            # Log any errors with the SCTP connection, but continue to run
            except OSError as e:
                log_error(self, e)

    def run_prediction(self):
        global current_iq_data

        # convert I/Q data into a spectrogram that our machine learning model can use as input
        sample = iq_to_spectrogram(current_iq_data)
        # Make a prediction with our spectrogram and get the result
        result = predict(self, sample)

        return result

    def predict(self, data) -> str:
        # Actually do the prediction. This will be dependent on your model.
        prediction, confidence = model_predict(ai_model, data)

        classifiers = ['5G', 'LTE', 'Interference']

        return classifiers[prediction] if confidence > CONFIDENCE_THRESHOLD else None


    def model_predict(model, unseen_data):
        # Instead of implementing a real model, we will simply use random values

        prediction = random.randint(0,2)
        confidence = random.random()

        return prediction, confidence


Deployment
----------

Building the Docker image
~~~~~~~~~~~~~~~~~~~~~~~~~

Our xApp will be hosted in a Docker container. In order to create a Docker container, we must provide a Dockerfile which will provide the instructions as to how the machine should be set up. In this case, we use an Ubuntu setup with Python as the base for our Docker image. This is what the Dockerfile looks like:

.. code-block:: docker

    # Load a miniconda setup for our base Docker image which contains Python
    FROM continuumio/miniconda3

    # Install all necessary libraries
    RUN apt-get update && apt-get -y install build-essential musl-dev libjpeg-dev zlib1g-dev libgl1-mesa-dev wget dpkg

    # Copy all the files in the current directory to /tmp/ml in our Docker image
    COPY . /tmp/ml

    # Go to /tmp/ml
    WORKDIR /tmp/ml

    # Install requirements.txt
    RUN pip install --upgrade pip && pip install requirements.txt

    # Set our xApp to run immediately when deployed
    ENV PYTHONUNBUFFERED 1
    CMD app.py

Once we have this Dockerfile, we can then build our Docker image and submit it to the xApp registry. This is done with one command:

.. code-block:: docker

    sudo docker build . -t xApp-registry.local:5008/ric-app-ml:1.0.0

This builds a Docker image labeled ric-app-ml with version 1.0.0, and submits it to the xApp registry.

Creating the xApp config
~~~~~~~~~~~~~~~~~~~~~~~~

In our xApp, we have an init folder which contains the config.json file.

.. code-block:: json

    {
        "json_url": "ric-app-ml",
        "xapp_name": "ric-app-ml",
        "version": "1.0.0",
        "containers": [
            {
                "name": "ric-app-ml",
                "image": {
                    "registry": "xApp-registry.local:5008",
                    "name": "ric-app-ml",
                    "tag": "1.0.0"
                }
            }
        ],
        "messaging": {
            "ports": [
                {
                    "name": "rmr-data",
                    "container": "ric-app-ml",
                    "port": 4560,
                    "rxMessages": [ "RIC_SUB_RESP", "RIC_SUB_FAILURE", "RIC_INDICATION", "RIC_SUB_DEL_RESP", "RIC_SUB_DEL_FAILURE" ],
                    "txMessages": [ "RIC_SUB_REQ", "RIC_SUB_DEL_REQ" ],
                    "policies": [1],
                    "description": "rmr receive data port for ric-app-ml"
                },
                {
                    "name": "rmr-route",
                    "container": "ric-app-ml",
                    "port": 4561,
                    "description": "rmr route port for ric-app-ml"
                }
            ]
        },
        "rmr": {
            "protPort": "tcp:4560",
            "maxSize": 2072,
            "numWorkers": 1,
            "txMessages": [ "RIC_SUB_REQ", "RIC_SUB_DEL_REQ" ],
            "rxMessages": [ "RIC_SUB_RESP", "RIC_SUB_FAILURE", "RIC_INDICATION", "RIC_SUB_DEL_RESP", "RIC_SUB_DEL_FAILURE" ],
        "policies": [1]
        }
    }

This config file is important as it signifies where the Docker image is located, and also provides the ports and capabilities of the E2 interface.
In our case, we are using an E2-like interface instead of the E2, so we will expose our own port after the deployment.

Finding local IP address
~~~~~~~~~~~~~~~~~~~~~~~~

Before running further steps, we will need the local IP address of the system. Use the first command ``hostname -I`` to find your local IP addresses. The first one that appears should work. Then, run the second command and replace <ip address> with the first IP you see. On my system, the address is ``10.0.2.15``.

.. code-block:: rst

    hostname -I
    export HOST_IP=<ip address>

Once this is done, we can replace the machine IP address with $HOST_IP.

Configuring the Nginx Web server
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The xApp descriptor files (config.json) must be hosted on a webserver when we use the **xapp-onboarder** to deploy xApps. This is because the xApp onboarder cannot access our local files, so we have to upload them to the network where it can find and download them. We will use Nginx as our webserver for hosting config files.

First, we need to install Nginx and check if it is in ``active (running)``  state. 

.. code-block:: rst

	sudo apt install nginx
	sudo systemctl status nginx

.. Unlink the default Configuration file and check if it is unlinked

.. .. code-block:: rst

.. 	cd /etc/nginx/sites-enabled
.. 	sudo unlink default
.. 	cd ../

Now we create some directories which can be accessed by the server and where the config files can be hosted.

.. code-block:: rst

	sudo mkdir /var/www/xApp_config.local
	sudo mkdir /var/www/xApp_config.local/config_files

Create a Custom Configuration File and define file locations

.. code-block:: rst
	
	sudo nano /etc/nginx/conf.d/xApp_config.local.conf

Paste the following content in the *conf* file.

.. code-block:: rst  

	server {
	    listen 5010 default_server;
	    server_name xApp_config.local;
	    location /config_files/ {

		root /var/www/xApp_config.local/;
	    }

	}


Save and update the configuration file with the following command, and check if there are any errors in the configuration file. If there is no output, then it updated successfully.

.. code-block:: rst

	sudo nginx -t

Hosting the config Files
~~~~~~~~~~~~~~~~~~~~~~~~

Make sure you are in the xApp directory, then copy the xApp config file to this directory. When we copy this file with sudo, it also protects the file from being modified, so we use the chmod command to re-enable read/write permissions.

.. code-block:: rst
	
    sudo cp init/config-file.json /var/www/xApp_config.local/config_files/ml-config-file.json
    sudo chmod 755 /var/www/xApp_config.local/config_files/ml-config-file.json

Now, you can check if the config file can be accessed from the newly created server.

.. code-block:: rst

	curl http://$HOST_IP:5010/config_files/ml-config-file.json


Create onboard URL file
~~~~~~~~~~~~~~~~~~~~~~~

Next, we need to create a ``.url`` file to point the ``xApp-onboarder`` to the Nginx server to get the xApp descriptor file and use it to create a helm chart and deploy the xApp.

.. code-block:: rst

	nano ml-onboard.url	

Paste the following in the ``ml-onboard.url`` file. Substitute the ``<machine_ip_addr>`` with the IP address of your machine. You can find this out through ``hostname -I`` or ``echo $HOST_IP``.

.. code-block:: rst

	{"config-file.json_url":"http://<machine_ip_addr>:5010/config_files/ml-config-file.json"}

Save the file. Now we are ready to deploy the xApp. 

Onboard and deploy the xApp
~~~~~~~~~~~~~~~~~~~~~~~~~~~

First, we collect and store the IP address of the Kong proxy to a variable, which allows you to connect to the different components of the RIC through a single address.

.. code-block:: rst

    export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`

Then, we submit our onboard URL file to the xApp onboarder, which indicates to the onboarder where our xApp config file is.

.. code-block:: rst

	curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@ml-onboard.url"

The config file is then processed by the xApp onboarder and a chart is created, which contains the instructions to deploy the xApp.

Finally, we request that the App Manager deploys our specific xApp, ``ric-app-ml``. It will use the chart that the xApp onboarder has to deploy our xApp.

.. code-block:: rst

	curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "ric-app-ml"}'

Verify if the xApp is deployed using ``sudo kubectl get pods -A``. There should be a ``ric-app-ml`` pod visible in the "ricxapp" namespace. 


Demonstration
-------------

Managing xApp deployment
~~~~~~~~~~~~~~~~~~~~~~~~
View Kubernetes pods:
``sudo kubectl get pods -A``

View Kubernetes services:
``sudo kubectl get svc -A``

Build Docker image:
``sudo docker build . -t xApp-registry.local:5008/ric-app-ml:1.0.0``

Restart xApp:
``sudo kubectl rollout restart deployment ricxapp-ric-app-ml -n ricxapp``

View xApp logs (replace <podname> with the name of your xApp's pod):
``sudo kubectl logs -n ricxapp <podname>``

Enter xApp Kubernetes pod with bash shell (replace <podname> with the name of your xApp's pod):
``sudo kubectl exec --stdin --tty -n ricxapp <podname>  -- /bin/sh``

Open additional port for E2-like interface
``sudo kubectl expose deployment ricxapp-ric-app-ml --port 5000 --target-port 5000 --protocol SCTP -n ricxapp --type=NodePort``

Connecting to srsRAN
~~~~~~~~~~~~~~~~~~~~~~~~

We will use a modified version of srsRAN with the E2-like interface enabled.

To connect our xApp to the E2-like interface, we also need to expose port 5000 of the xApp to our system. This command will enable SCTP connections on our local IP address by creating a NodePort service in Kubernetes called ricxapp-ric-app-ml.

.. code-block:: rst

    sudo kubectl expose deployment ricxapp-ric-app-ml --port 5000 --target-port 5000 --protocol SCTP -n ricxapp --type=NodePort

However, Kubernetes will reroute the xApp's port to another port that is not 5000, and we need to search for this port by finding the new Kubernetes service that we just created. Run the following command to get a list of all the services:

.. code-block:: rst

    sudo kubectl get svc -A

Look for ricxapp-ric-app-ml. On the same row in the terminal you should see a set of ports that look like 5000:3XXXX/SCTP. An example is shown below:

.. code-block:: rst

    ricxapp       ricxapp-ric-app-ml            NodePort    10.109.106.34    <none>        5000:30255/SCTP   34m

In the above case, we want to use port 30255, as that is the port to access the xApp's SCTP interface from our local IP address.

Let's store this xApp port in a variable to use later. Replace <xapp port> with the port you found in the previous command.

.. code-block:: rst

    export XAPP_PORT=<xapp port>

First, start the EPC if you haven't already:

.. code-block:: rst

	sudo srsepc/src/srsepc

Before starting the base station, make sure you have the local IP address that you found from the previous steps.

.. code-block:: rst

    hostname -I
    export HOST_IP=<ip address>

Then, we can start the base station, which will connect to the xApp immediately on startup:

.. code-block:: rst

    sudo srsenb/src/srsenb --ric.agent.log_level=debug --log.filename=stdout --ric.agent.remote_ipv4_addr=$HOST_IP --ric.agent.remote_port=$XAPP_PORT --ric.agent.local_ipv4_addr=$HOST_IP --ric.agent.local_port=38071

You should see srsENB connect to the xApp and start sending I/Q data. You will also see E2-like commands being sent.

The I/Q data will be empty until we connect a UE. Start the UE and it should connect, initiating I/Q data transfer.

.. code-block:: rst

    sudo srsue/src/srsue

If you view the logs of the xApp, you should see the I/Q data being received and the predictions being made by the xApp. These predictions are random and are not based on the I/Q data, but the xApp receives the I/Q data and creates valid spectrograms, so you can modify the code to handle the spectrograms however you would like.


Undeployment
-----------------

Undeploy xApp using App Manager:

.. code-block:: rst

    export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    curl -L -X DELETE http://${APPMGR_HTTP}:8080/ric/v1/xapps/ric-app-ml

Remove xApp's chart from xApp onboarder:

.. code-block:: rst

    export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`
    curl -L -X DELETE "http://${ONBOARDER_HTTP}:8080/api/charts/<xApp_name>/<xApp_tag>"

Undeploy/redeploy the entire RIC:

.. code-block:: rst

    cd ~/oaic/RIC-Deployment/bin/
    sudo ./undeploy-ric-platform

    sudo ./deploy-ric-platform -f ../RECIPE_EXAMPLE/PLATFORM/example_recipe_oran_e_release_modified_e2.yaml

Delete additional port for E2-like interface:
``sudo kubectl delete service ricxapp-ric-app-ml -n ricxapp``


Troubleshooting
---------------

**Kong is stuck in CrashLoopBackOff!**

If Kong is not working in your near-RT RIC, you will not be able to deploy the xApp with the above commands. However, we can directly access the xApp Onboarder and App Manager's IP addresses and bypass the Kong proxy.

For the `Onboard and deploy the xApp` section, use the following commands instead:

Get the IP addresses for the necessary pods:

.. code-block:: rst

    export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`

Submit our onboard URL file to the xApp onboarder:

.. code-block:: rst

	curl -L -X POST "http://$ONBOARDER_HTTP:8888/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@ml-onboard.url"

Deploy the xApp:

.. code-block:: rst

	curl -L -X POST "http://$APPMGR_HTTP:8080/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "ric-app-ml"}'


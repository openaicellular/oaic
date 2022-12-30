OAIC-T Test xApp Deployment
====================================

Pre-requisites
==============


We are going to deploy oaict test xApp step-by-step. Please follow the steps to see the results.

.. note::

	The near-RT RIC installation, KPI1 xApp deployment (upgraded version of KPIMON xApp) have to be completed, and your RAN network should be up! before proceeding with the following sections.

Hosting the config Files
========================

First, please clone the xApp to the oaic repository:

.. code-block:: rst
	
	git clone <github address for this xApp>

Hosting the config Files
========================

We first need to host the config file (xApp descriptor) provided in the web server we have created already. Follow the instructions to :ref:`create a Web server <createnginx>` and continue with next steps

Copy xapp's config file to config-files directory. Reload Nginx once this has been done. 

.. code-block:: rst
	
	cp -R ./sc3-config-file.json /var/www/xApp_config.local/config_files/
  nginx -t
	sudo systemctl reload nginx

Now, you can check if the config file can be accessed from the newly created server. Place all files you want to host in the ``config_files`` directory

.. code-block:: rst
	
	export MACHINE_IP=`hostname  -I | cut -f1 -d' '`
	curl http://${MACHINE_IP}:5010/config_files/sc3-config-file.json

We host the config file on the web server to make it possible for the onboarder to download and use it for deployment.

Creating xApp Docker image
=================================

Here, we are going to build a docker image of our xApp using the given docker file.

.. code-block:: rst

	cd sc3
	sudo docker build . -t xApp-registry.local:5008/sc3:0.0.1
	

xApp Onboarder Deployment
=========================

Get Variables
-------------

Now, we need to get some variables of RIC Platform ready. The following variables represent the IP addresses of the services running on the RIC Platform.

.. code-block:: rst

	export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
	export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
	export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`


Check for helm charts
---------------------

Get helm charts and check if the current xApp is one of them.

.. code-block:: rst

	curl --location --request GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"

Update onboarder file
---------------------

We need to update ``xApp-onboarder`` to the Ngnix server to get the xApp descriptor file and use it to create a helm chart and deploy the xApp. Please replace the existing "machine_ip_addr" with your machine ip address, You can find this out through ``ifconfig``.

.. code-block:: rst

	vim sc3-onboard.url	
	{"config-file.json_url":"http://<machine_ip_addr>:5010/config_files/sc3-config-file.json"}

Save the file. Now we are ready to deploy the xApp. 

Deploying the xApp
------------------

.. code-block:: rst

	curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@sc3-onboard.url"
	curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"    
	curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "sc3"}'


Verifying xApp Deployment
-------------------------

There should be a ``ricxapp-sc3`` pod in ``ricxapp`` namespace

.. code-block:: rst

	sudo kubectl get pods -A

We can check the xApp logs using

.. code-block:: rst

	kubectl logs -f -n ricxapp -l app=ricxapp-sc3
  
  

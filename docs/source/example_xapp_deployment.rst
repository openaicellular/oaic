.. _kpimon_deployment:

======================
KPIMON xApp Deployment
======================

Here we are going to deploy KPIMON xApp as an example of xApp deployment. Please follow the steps to see the results.

.. note::

	The near-RT RIC installation has to be completed before proceeding with the following sections.


Hosting the config Files
========================

As indicated in the :ref:`Genral guidelines to Deploy an xApp <xappdeployment>` section , we first need to host the config file (xApp descriptor) provided in the web server we have created already. Follow the instructions to :ref:`create a Web server <createnginx>` and continue with next steps

Create a symlink from the xapp's config file (in this case kpimon).This can be replaced by another symlink in the future. Reload Nginx once this has been done. 

.. code-block:: rst
	
..	
        sudo ln -s ric-scp-kpimon/scp-kpimon-config-file.json /var/www/xApp_config.local/config_files/scp-kpimon-config-file.json
	
        sudo cp ric-scp-kpimon/scp-kpimon-config-file.json /var/www/xApp_config.local/config_files/
        sudo systemctl reload nginx

Now, you can check if the config file can be accessed from the newly created server. Place all files you want to host in the ``config_files`` directory

.. code-block:: rst
	
	export MACHINE_IP=`hostname  -I | cut -f1 -d' '`
	curl http://${MACHINE_IP}:5010/config_files/scp-kpimon-config-file.json

We host the config file on the web server to make it possible for the onboarder to download and use it for deployment.

Creating KPIMON xApp Docker image
=================================

Now, we create a docker image of the KPIMON xApp using the given docker file.

.. code-block:: rst

	cd ric-scp-kpimon
	sudo docker build . -t xApp-registry.local:5008/scp-kpimon:1.0.1
	

xApp Onboarder Deployment
=========================

.. warning::

	Before Deploying the xApp, it is essential to have the :ref:`5G Network Up and Running <xappdeployment>`. Otherwise the subscription procedure will not be successful.

Get Variables
-------------

First, we need to get some variables of RIC Platform ready. The following variables represent the IP addresses of the services running on the RIC Platform.

.. code-block:: rst

	export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
	export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
	export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`


Check for helm charts
---------------------

Get helm charts and check if the current xApp is one of them. If there is no helm chart, then we are good to go. Otherwise, we have to use the existing chart or delete it and then proceed forward.

.. code-block:: rst

	curl --location --request GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"

Create a .url file
------------------

Next, we need to create a ``.url`` file to point the ``xApp-onboarder`` to the Ngnix server to get the xApp descriptor file and use it to create a helm chart and deploy the xApp.

.. code-block:: rst

	vim scp-kpimon-onboard.url	

Paste the following in the ``onboard.url`` file. Substitue the ``<machine_ip_addr>`` with the IP address of your machine. You can find this out through ``ifconfig``.

.. code-block:: rst

	{"config-file.json_url":"http://<machine_ip_addr>:5010/config_files/scp-kpimon-config-file.json"}

Save the file. Now we are ready to deploy the xApp. 

Deploying the xApp
------------------

.. code-block:: rst

	curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@scp-kpimon-onboard.url"
	curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"    
	curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "scp-kpimon"}'


Verifying xApp Deployment
-------------------------

There should be a ``ricxapp-scp-kpimon`` pod in ``ricxapp`` namespace

.. code-block:: rst

	sudo kubectl get pods -A

We can check the xApp logs using

.. code-block:: rst

        sudo kubectl logs -f -n ricxapp -l app=ricxapp-scp-kpimon

Since the E2 Node is already up and running and the Key Performance Metrics (KPM) RAN function is enabled by default, the xApp will be able to subscribe to the E2 Node and start getting **INDICATION** messages. The decoded message containing information about the metrics is stored in the ``kpimon.log`` within the pod. This can be viewed by,

.. code-block:: rst

	sudo kubectl exec -it -n ricxapp `sudo kubectl get pod -n ricxapp -l app=ricxapp-scp-kpimon -o jsonpath='{.items[0].metadata.name}'` -- tail -F /opt/kpimon.log

xApp Re-Deployment & Undeployment
=================================

To redeploy the xApp, on the near-RT RIC side, run the command

.. code-block:: rst

	sudo kubectl -n ricxapp rollout restart deployment ricxapp-scp-kpimon

To Undeploy the xApp, first let's get the IP address of the ``App Manager``

.. code-block:: rst

	export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
	curl -L -X DELETE http://${APPMGR_HTTP}:8080/ric/v1/xapps/scp-kpimon
	
To remove the xApp descriptors from the Chart Museum, use the command

.. code-block:: rst

	curl -L -X DELETE "http://${ONBOARDER_HTTP}:8080/api/charts/scp-kpimon/1.0.1"
 







  
  









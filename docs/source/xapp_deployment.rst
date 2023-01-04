.. _xappdeployment:

====================================
xApp Deployment - General Guidelines
====================================

Pre-requisites
==============

Hosting Files on a Web Server 
-----------------------------

The xApp descriptor files (config.json) must be hosted on a webserver when we use the **xapp-onboarder** to deploy xApps. To host these files we use Nginx to create a web server.

.. _createnginx:

Configuring the Nginx Web server
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

First, we need to install Nginx and check if it is in ``active (running)``  state. 

.. code-block:: rst

	sudo apt-get install nginx
	sudo systemctl status nginx

Unlink the default Configuration file and check if it is unlinked

.. code-block:: rst

	cd /etc/nginx/sites-enabled
	sudo unlink default
	cd ../

Now we create some directories which can be accessed by the server and where the config files can be hosted

.. code-block:: rst

	cd ../../var/www
	sudo mkdir xApp_config.local
	cd xApp_config.local/
	sudo mkdir config_files

Create a Custom Configuration File and define file locations

.. code-block:: rst
	
	cd ../../../etc/nginx/conf.d
	sudo vim xApp_config.local.conf

Paste the following content in the *conf* file.

.. code-block:: rst  

	server {
	    listen 5010 default_server;
	    server_name xApp_config.local;
	    location /config_files/ {

		root /var/www/xApp_config.local/;
	    }

	}


Save the configuration file and check if there are any errors in the configuration file.

.. code-block:: rst

	sudo nginx -t

.. _hostxAppdescriptor:

Hosting the config Files
~~~~~~~~~~~~~~~~~~~~~~~~

Copy the xApp config file to this directory. Reload Nginx once this has been done

.. code-block:: rst
	
	sudo cp <path_to_config_file>/config-file.json /var/www/xApp_config.local/config_files/
	sudo systemctl reload nginx

Now, you can check if the config file can be accessed from the newly created server. Place all files you want to host in the ``config_files`` directory

.. code-block:: rst

	curl http://<machine_ip_addr>:5010/config_files/config-file.json

.. tip::

	* Place all files you want to host in the ``config_files`` directory.
	* Make sure all files have a unique name


Creating xApp Docker Image
==========================


We create the xapp image using the given Dockerfile within the xApp repository.

.. code-block:: rst

	docker build . -t xApp-registry.local:5008/<xapp-image-name>:<version> (Example : xappkpimon:1.0.0).

What we are essentially doing here is that we are storing our built image in a local docker repository. This is done to keep things simple. We could use cloud servers too (in that case the image needs to be pushed to the respective server using ``docker push``).


xApp Onboarder Deployment
=========================

Getting Variables ready

.. code-block:: rst

	export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
	export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
	export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`

Get helm charts and check if the current xApp is one of them. If there is no helm chart, then we are good to go. Otherwise, we have to use the existing chart or delete it and then proceed forward.

.. code-block:: rst

	curl --location --request GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"

Now, we need to indicate in the xapp descriptor ``config-file.json`` to use the image we built in the previous step. To do this, in the ``image`` section edit the ``registry`` field to  ``xApp-registry.local:5008``, ``name`` to ``<xapp-image-name>`` and ``tag`` to ``<version>``. An example is given below

.. code-block:: rst

	 "image": {
                "registry": "xApp-registry.local:5008",
                "name": "<xApp-image-name>",
                "tag": "<version>"
	}

Save the xApp descriptor file and :ref:`host <hostxAppdescriptor>` it in the Nginx server we previously created. Also, perform the check to see if the config-file is hosted on the server.


Next, we need to create a ``.url`` file to point the ``xApp-onboarder`` to the Ngnix server to get the xApp descriptor file and use it to create a helm chart and deploy the xApp.

.. code-block:: rst

	vim <xApp-name>-onboard.url	

Paste the following in the ``onboard.url`` file. Substitue the ``<machine_ip_addr>`` with the IP address of your machine. You can find this out through ``ifconfig``.

.. code-block:: rst

	{"config-file.json_url":"http://<machine_ip_addr>:5010/<xApp-name->config-file.json"}

Save the file. Now we are ready to deploy the xApp. 

.. code-block:: rst

	curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@<xApp-name>-onboard.url"
	curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"    
	curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "scp-kpimon"}'

Verify if the xApp is deployed. There should be a <xApp-name> pod in "ricxapp" namespace

.. code-block:: rst

	sudo kubectl get pods -A

We can check the xApp logs using

.. code-block:: rst

	kubectl logs -f -n ricxapp -l app=<xApp-pod-name>



DMS-CLI Deployment
==================



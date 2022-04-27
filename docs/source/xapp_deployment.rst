====================================
xApp Deployment - General Guidelines
====================================

Pre-requisites
==============

Create an xApp Image Registry
-----------------------------

First we need to create a registry to host our xApp images. Here we create a local docker registry called ``xApp-registry.local:5008``. 5008 indicates the port number on which the registry is listening. 

.. code-block:: rst
	

Hosting Files on a Web Server 
-----------------------------

The xApp descriptor files (config.json) must be hosted on a webserver when we use the **xapp-onboarder** to deploy xApps. To host these files we use Nginx to create a web server.


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

	nginx -t

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

	docker build . -t xApp-registry.local:5008/<xapp-name>:<version> (Example : xappkpimon:1.0.0).

What we are essentially doing here is that we are storing our built image in a local docker repository. This is done to keep things simple. We could use cloud servers too (in that case the image needs to be pushed to the respective server using docker push).

Now that we have the docker image, we need to push it to our local registry ``xApp-respository.local:5008``

.. code-block:: rst 

	docker push xApp-repository.local:5008/<xapp-name>:<version>
	

xApp Onboarder Deployment
=========================

Getting Variables ready

.. code-block:: rst

	export KONG_PROXY=`kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
	export APPMGR_HTTP=`kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
	export ONBOARDER_HTTP=`kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`

Get helm charts and check if the current xApp is one of them. If there is no helm chart, then we are good to go. Otherwise, we have to use the existing chart or delete it and then proceed forward.

.. code-block:: rst

	curl --location --request GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"


DMS-CLI Deployment
==================



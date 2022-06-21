.. _kpimon_deployment:

======================
KPIMON xApp Deployment
======================

Here we are going to deploy KPIMON xApp as an example of xApp deployment. Please follow the steps to see the results.

.. note::

	The near-RT RIC installation has to be complete before proceeding with the following sections.

As indicated in the General xApp guidelines section , we 

Get some variables ready
~~~~~~~~~~~~~~~~~~~~~~~~

First, we need to get some variables of RIC Platform ready. The following variables represent the IP addresses of the services running on the RIC Platform.

.. code-block:: rst
   
  export KONG_PROXY=`kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
  export E2MGR_HTTP=`kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2mgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
  export APPMGR_HTTP=`kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
  export E2TERM_SCTP=`kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`
  export ONBOARDER_HTTP=`kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`
  export RTMGR_HTTP=`kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-rtmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`

Check helm charts to see if KPIMON is one of them
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If there is no helm chart for KPIMON, it's good to continue. Otherwise, we have to use the existing chart or delete it and then proceed forward.

.. code-block:: rst

	curl --location --request GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"

Build Docker image of KPIMON
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Build the Docker image of KPIMON xApp:
First, clone the KPIMON and change the file's content to e2sm-kpm-01.00 commit:

.. code-block:: rst

	git clone https://gitlab.flux.utah.edu/powderrenewpublic/ric-scp-kpimon.git
	cd ric-scp-kpimon
	git checkout revert-to-e2sm-kpm-01.00

Now you have a Docker image in ric-scp-kpimon directory that should be replaced with the existing Docker image in OAIC github repository. First download the Docker file and:

.. code-block:: rst

	cp -R /home/<directory>/Dockerfile /home/<directory>/ric–scp–kpimon 
  
Second, build the docker image:

.. code-block:: rst

	sudo docker build . -f Dockerfile --tag docker-repository.local:5000/scp-kpimon:1.0.1
  
Third, push the docker image:

.. code-block:: rst

	sudo docker push localhost:5000/scp-kpimon:1.0.1
  
If a local registry is not present, create it with name "docker-repository.local" and give it port number '5000':

.. code-block:: rst

	sudo docker run -d -p 5000:5000 --restart=always --name docker-repository.local registry:2
  
Host & Onboard the xApp descriptor 'config-file.jason'
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The xApp descriptor files (config.json) must be hosted on a webserver when we use the **xapp-onboarder** to deploy xApps. To host these files we use Nginx to create a web server. Next, we need to create a .url file to point the xApp-onboarder to the Ngnix server to get the xApp descriptor file and use it to create a helm chart and deploy the xApp. Please follow instruction on 'xApp Deployment' section <https://openaicellular.github.io/oaic/xapp_deployment.html>, to host the config-file.json of KPIMON xApp on a server. Just consider to use 'scp-kpimon' for 'xApp-name'

Deploy the KPIMON xApp
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now we are ready to deploy the xApp:

.. code-block:: rst

  curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@scp-kpimon-onboard.url"
  curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts
  curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "scp-kpimon"}'
  
The xApp should be successfully deployed. Verify this using:
  
.. code-block:: rst

  sudo kubectl get pods -A



  









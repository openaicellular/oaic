===============
xApp Deployment
===============

Creating xApp Docker Image
==========================

First we need to create a registry to host our xApp images. Here we create a local docker registry called ``xApp-repository.local:5008``. 5008 indicates the port number on which the registry is listening. 

.. code-block::rst
	
	

Now, we need the xapp image using the given Dockerfile within the xApp repository.

.. code-block:: rst

	docker build . -t xApp-repository.local:5008/<xapp-name>:<version> (Example : xappkpimon:1.0.0).

What we are essentially doing here is that we are storing our built image in a local docker repository. This is done to keep things simple. We could use cloud servers too (in that case the image needs to be pushed to the respective server using docker push).

Now that we have the docker image, we need to push it to our local registry ``xApp-respository.local:5008``

.. code-block::rst 

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



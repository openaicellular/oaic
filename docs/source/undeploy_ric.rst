========================
UnDeploying RIC Platform
========================
To undeploy the near-RT RIC platform, we can use the scripts provided

.. code-block:: rst

	cd /oaic/RIC-Deployment/bin
	sudo ./undeploy-ric-platform
	
Un-deploying and Re-deploying xApps
===================================

To redeploy the xApp, on the near-RT RIC side, run the command

.. code-block:: rst

	sudo kubectl -n ricxapp rollout restart deployment <pod_name>

To Undeploy the xApp, first let's get the IP address of the ``App Manager``

.. code-block:: rst

	export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
	curl -L -X DELETE http://${APPMGR_HTTP}:8080/ric/v1/xapps/<xApp_name>
	
To remove the xApp descriptors from the Chart Museum, we first need to get the ``Onboarder's IP address`` use the command

.. code-block:: rst
	
	export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`
	curl -L -X DELETE "http://${ONBOARDER_HTTP}:8080/api/charts/<xApp_name>/<xApp_tag>"
 

.. code-block:: rst

	kubectl -n ricxapp rollout restart deployment <pod_name>




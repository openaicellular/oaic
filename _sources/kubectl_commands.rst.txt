==============================
Kubectl Commands - Cheat Sheet
==============================

Check the Kubernetes pods and services
======================================

View all pods deployed

.. code-block:: rst

	sudo kubectl get pods -A

View all services offered

.. code-block:: rst

	sudo kubectl get services -A

View pods in a particular namespace. For example, the namespace can be ``ricplt``, ``ricxapp`` or ``ricinfra``

.. code-block:: rst

	sudo kubectl get pods -n <namespace>  or sudo kubectl get pods -n ricplt

View services in a particular namespace

.. code-block:: rst

	sudo kubectl get services -n ricplt

View logs of Kubernetes pods
============================

.. code-block:: rst

	sudo kubectl logs -f -n <namespace> -l app=<pod_name>

In our setup the <namespace> can be ``ricplt``, ``ricxapp`` or ``ricinfra``. An example would look like the following

.. code-block:: rst 

	sudo kubectl logs -f -n ricplt -l app=ricplt-e2mgr

Run commands in a Kubernetes pod
================================

.. code-block:: rst

	sudo kubectl exec -it -n <namespace> <pod_name> <command>

Get the full pod name
---------------------

.. code-block:: rst

	sudo kubectl get pod -n ricxapp -l app=<pod_name> -o jsonpath='{.items[0].metadata.name}'

Get the IP of a service offered by a pod
----------------------------------------

.. code-block:: rst

	sudo kubectl get svc -n ricxapp --field-selector metadata.name=<service_name> -o jsonpath='{.items[0].spec.clusterIP}'



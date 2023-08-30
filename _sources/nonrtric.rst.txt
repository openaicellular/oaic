===========================================
O-RAN Non-Real Time RIC Installation Guide
===========================================


Pre-requisites
==============

System Requirements
-------------------
For Non-SMO installation

.. note::

    We recommend you to run these installation procedures on a VM with the specs below


* OS: `Ubuntu Live server 20.04 LTS (Focal Fossa) <https://releases.ubuntu.com/focal/ubuntu-20.04.6-live-server-amd64.iso>`_
* CPU(s): 6 vCPUs
* RAM: 40 GB
* Storage: 60 GB


Installing the Non-Real Time RIC
================================

First install snap and reboot.

.. code-block:: bash

    sudo apt-get install snapd -y
    sudo reboot
   
After your machine have finished rebooting git clone the following repositories into your home directory

.. code-block:: bash

    sudo git clone "https://gerrit.o-ran-sc.org/r/it/dep"
    sudo git clone --recursive https://github.com/sebdet/oran-deployment.git
   
Checkout your ``dep`` repository to master and do a git submodule update.

.. code-block:: bash

    cd ~/dep
    sudo git checkout master
    sudo git submodule update --init --recursive
   
.. note::

    If your git commands doesn't work, you may need to declare the git repository as safe with this command: ``git config --global --add safe.directory /home/YOUR_USERNAME/dep``
   
Then run these following commands to setup Kubernetes, Chart Museum, and Helm v3

.. code-block:: bash

    cd ~/
    sudo ./dep/smo-install/scripts/layer-0/0-setup-microk8s.sh
    sudo ./dep/smo-install/scripts/layer-0/0-setup-charts-museum.sh
    sudo ./dep/smo-install/scripts/layer-0/0-setup-helm3.sh
   
Build the helm charts for the Non-Real Time RIC

.. code-block:: bash

    sudo ./dep/smo-install/scripts/layer-1/1-build-all-charts.sh
   
Finally, deploy the Non-Real Time RIC

.. code-block:: bash

    sudo ./dep/smo-install/scripts/layer-2/2-install-nonrtric-only.sh standalone-nonrtric
   
Run this command to get all your pods.

.. code-block:: bash

    sudo kubectl get pods -A
   
Here are the results after you run the command.

.. image:: nonrtric_pods.png
   :width: 60%
   :alt: Non Real-time RIC Pods
   
.. note::

    It may take thirty minutes for all your pods to start running

.. vagrant:

=======================================
Installing Virtual Machine with Vagrant
=======================================

The installation can be done through initializing a base box and provisioning it for OAIC or by donwloading a pre-provisioned VM from Vagrant Cloud.


Pre-requisites
--------------

Install Vagrant: https://www.vagrantup.com/docs/installation (Warning: Do not use a package manager for the installation.)


Provision a base box
====================

To create a base box, we follow the below steps:-

Initialize Vagrant
------------------

In an empty directory, run 'vagrant init'. This creates a VagrantFile with base specifications.

Provision Vagrant for OAIC
--------------------------

For OAIC, we need to provision certain networking protocols and additional RAM and disk space. This is done by replacing the default VagrantFile with the file 'VagrantFile' included with OAIC.

Running 'vagrant up' && 'vagrant ssh' completes our setup of this base box. 

The installation of the OAIC software can now begin by running the installation script (/link to documentation script/).


Installing OAIC VM
==================

Initialize Vagrant by running 'vagrant init' in an empty directory with a minimum of 10GB of disk space.

Set up the OAIC VM using Vagrant Cloud: 
	a) Navigate to https://app.vagrantup.com/romilkb/boxes/test_oaic to access the latest vagrant box
	b) Copy the latest VagrantFile and replace the local VagrantFile with it

Run 'vagrant up'
Run 'vagrant ssh'

You now have access to a VM with ORAN and srsRAN installed. You may now setup your own 5G network. (/link to 'Setup your own 5G network'/)



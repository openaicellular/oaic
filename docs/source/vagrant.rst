.. vagrant:

=======================================
Installing Virtual Machine with Vagrant
=======================================

Pre-requisites
==============


Installation instructions go here

To create a base box, we follow the below steps:-

Install Vagrant
---------------

1. Install Vagrant: https://www.vagrantup.com/docs/installation

2. Initialize a base box:
	a) Create an empty folder ensuring disk space of 10GB
	b)  Run 'vagrant init'

3. Set up the OAIC VM using Vagrant Cloud: 
	a) Navigate to https://app.vagrantup.com/romilkb/boxes/test_oaic to access the latest vagrant box
	b) Copy Latest VagrantFile and replace the local VagrantFile with it

4. Run 'vagrant up'
5. Run 'vagrant ssh'

Initialize Vagrant
------------------

In an empty directory, run 'vagrant init'. This creates a VagrantFile with base specifications.

Provision Vagrant
-----------------

For OAIC, we need to provision certain networking protocols and additional RAM and disk space. This is done by replacing the VagrantFile with the following:-

(TBD)

Running 'vagrant up' && 'vagrant ssh' completes our setup of this base box. The installation of the OAIC software can now begin.

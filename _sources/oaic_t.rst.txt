=======================================
OAIC-T 
=======================================

About OAIC-T
------------

OAIC-T is an open-source AI cellular testing platform which supports automated, distributed, and AI-enhanced testing of xApps in O-RAN.
The OAIC-T framework consists of three major components: i). Test Input, which includes both test configuration and test script files, 
ii). the Server, which sets up the testing environment as described in test configuration files and orchestrates the test execution as 
defined in test scripts, and iii) the Actor(s), which executes test actions as instructed by the server. 

.. image:: oaci_t_framework.jpg
   :width: 90%
   :alt: OAIC-T Framework


The server manages a number of remote testing actors, including maintaining the socket connection to each actor and monitoring its resource 
usage (e.g., CPU, memory, disk, and SDR hardware) and test status. 

One actor consists of the actor manager, AI core, and test executor, as well as two adapters to interface with the unit under test 
(i.e., a cellular radio network controller). Each actor can interact with xApps/rApps under test through either the SIM Adapter or the 
SDR Adapter. The SIM Adapter acts as a testing xApp/rApp which can be deployed in a non-RT, near-RT, or RT RIC to send testing data and 
receive response to/from the RAN through socket communications. The SDR Adapter acts as a user equipment (UE) which leverages SDR-based 
software suite (e.g., srsRAN) to send radio testing signals to the RAN through antennas. 

Get Started
-------------------------

To use OAIC-T, first clone the `oaic-t <https://github.com/openaicellular/oaic-t.git>`_ repository:

.. code-block:: rst

   git clone https://github.com/openaicellular/oaic-t.git


.. note::

	The OAIC-T now only supports the testing of O-RAN under ZeroMQ mode, i.e., both eNodeB and UE are running in the same machine without 
	the use of SDRs, also known as virtual radio. Also the srsRAN has to be installed and configured in a way that a UE can be started by 
	running the command like "sudo srsue ...". This involves to i). set srsRAN/UE path to the environment, and ii). configure the system 
	to run sudo commands without typeing password.	

ZeroMQ Mode
===========

This test example "test_virtual_traffics.json" can creates a network namespace, starts a UE, and generates downlink/uplink traffics to the eNodeB. 

.. note::

	The EPC and the eNodeB must be started before running this test example. 	
	
Open a terminal, and start the Server first:

.. code-block:: rst

   cd oaic-t/server/src
   python3 server_main.py test_virtual_traffics.json

Open a terminal, and then start one Actor:

.. code-block:: rst

   cd oaic-t/actor/src
   python3 actor_main.py
   
More actors can be started, but each actor should have a unique name. Edit the actor/src/config.txt to change the actor name before running it.

More test script examples and test actions will be provided later.

SDR Mode
========

To be released soon.


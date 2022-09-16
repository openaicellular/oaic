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


How To Run OAIC-T, assuming both server and actor run in the same machine:
Step 1. Run the OAIC-T Server: 

.. code-block:: rst
	cd server/src 
	python3 server_main.py 
	
Step 2. Run the OAIC-T Actor(s): 

.. code-block:: rst
	cd actor/src
	sudo python3 actor_main.py

More actors can be started, but each actor should have a unique name. Edit the actor/src/config.txt to change the actor name before running it.

Once the actor starts, you will see a message in the server console showing one Actor is registered with its name.
Then, you can type commands in the console to interact with the Server. The following commands are currently supported:

1. "list actors": It will list all registered Actors.
2. "run --test test_script_file --actor actor_name": It will run the test script in the specified actor. Currently six test script exampls are included (more will become available soon):

"start_epc.json": This script will run the srsepc (Note: only one running epc is allowed in one actor).

"stop_epc.json": This script will stop the running srsepc.

"start_enodeb.json": This script will run the srsenb (Note: only one running enodeb is allowed in one actor).

"stop_enodeb.json": This script will stop the running srsenb.

"test_virtual_traffics.json": This script will run a UE and generate traffics using ping methods (Note: epc and enodeb have to be started before starting the UE, either mannually starting them or using the above two scripts).

"test_virtual_traffics_iperf.json": Similar to the "test_virtual_traffics.json" script, but using the iperf method which allows to generate traffics with specific bandwidth, e.g., 10Mbps for 20 seconds.

"test_virtual_traffics_all.json": This script will include a whole workflow to generate traffics, combining "start epc", "start enodeb", "generate traffics using ping", "stop ue", "stop enodeb" and "stop epc" actions. 


More test script examples and test actions will be provided later.

SDR Mode
========

To be released soon.


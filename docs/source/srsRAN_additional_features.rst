==========================
srsRAN Additional Features
==========================

.. _multiple_ue:

Multiple Simulated UEs using GNU Radio Companion
================================================

GNU Radio Companion (GRC) can be used to manipulate, and/or visualize base band I/Q data as it is sent between the UE and eNB. GRC contains ZeroMQ compatible blocks that are connected to the TCP ports used to transmit data between the UE and eNB. The Broker basically acts as a channel for information flow.

Installing GNU Radio Companion
------------------------------

.. warning::
   
   Make sure you do not have GNU Radio already installed in your system. If so, uninstall previous versions. Also, remove other gnuradio ppas if already configured. 

`GNU Radio Installation <https://wiki.gnuradio.org/index.php/InstallingGR>`_

Installation From Binaries
~~~~~~~~~~~~~~~~~~~~~~~~~~

To access the current released version (3.10), add the gnuradio/gnuradio-releases ppa, update the sources and install gnuradio

.. code-block:: rst
  
	sudo add-apt-repository ppa:gnuradio/gnuradio-releases
	sudo apt-get update
	sudo apt install gnuradio

GNU Radio 3.10 will need *python packaging*. 

.. code-block:: rst

	sudo apt install python3-pip
	pip install packaging


.. tip::

	(**Optional**) If you need to increase performance, you can run ``volk_profile``. If you encounter *cannot import gnuradio* error, follow the steps given `here <https://wiki.gnuradio.org/index.php?title=ModuleNotFoundError#B._Finding_the_Python_library>`_.


Connecting to an external server/internet
=========================================

To allow UE to connect to the internet via the EPC, the pre-configured masquerading script ``srsepc_if_masq.sh`` must be run . The script can be found in `srsRAN-e2/srsepc <https://github.com/openaicellular/srsRAN-e2/tree/test_branch/srsepc>`_. The masquerading script enables IP forwarding and sets up Network Address Translation to pass traffic between the srsRAN network (through EPC) and the external network. 

.. note::

	The script must be run each time the machine is re-booted, and can be done before or while the network is running. Here we setup the network first.
	Run the ``srsepc`` and ``srsenb``. Please check the tutorial `:ref:here <>` for the steps. 

Running the above script requires the network interface which is connected to the external network/internet to be passed as an input argument. To identify this run

.. code-block:: rst

	route

You will see an output similar to the following:

.. code-block:: rst 

	Kernel IP routing table
	Destination     Gateway       Genmask        Flags      Metric  Ref    Use      Iface
	default         192.168.1.1   0.0.0.0        UG         600     0        0      wlp2s0
	10.0.2.0        0.0.0.0       255.255.255.0  U          100     0        0      enp0s3
	link-local      0.0.0.0       255.255.0.0    U          1000    0        0      wlp2s0
	192.168.1.0     0.0.0.0       255.255.255.0  U          600     0        0      wlp2s0

The interface (*Iface*) associated with the **default** destination is one which must be passed into the masq. script. In the above output that is the ``wlp2s0`` (indicates wifi) interface.

The ``enp0s3`` usually indicates ethernet interface. 

Now run the Masquerading script. Plug in the interface you obtained using the command above.

.. code-block:: rst 

	sudo ./srsepc_if_masq.sh <interface>

If the script executed successfully you will get 

.. code-block:: rst 
	
	Masquerading Interface <interface>

.. warning::

	Make sure the UE machine is not connected to internet.

Run the ``srsue``. On successful connection (**RRC connected**) A virtual network interface named **tun_srsue** will be created on the UE machine and it will be assigned an IP address by the EPC with format ``172.16.0.x``.
Here we assume that the UE is assigned an IP address ``172.16.0.2``

Check if you can ping the EPC (SGi IP address: ``172.16.0.1``)

.. code-block:: rst 
	
	ping 172.16.0.1

Once this is successful, we will configure the EPC to act as a gateway to the external network/internet for the UE. On the UE machine run

.. code-block:: rst

	sudo route add default gw 172.16.0.1 tun_srsue

That's it. Test by pinging Google's DNS server

.. code-block:: rst

	ping 8.8.8.8


Connecting USRPs to VirtualBox VMs
==================================


Connecting B210/B-205 mini over USB 3.0
---------------------------------------


Connecting X310/N310 through ethernet
-------------------------------------

.. tip::

	If your machine, does not support 10G Ethernet then you can use adapters (USB-C/USB3.0 to 10G ethernet adapters)

.. admonition:: TODO

	Add images here. 
	USRP IP address will change depending on the FPGA Image that it loaded. Indicate this in the instructions below.

Make sure the USRP is connected to the machine's secondary NIC (Ethernet in this case) and the VM is not running. In VirtualBox's Settings window, click the ``Network`` tab, and then ``Adpater 2``. Leave ``Adapter 1`` alone so that you still have Internet access inside the VM. In ``Attached to``, choose ``Bridged Adapter`` so that the VM has direct access to the physical network adapter. Then in ``Name``, choose the **secondary NIC (ethernet)** that is physically connected to the USRP.

Save the configuration and boot into the Virtual Machine.

Make sure the network adapter is visible.

.. code-block:: rst

	ifconfig -a
	sudo ifconfig enp0s8 192.168.10.1
	sudo ip route add 192.168.10.0/24 dev enp0s8
	ip route
	sudo ifconfig enp0s8 up
	ping 192.168.10.2
	uhd_find_devices

Now you should be able to ping the USRP from the VM


.. _performance_tips:

UHD USRP Performance Enhancement Tips and Tricks
================================================


COTS UE Configuration
=====================


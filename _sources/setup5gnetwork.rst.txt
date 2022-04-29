
=======================
Starting EPC/NodeB/UE(s)
=======================

In a new command windows run srsRAN EPC:

::

        sudo srsepc

In a new command windows run srsRAN eNodeB.

::


        sudo srsenb

In a new command windows run UE, placing its network interface into a separate network namespace:

Creating Namespace: .. code-block:: rst
::

        sudo ip netns add ue1

Run srsUE with the new assigned namespace .. code-block:: rst
::

    sudo srsue –gw.netns=ue1

Send some simple signaling-like traffic from UE to EPC, Perform commands from ue1: 

.. code-block:: rst

    sudo ip netns exec ue1 ping 172.16.0.1

You can find configuration files and database file related to ‘ue1’, ‘enb’, ‘epc’ in /root/.config/ directory. If you want to change the contents of the ‘ue.config’ and ‘user_db.csv’ files such as IMSI and other values, make sure to first kill the EPC process, then modify, then restart EPC. The EPC process updates this file when it exits.

.. _gettingstarted:

Getting Started
===============

Introduction
------------

OAIC is an open-source effort led by a consortium of academic institutions to provide fully open-source software architecture, library, and toolset that encompass both the AI controllers (OAIC-C) as well as an AI testing framework (OAIC-T). This project will develop a software infrastructure that spurs research and development on AI-enabled cellular radio networks. We leverage existing open-source 5G software efforts to create a framework which integrates AI controllers into 5G processing blocks and extends the scope of the Open Radio Access Network (O-RAN) framework, the industry standard for future RANs.


Structure of OAIC
-----------------

For deployment please follow structure below, also we've provided step-by-step installation videos to help you for better understanding of our deployment structure:
.. code-block:: rst

   https://www.youtube.com/watch?v=LMui8HqKdyg&t=315s
   https://www.youtube.com/watch?v=R9gCBWsLZJw&t=1s
   https://www.youtube.com/watch?v=dWhrVLuswaQ

To use Open AI Cellular, first clone the `oaic <https://github.com/openaicellular/oaic.git>`_ repository:

.. code-block:: rst

   git clone https://github.com/openaicellular/oaic.git

oaic is the root directory for all purposes. Different components of OAIC like the Near-real time RIC, srsRAN and xApps are linked as submodules in this directory. Performing an update of the submodules will provide all the files required to run OAIC.

Check out the latest version of every dependent submodule within the “oaic” repository.

.. code-block:: rst

    cd oaic    
    git submodule update --init --recursive --remote


How to Become a Contributor
---------------------------

OAIC will facilitate collaboration to build an open-source software development environment and user community.
*More info coming Soon*


Source Code
-----------

`OAIC on Github <https://github.com/openaicellular/oaic>`_


Research Papers
---------------
*Coming Soon*


Mailing List
------------




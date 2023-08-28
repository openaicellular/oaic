srsRAN with E2 Agent
====================

[![Build Status](https://app.travis-ci.com/srsran/srsRAN.svg?branch=master)](https://app.travis-ci.com/github/srsran/srsRAN)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/srsran/srsRAN.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/srsran/srsRAN/context:cpp)
[![Coverity](https://scan.coverity.com/projects/23045/badge.svg)](https://scan.coverity.com/projects/srsran)

srsRAN is a 4G/5G software radio suite developed by [SRS](http://www.srs.io). This is a modified version of srsRAN 21.10 and POWDER's E2 agent enabled srsLTE. 

See the [srsRAN project pages](https://www.srsran.com) for information, guides and project news.

The srsRAN suite includes:
  * srsUE - a full-stack SDR 4G/5G-NSA UE application (5G-SA coming soon)
  * srsENB - a full-stack SDR 4G/5G-NSA eNodeB application (5G-SA coming soon)
  * srsEPC - a light-weight 4G core network implementation with MME, HSS and S/P-GW

For application features, build instructions and user guides see the [srsRAN documentation](https://docs.srsran.com).


For license details, see LICENSE file.

## Pre-requisites
* System Requirements - 4 core CPU (3 - 5 GHz)
* Operating system - Ubuntu 18.04
* E2 Agent Integration - E2 Bindings, asn1c Compiler, O-RAN Specification documents(optional)
* Simulated 1 UE 1 eNB/gNB setup - ZeroMQ libraries, Single Host machine/VM
* USRP frontend - UHD version 4.1, At least two host machines/VMs
* Multiple simulated UE and eNB/gNB support : GNU Radio companion 3.8

## Installation Procedure
First, we need to install ZeroMQ and UHD Libraries
Create a new directory to host all the files related to srsRAN

`mkdir -p srsRAN-OAIC`

### Getting ZeroMQ development Libraries
https://docs.srsran.com/en/latest/app_notes/source/zeromq/source/index.html

**Package Installation**

`sudo apt-get install libzmq3-dev`

**Installing from Sources**

1. Get libzmq
```
git clone https://github.com/zeromq/libzmq.git
cd libzmq
./autogen.sh
./configure
make
sudo make install
sudo ldconfig
cd ..
```

2. Get czmq
```
git clone https://github.com/zeromq/czmq.git
cd czmq
./autogen.sh
./configure
make
sudo make install
sudo ldconfig
cd ..
```

### Installing UHD 4.1 

Make sure you don't have UHD already installed in your system.

https://files.ettus.com/manual/page_install.html

**Using package manager**

`sudo apt-get install libuhd-dev libuhd4.1.0 uhd-host`

**Using Binaries**
```
sudo add-apt-repository ppa:ettusresearch/uhd
sudo apt-get update
sudo apt-get install libuhd-dev libuhd4.1.0 uhd-host
```

**Installation from source**
https://files.ettus.com/manual/page_install.html
```
sudo apt-get install autoconf automake build-essential ccache cmake cpufrequtils doxygen ethtool \
g++ git inetutils-tools libboost-all-dev libncurses5 libncurses5-dev libusb-1.0-0 libusb-1.0-0-dev \
libusb-dev python3-dev python3-mako python3-numpy python3-requests python3-scipy python3-setuptools \
python3-ruamel.yaml 
```
```
git clone https://github.com/EttusResearch/uhd.git
cd uhd
git checkout UHD-4.1
cd host
mkdir build
cd build
cmake ../
make
sudo make install
sudo ldconfig
cd ../../../
```

### Configuring Virtual Machines to recognize Ettus USRP devices
***//TODO: Add this as an application note in readthedocs. This is a supplement. Not an essential part of readme.*** **

Support
=======

Mailing list: https://lists.srsran.com/mailman/listinfo/srsran-users

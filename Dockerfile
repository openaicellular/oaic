# Create oaic docker image from script:
#   sudo docker image build -f Dockerfile -t oaic .
#   sudo docker run -it --net=host oaic
FROM ubuntu:20.04

# Add User and Accessibility Applications
ARG MAINUSER=oaic
ARG DEBIAN_FRONTEND=noninteractive
USER root
RUN apt-get update && apt-get install -y --no-install-recommends sudo vim eog tzdata build-essential apt-utils wget && \
    useradd -ms /bin/bash ${MAINUSER} && \
    usermod -aG sudo ${MAINUSER} && \
    echo ${MAINUSER}'	ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Install base dependencies
RUN apt-get install -y --no-install-recommends nfs-common build-essential cmake libtool autotools-dev autoconf automake \
    libfftw3-dev libmbedtls-dev libboost-program-options-dev libconfig++-dev libsctp-dev libzmq3-dev

# add base directories
USER ${MAINUSER}
WORKDIR /home/${MAINUSER}
COPY --chown=${MAINUSER} . /home/${MAINUSER}/

# Install asn1c Compiler
RUN cd /home/${MAINUSER}/asn1c && \
    autoreconf -iv && ./configure && make -j4 && sudo make install && sudo ldconfig



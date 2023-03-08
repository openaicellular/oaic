# Create oaic docker image from script:
#   sudo docker image build -f Dockerfile -t oaic .
#   sudo docker run -it --net=host --privileged oaic
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
    libfftw3-dev libmbedtls-dev libboost-program-options-dev libconfig++-dev libsctp-dev libzmq3-dev \
    git gettext lsb-release kmod \
    apt-transport-https ca-certificates curl apt-utils

# install kubernetes...
# https://kubernetes.io/docs/tasks/tools/install-kubectl-linux/
# apt-get install -y apt-transport-https ca-certificates curl
RUN \
    mkdir -p /etc/apt/keyrings/ && \
    curl -fsSLo /etc/apt/keyrings/kubernetes-archive-keyring.gpg https://packages.cloud.google.com/apt/doc/apt-key.gpg && \
    echo "deb [signed-by=/etc/apt/keyrings/kubernetes-archive-keyring.gpg] https://apt.kubernetes.io/ kubernetes-xenial main" | sudo tee /etc/apt/sources.list.d/kubernetes.list && \
    apt-get update && \
    apt-get install -y kubelet kubeadm kubectl && \
    apt-mark hold kubelet kubeadm kubectl

# add base directories
USER ${MAINUSER}
WORKDIR /home/${MAINUSER}
COPY --chown=${MAINUSER} . /home/${MAINUSER}/

# Install asn1c Compiler
RUN cd /home/${MAINUSER}/asn1c && \
    autoreconf -iv && ./configure && make -j4 && sudo make install && sudo ldconfig

# Install O-RAN Near Real-Time RIC
RUN \
    cd /home/${MAINUSER}/RIC-Deployment/tools/k8s/bin && \
    ./gen-cloud-init.sh && \
    sudo ./k8s-1node-cloud-init-k_1_16-h_2_17-d_cur.sh && \
    sudo kubectl get pods -A && \
    sudo kubectl create ns ricinfra && \
    sudo helm install stable/nfs-server-provisioner --namespace ricinfra --name nfs-release-1 && \
    sudo kubectl patch storageclass nfs -p '{"metadata": {"annotations":{"storageclass.kubernetes.io/is-default-class":"true"}}}' && \
    sudo docker run -d -p 5001:5000 --restart=always --name ric registry:2 && \
    cd /home/${MAINUSER}/ric-plt-e2/RIC-E2-TERMINATION && \
    sudo docker build -f Dockerfile -t localhost:5001/ric-plt-e2:5.5.0 . && \
    sudo docker push localhost:5001/ric-plt-e2:5.5.0 && \
    cd /home/${MAINUSER}/RIC-Deployment/bin && \
    sudo ./deploy-ric-platform -f ../RECIPE_EXAMPLE/PLATFORM/example_recipe_oran_e_release_modified_e2.yaml


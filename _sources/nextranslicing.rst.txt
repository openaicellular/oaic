





NexRAN Slicing xApp Installation & running in Kubernetes
========================================================

::

    export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
    export E2MGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2mgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export E2TERM_SCTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`
    export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`
    export RTMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-rtmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`

::

    sudo docker build -f Dockerfile -t docker-repository.local:8080/nexran .
    sudo docker push docker-repository.local:8080/nexran
    sudo docker tag localhost:8080/nexran docker-repository.local:8080/nexran

Create nexran-config file (copy from nexran.sh in powder code)
::

    {"config-file.json_url":"https://cdn-123.anonfiles.com/Rfn702Bdu5/6ebc2dd4-1628643253/nexran-config-file.json"}


Create nexran-onboard.url file & copy ""
https://cdn-123.anonfiles.com/Rfn702Bdu5/6ebc2dd4-1628643253/nexran-config-file.json (copy link nexran-onboard.url as per nexran.sh file (as per line 91))

::

    curl -L -X POST \
            "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" \
            --header 'Content-Type: application/json' \
    --data-binary "@nexran-onboard.url"

    curl -L-XGET \        "http://$KONG_PROXY:32080/onboard/api/v1/charts"    

    curl -L-XPOST \        "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps"\        
    --header'Content-Type: application/json'\        
    --data-raw'{"xappName": "nexran"}'









srsRAN nexRAN Running Procedure
===============================

::

    sudo /home/pratheek/kumar/srslte-ric/build/srsepc/src/srsepc --spgw.sgi_if_addr=192.168.0.1

    export E2TERM_SCTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`

    sudo /home/pratheek/kumar/srslte-ric/build/srsenb/src/srsenb \
        --enb.n_prb=15 --enb.name=enb1 --enb.enb_id=0x19B --rf.device_name=zmq \
        --rf.device_args="fail_on_disconnect=true,id=enb,base_srate=23.04e6,tx_port=tcp://*:2000,rx_port=tcp://localhost:2001" \
        --ric.agent.remote_ipv4_addr=${E2TERM_SCTP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout \
        --slicer.enable=1 --slicer.workshare=0
::

    sudo ip netns add ue1
    sudo /home/pratheek/kumar/srslte-ric/build/srsue/src/srsue \
        --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2001,rx_port=tcp://localhost:2000,id=ue,base_srate=23.04e6" \
        --usim.algo=xor --usim.imsi=001010123456789 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 \
        --log.all_level=warn --log.filename=stdout --gw.netns=ue1

    kubectl logs -f -n ricxapp -l app=ricxapp-nexran

    export NEXRAN_XAPP=`kubectl get svc -n ricxapp --field-selector metadata.name=service-ricxapp-nexran-rmr -o jsonpath='{.items[0].spec.clusterIP}'` ; echo $NEXRAN_XAPP

    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/version ; echo ; echo

    sudo ip netns exec ue1 iperf -s -p 5010 -i 4 -t 36000

    iperf -c 192.168.0.2 -p 5010 -i 4 -t 36000

    #!/bin/sh

    SLEEPINT=4

    export NEXRAN_XAPP=`kubectl get svc -n ricxapp --field-selector metadata.name=service-ricxapp-nexran-rmr -o jsonpath='{.items[0].spec.clusterIP}'`

    echo NEXRAN_XAPP=$NEXRAN_XAPP ; echo
    
    echo Listing NodeBs: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/nodebs ; echo ; echo
    echo Listing Slices: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo
    echo Listing Ues: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo

    sleep $SLEEPINT

    echo "Creating NodeB (id=1):" ; echo
    curl -i -X POST -H "Content-type: application/json" -d '{"type":"eNB","id":411,"mcc":"001","mnc":"01"}' http://${NEXRAN_XAPP}:8000/v1/nodebs ; echo ; echo
    echo Listing NodeBs: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/nodebs ; echo ; echo

    sleep $SLEEPINT

    echo "Creating Slice (name=fast)": ; echo
    curl -i -X POST -H "Content-type: application/json" -d '{"name":"fast","allocation_policy":{"type":"proportional","share":1024}}' http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo
    echo Listing Slices: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo

    sleep $SLEEPINT

    echo "Creating Slice (name=slow)": ; echo
    curl -i -X POST -H "Content-type: application/json" -d '{"name":"slow","allocation_policy":{"type":"proportional","share":256}}' http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo
    echo Listing Slices: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices ; echo ; echo

    sleep $SLEEPINT

    echo "Binding Slice to NodeB (name=fast):" ; echo
    curl -i -X POST http://${NEXRAN_XAPP}:8000/v1/nodebs/enB_macro_001_001_0019b0/slices/fast ; echo ; echo
    echo "Getting NodeB (name=enB_macro_001_001_0019b0):" ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/nodebs/enB_macro_001_001_0019b0 ; echo ; echo

    sleep $SLEEPINT

    echo "Binding Slice to NodeB (name=slow):" ; echo
    curl -i -X POST http://${NEXRAN_XAPP}:8000/v1/nodebs/enB_macro_001_001_0019b0/slices/slow ; echo ; echo
    echo "Getting NodeB (name=enB_macro_001_001_0019b0):" ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/nodebs/enB_macro_001_001_0019b0 ; echo ; echo

    sleep $SLEEPINT

    echo "Creating Ue (ue=001010123456789)" ; echo
    curl -i -X POST -H "Content-type: application/json" -d '{"imsi":"001010123456789"}' http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo
    echo Listing Ues: ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo

    sleep $SLEEPINT

    echo "Binding Ue to Slice fast (imsi=001010123456789):" ; echo
    curl -i -X POST http://${NEXRAN_XAPP}:8000/v1/slices/fast/ues/001010123456789 ; echo ; echo
    echo "Getting Slice (name=fast):" ; echo
    curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices/fast ; echo ; echo

    #echo "Creating Ue (ue=001010123456788)" ; echo
    #curl -i -X POST -H "Content-type: application/json" -d '{"imsi":"001010123456788"}' http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo
    #echo Listing Ues: ; echo
    #curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/ues ; echo ; echo

    #echo "Binding Ue (imsi=001010123456788):" ; echo
    #curl -i -X POST http://${NEXRAN_XAPP}:8000/v1/slices/slow/ues/001010123456788 ; echo ; echo
    #echo "Getting Slice (name=slow):" ; echo
    #curl -i -X GET http://${NEXRAN_XAPP}:8000/v1/slices/slow ; echo ; echo

    #sleep $SLEEPINT

    #echo "Inverting priority of fast and slow slices:" ; echo

    #curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":1024}}' http://${NEXRAN_XAPP}:8000/v1/slices/slow ; echo ; echo ;

    #sleep $SLEEPINT

    #curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":256}}' http://${NEXRAN_XAPP}:8000/v1/slices/fast ; echo ; echo



    curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":1024}}' http://${NEXRAN_XAPP}:8000/v1/slices/slow ; echo ; echo ;
    curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":256}}' http://${NEXRAN_XAPP}:8000/v1/slices/fast ; echo ; echo

    curl -i -X PUT -H "Content-type: application/json" -d '{"allocation_policy":{"type":"proportional","share":1024}}' http://${NEXRAN_XAPP}:8000/v1/slices/fast ; echo ; echo 

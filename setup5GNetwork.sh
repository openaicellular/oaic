sudo ip netns add ue1

sudo srsepc & pid_epc=$! 

export E2NODE_IP=`hostname  -I | cut -f1 -d' '`
export E2NODE_PORT=5006
export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`

sudo srsenb --enb.n_prb=50 --enb.name=enb1 --enb.enb_id=0x19B \
--rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101,id=enb,base_srate=23.04e6" \
--ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=enbLog.txt --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT} & pid_enb=$!


echo "Waiting for RIC state to establish"
sleep 45

grep -Fq 'RIC state -> ESTABLISHED' enbLog.txt && exit

sudo srsue --gw.netns=ue1 & pid_ue=$!

sleep 15

timeout 5 sudo ip netns exec ue1 ping 172.16.0.1 -O 

rc=$? #124 for succesful ping

if [ $rc -ne 124 ] ; then exit -1 ; fi

#sudo killall -s2 srsue srsepc srsenb
#wait

echo "5G Network setup completed"

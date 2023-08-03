Instructions for deploying the Kpimon XApp
==========================================

1. Edit Configurations
----------------------

We need to change some settings in the srsRan config files to use ZMQ for the multiple UEs. Access the command line as superuser:

.. code-block:: bash

    $ sudo -i

Open ``rr.conf`` configuration file with the following command:

.. code-block:: bash

    $ vim .config/srsran/rr.conf

Make sure the following lines of text are commented with ``//``:

.. code-block:: none

    nr_cell_list =
    (
        //   {
        //    rf_port = 1;
        //    cell_id = 0x02;
        //    tac = 0x0007;
        //    pci = 500;
        //    root_seq_idx = 204;

        // TDD:
        //dl_arfcn = 634240;
        //band = 78;

        // FDD:
        //    dl_arfcn = 368500;
        //    band = 3;
        //  }
    );

Save and close the file. Then open ``ue.conf`` configuration file with the following command:

.. code-block:: bash

    $ vim .config/srsran/ue.conf

Comment out the ``[rat.eutra]`` section:

.. code-block:: none

    #[rat.eutra]
    #dl_earfcn = 3350

Comment out Lte release under the ``[rrc]`` section:

.. code-block:: none

    #release = 15

Make sure ``[rat.nr]`` section is commented out as well:

.. code-block:: none

    #[rat.nr]
    #bands = 3,78
    #nof_carriers = 1

Save and close the file. Then open ``enb.conf`` configuration file with the following command:

.. code-block:: bash

    $ vim .config/srsran/enb.conf

Make sure ``device_name`` and ``device_args`` are commented out:

.. code-block:: none

    #device_name = zmq
    #device_args = fail_on_disconnect=true,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101,id=enb,base_srate=23.04e6

Finally, we exit the root:

.. code-block:: bash

    $ exit

2. Start a multi-UE network
---------------------------

Here, we use two UEs as an example.

Terminal 1: Add two UEs:

.. code-block:: bash

    $ sudo ip netns add ue1
    $ sudo ip netns add ue2

Check the results:

.. code-block:: bash

    $ sudo ip netns list

If two UEs are listed, start the Core Network:

.. code-block:: bash

    $ sudo srsepc

Terminal 2: Set up Environment Variables and Base Station:

.. code-block:: bash

    $ export E2NODE_IP=`hostname -I | cut -f1 -d' '`
    $ export E2NODE_PORT=5006
    $ export E2TERM_IP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-e2term-sctp-alpha -o jsonpath='{.items[0].spec.clusterIP}'`

    $ sudo srsenb --enb.n_prb=100 --enb.name=enb1 --enb.enb_id=0x19B --rf.device_name=zmq --rf.device_args="fail_on_disconnect=true,tx_port=tcp://*:2000,rx_port=tcp://localhost:2009,id=enb,base_srate=23.04e6" --ric.agent.remote_ipv4_addr=${E2TERM_IP} --log.all_level=warn --ric.agent.log_level=debug --log.filename=stdout --ric.agent.local_ipv4_addr=${E2NODE_IP} --ric.agent.local_port=${E2NODE_PORT}

Terminal 3: Set up the first UE:

.. code-block:: bash

    $ sudo srsue \
      --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2010,rx_port=tcp://localhost:2008,id=ue,base_srate=23.04e6" \
    --usim.algo=xor --usim.imsi=001010123456789 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 \
    --log.all_level=warn --log.filename=stdout --gw.netns=ue1

Terminal 4: Set up the second UE:

.. code-block:: bash

    $ sudo srsue \
      --rf.device_name=zmq --rf.device_args="tx_port=tcp://*:2007,rx_port=tcp://localhost:2006,id=ue,base_srate=23.04e6" \
    --usim.algo=xor --usim.imsi=001010123456780 --usim.k=00112233445566778899aabbccddeeff --usim.imei=353490069873310 \
    --log.all_level=warn --log.filename=stdout --gw.netns=ue2

Terminal 5: Attach the two UEs to eNB:

.. code-block:: bash

    $ python3 multi_ues.py

Generate traffic by ping test on server side
In Terminal 6 and 7:

.. code-block:: bash

    $ sudo ip netns exec ue1 ping 172.16.0.1
    $ sudo ip netns exec ue2 ping 172.16.0.1

Alternatively, generate traffics by iperf3:
In Terminal 6 and 7:

.. code-block:: bash

    $ iperf3 -s -i 1 -p 5201
    $ iperf3 -s -i 1 -p 5301

In Terminal 8 and 9:

.. code-block:: bash

    $ sudo ip netns exec ue1 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 600 -p 5201
    $ sudo ip netns exec ue2 iperf3 -c 172.16.0.1 -b 10M -i 1 -t 600 -p 5301

3. Upgraded KPIMON deployment
-----------------------------

The xApp descriptor files (config.json) must be hosted on a web server when we use the xapp-onboarder to deploy xApps. To host these files we use Nginx to create a web server.

In a New Terminal, go through the following commands one by one:

.. code-block:: bash

    $ sudo apt-get install nginx
    $ sudo systemctl status nginx
    $ cd /etc/nginx/sites-enabled
    $ sudo unlink default
    $ cd ../
    $ cd ../../var/www
    $ sudo mkdir xApp_config.local
    $ cd xApp_config.local/
    $ sudo mkdir config_files
    $ cd ../../../etc/nginx/conf.d
    $ sudo vim xApp_config.local.conf

Paste the following content in the conf file.

.. code-block:: none

    server {
        listen 5010 default_server;
        server_name xApp_config.local;
        location /config_files/ {
            root /var/www/xApp_config.local/;
        }
    }

Save the configuration file and check if there are any errors in the configuration file.

.. code-block:: bash

    sudo nginx -t

We can now clone the repositories. Under the OAIC directory, run the following commands:

.. code-block:: bash

    $ git clone https://github.com/openaicellular/upgraded-kpimon-xApp.git

Create a symlink from the xapp’s config file (in this case kpimon). This can be replaced by another symlink in the future. Reload Nginx once this has been done.

.. code-block:: bash

    $ sudo cp /scp-kpimon-config-file.json /var/www/xApp_config.local/config_files/
    $ sudo systemctl reload nginx

Check if the config file can be accessed from the newly created server.

.. code-block:: bash

    $ export MACHINE_IP=`hostname -I | cut -f1 -d' '`
    $ curl http://${MACHINE_IP}:5010/config_files/scp-kpimon-config-file.json

Now, we create a docker image of the KPIMON xApp using the given docker file.

.. code-block:: bash

    $ cd upgraded-kpimon-xApp
    $ sudo docker build . -t xApp-registry.local:5008/scp-kpimon:1.0.1

Get some variables of RIC Platform ready. The following Three variables represent the IP addresses of the services running on the RIC Platform.

.. code-block:: bash

    $ export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
    $ export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
    $ export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`

Check for helm charts:

.. code-block:: bash

    $ curl --location --request GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"

Next, we need to create a .url file to point the xApp-onboarder to the Nginx server to get the xApp descriptor file and use it to create a helm chart and deploy the xApp.

Get IP address by one of the two commands below:

.. code-block:: bash

    $ hostname -I
    or
    $ echo $MACHINE_IP$

Create url file:

.. code-block:: bash

    $ vim scp-kpimon-onboard.url

Paste the following in the `scp-kpimon-onboard.url` file. Substitute the `172.17.0.1` with the IP address of your machine.

.. code-block:: none

    {"config-file.json_url":"http://<machine_ip_address>:5010/config_files/scp-kpimon-config-file.json"}

Save the file.

Deploying the xApp:

Check for helm charts and determine if the current xApp is present. If no helm chart exists, proceed. If a chart exists, either use it or delete it before continuing.

.. code-block:: bash

    $ curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@scp-kpimon-onboard.url"
    $ curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"
    $ curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "scp-kpimon"}'

Verify xApp Deployment:

There should be a ricxapp-scp-kpimon pod in ricxapp namespace:

.. code-block:: bash

    $ sudo kubectl get pods -A

In the terminal, print the KPIMON logs:

.. code-block:: bash

    $ sudo kubectl logs -f -n ricxapp -l app=ricxapp-scp-kpimon

If you already have KPIMON xApp deployed on your system, you need to restart the pod using the command below before running the rest of the commands.

In a New Terminal, print the KPIMON Metrics:

.. code-block:: bash

    $ sudo kubectl exec -it -n ricxapp `sudo kubectl get pod -n ricxapp -l app=ricxapp-scp-kpimon -o jsonpath='{.items[0].metadata.name}'` -- tail -F /opt/kpimon.log

If the above deployment has an issue, you can restart the KPIMON xApp:

.. code-block:: bash

    $ sudo kubectl -n ricxapp rollout restart deployment ricxapp-scp-kpimon


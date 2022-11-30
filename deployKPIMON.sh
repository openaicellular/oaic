sudo apt-get install nginx
sudo systemctl status nginx

cd /etc/nginx/sites-enabled
sudo unlink default
cd ../

cd ../../var/www
sudo mkdir xApp_config.local
cd xApp_config.local/
sudo mkdir config_files

cd ../../../etc/nginx/conf.d

echo "server {
    listen 5010 default_server;
    server_name xApp_config.local;
    location /config_files/ {

        root /var/www/xApp_config.local/;
    }

}" >xApp_config.local.conf

sudo nginx -t

sudo ln -s ric-scp-kpimon/scp-kpimon-config-file.json /var/www/xApp_config.local/config_files/scp-kpimon-config-file.json
sudo systemctl reload nginx

export MACHINE_IP=`hostname  -I | cut -f1 -d' '`
curl http://${MACHINE_IP}:5010/config_files/scp-kpimon-config-file.json

cd ~/oaic/ric-scp-kpimon
sudo docker build . -t xApp-registry.local:5008/scp-kpimon:1.0.1

export KONG_PROXY=`sudo kubectl get svc -n ricplt -l app.kubernetes.io/name=kong -o jsonpath='{.items[0].spec.clusterIP}'`
export APPMGR_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-appmgr-http -o jsonpath='{.items[0].spec.clusterIP}'`
export ONBOARDER_HTTP=`sudo kubectl get svc -n ricplt --field-selector metadata.name=service-ricplt-xapp-onboarder-http -o jsonpath='{.items[0].spec.clusterIP}'`

curl --location --request GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"

echo '{"config-file.json_url":"http://'$MACHINE_IP':5010/config_files/scp-kpimon-config-file.json"}' > scp-kpimon-onboard.url

curl -L -X POST "http://$KONG_PROXY:32080/onboard/api/v1/onboard/download" --header 'Content-Type: application/json' --data-binary "@scp-kpimon-onboard.url"
curl -L -X GET "http://$KONG_PROXY:32080/onboard/api/v1/charts"
curl -L -X POST "http://$KONG_PROXY:32080/appmgr/ric/v1/xapps" --header 'Content-Type: application/json' --data-raw '{"xappName": "scp-kpimon"}'


sleep 10

sudo kubectl get pods -A | grep 'kpimon' | grep 'Running'

sudo timeout 5 sudo kubectl logs -f -n ricxapp -l app=ricxapp-scp-kpimon

rc=$? #124 for succesful ping

if [ $rc -ne 124 ] ; then exit -1 ; fi

echo 'Successful: KPIMON xApp up and running'

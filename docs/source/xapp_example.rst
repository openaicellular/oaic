
=================================
Getting started: Developing xapps
=================================

This document describes how to write an xApp and how to deploy it on the RIC platform.

For a full reference, it is recommended to use the xApps Writers Guide:
https://wiki.o-ran-sc.org/download/attachments/17269011/xApp_Writer_s_Guide_v2.pdf?version=4&modificationDate=1625642899082&api=v2


Overview
--------

The RIC platform currently provides frameworks that make it easier to construct xApps in several languages. We will use the go xapp framework, but other languages do not differ much.


xApp framework is a simple skeleton designed for rapid development of RIC xapps based on Go. Following figure depicts the high level architecture of xApp-framework, which consists of several loosely linked components that provide the common functions needed by the xApp developers. For GO-based xApps, application developers do not need to write xApp code from the scratch, but can use xApp-framework, which is designed to facilitate and rapidly build a full-fledged RIC xApps. The xApp-framework can be found [xapp-frame] and a simple example-Xapp that illustrates how the framework can be used is located at [example] The dependencies and prerequisites of xApp development are also described in the above links.



The Xapp Frame common functions and interfaces
----------------------------------------------


• RESTful (new resources can be injected dynamically)
• RMR (with message filtering based on application rules)
• Database backend services (with configurable namespaces, etc)
• Watching and populating config-map changes in K8S environment
• Logging (via MDCLOG + tracing support in the future)
• ASN.1 decoding and encoding.


The GO xApp Framework supports various essential components
-----------------------------------------------------------

• RESTful: HTTP services, health probes and injecting new resources can be dynamically
• RMR client: Sending and receiving of RMR messages with message filtering based on application rules). 
• DB client: Database backend services (with configurable namespaces etc) for SDL, RNIB and UENIB. 
• Config: Reading, watching and populating config-map changes in K8S environment on runtime
• Logging and tracing: Generic system logging and tracing services (in the future). 
• Metrics and statistics: Generating and publishing metrics to the met- rics server (Prometheus interface used to collect (or “scrap”) metrics and counters. 
• ASN.1 decoding and encoding.


For more information on the go-xapp-frame: https://gerrit.o-ran-sc.org/r/admin/repos/ric-plt/xapp-frame


Hello world xAPP code and deployment
====================================


For demonstration purposes the hello world xapp written in go will be used.

To get started clone the code:

.. code-block:: rst
    
    git clone https://github.com/o-ran-sc/ric-app-hw-go.git



This xAPP aims to provide basic implementation of :

1. A1 interfaces interactions

2. Read-write operations into a persistent storage. 

3. xAPP Configuration management

4. RMR Health Check

5. xAPP SDL Check

6. Raising alarms

7. Generating metrics

8. E2 Interface intereactions


The methods in the source code show these functionalities.

hwApp.go source code:

.. code-block:: rst

    package main

    import (
	    "gerrit.o-ran-sc.org/r/ric-plt/xapp-frame/pkg/xapp"
    )

    type HWApp struct {
    }

    var (
	    A1_POLICY_QUERY      = 20013
	    POLICY_QUERY_PAYLOAD = "{\"policy_type_id\":20000}"
    )

    func (e *HWApp) sendPolicyQuery() {
	    xapp.Logger.Info("Invoked method to send  policy query message")

	    // prepare and send policy query message over RMR
	    rmrParams := new(xapp.RMRParams)
	    rmrParams.Mtype = A1_POLICY_QUERY // A1_POLICY_QUERY
	    rmrParams.Payload = []byte(POLICY_QUERY_PAYLOAD)

	    // send rmr message
	    flg := xapp.Rmr.SendMsg(rmrParams)

	    if flg {
		    xapp.Logger.Info("Successfully sent policy query message over RMR")
	    } else {
		    xapp.Logger.Info("Failed to send policy query message over RMR")
	    }
    }

    func (e *HWApp) ConfigChangeHandler(f string) {
	    xapp.Logger.Info("Config file changed")
    }

    func (e *HWApp) xAppStartCB(d interface{}) {
	    xapp.Logger.Info("xApp ready call back received")
    }

    func (e *HWApp) Consume(msg *xapp.RMRParams) (err error) {
	    id := xapp.Rmr.GetRicMessageName(msg.Mtype)

	    xapp.Logger.Info("Message received: name=%s meid=%s subId=%d txid=%s len=%d", id, msg.Meid.RanName, msg.SubId, msg.Xid, msg.PayloadLen)

	    switch id {
	    // policy request handler
	    case "A1_POLICY_REQUEST":
		    xapp.Logger.Info("Recived policy instance list")

	    // health check request
	    case "RIC_HEALTH_CHECK_REQ":
		    xapp.Logger.Info("Received health check request")

	    default:
		    xapp.Logger.Info("Unknown message type '%d', discarding", msg.Mtype)
	    }

	    defer func() {
		    xapp.Rmr.Free(msg.Mbuf)
		    msg.Mbuf = nil
	    }()
	    return
    }

    func (e *HWApp) Run() {

	    // set MDC
	    xapp.Logger.SetMdc("HWApp", "0.0.1")

	    // set config change listener
	    xapp.AddConfigChangeListener(e.ConfigChangeHandler)

	    // register callback after xapp ready
	    xapp.SetReadyCB(e.xAppStartCB, true)

	    // reading configuration from config file
	    waitForSdl := xapp.Config.GetBool("db.waitForSdl")

	    // start xapp
	    xapp.RunWithParams(e, waitForSdl)

    }

    func main() {
	    hw := HWApp{}
	    hw.Run()
    }



*The repository provides several optiuons for running the xAPP. The instructions are mirrored below from the github repository*



This xAPP can be run directly as a Linux binary, as a docker image, or in a pod in a Kubernetes environment. The first two can be used for dev testing. The last option is how an xAPP is deployed in the RAN Intelligent Controller environment. This covers all three methods. 

1. Docker 

2. Linux Binary

3. Kubernetes 



Software Installation and Deployment
====================================


Onboarding of hw-go using dms_cli tool
--------------------------------------


dms_cli offers rich set of command line utility to onboard hw-go xapp
to chartmuseme.

First checkout the [hw-go](https://gerrit.o-ran-sc.org/r/admin/repos/ric-app/hw-go) repository from gerrit.

.. code-block:: rst

    git clone "https://gerrit.o-ran-sc.org/r/ric-app/hw-go"


hw-go has following folder structure

.. code-block:: rst

    ├── Dockerfile
    ├── INFO.yaml
    ├── LICENSES.txt
    ├── README.md
    ├── config
    │   ├── config-file.json           // descriptor for hw-go
    │   ├── schema.json                // schema for controls section of descriptor
    │   └── uta_rtg.rt                 // local route file
    ├── docs
    ├── go.mod
    ├── go.sum
    └── hwApp.go



For onboarding hw-go make sure that dms_cli and helm3 is installed. 
One can follow [documentation](https://docs.o-ran-sc.org/projects/o-ran-sc-it-dep/en/latest/installation-guides.html#ric-applications) to configure dms-cli.

Once dms_cli is availabe we can proceed to onboarding proceure.

configure the export CHART_REPO_URL to point chartmuseme.

.. code-block:: rst
    
    $ export CHART_REPO_URL=http://<service-ricplt-xapp-onboarder-http.ricplt>:8080


check if dms_cli working fine.

.. code-block:: rst

    dms_cli health
    True


Now move to config folder to initiate onboarding.

.. code-block:: rst
   
    $ cd config
    $ dms_cli onboard --config_file_path=config-file.json --shcema_file_path=schema.json
    httpGet:
    path: '{{ index .Values "readinessProbe" "httpGet" "path" | toJson }}'
    port: '{{ index .Values "readinessProbe" "httpGet" "port" | toJson }}'
    initialDelaySeconds: '{{ index .Values "readinessProbe" "initialDelaySeconds" | toJson }}'
    periodSeconds: '{{ index .Values "readinessProbe" "periodSeconds" | toJson }}'

    httpGet:
    path: '{{ index .Values "livenessProbe" "httpGet" "path" | toJson }}'
    port: '{{ index .Values "livenessProbe" "httpGet" "port" | toJson }}'
    initialDelaySeconds: '{{ index .Values "livenessProbe" "initialDelaySeconds" | toJson }}'
    periodSeconds: '{{ index .Values "livenessProbe" "periodSeconds" | toJson }}'

    {
     "status": "Created"
    }


Check if hw-go is onborded

.. code-block:: rst
   
    $ curl --location --request GET "http://<appmgr>:32080/onboard/api/v1/charts"  --header 'Content-Type: application/json'

    {
     "hw-go": [
         {
             "name": "hw-go",
             "version": "1.0.0",
             "description": "Standard xApp Helm Chart",
             "apiVersion": "v1",
             "appVersion": "1.0",
             "urls": [
                 "charts/hw-go-1.0.0.tgz"
             ],
             "created": "2021-06-24T18:57:41.98056196Z",
             "digest": "14a484d9a394ed34eab66e5241ec33e73be8fa70a2107579d19d037f2adf57a0"
         }
     ]
    }



If we would wish to download the charts then we can perform following curl operation :

.. code-block:: rst
    
    curl --location --request GET "http://<appmgr>:32080/onboard/api/v1/charts/xapp/hw-go/ver/1.0.0"  --header 'Content-Type: application/json' --output hw-go.tgz


Now the onboarding is done.



Deployment of hw-go
~~~~~~~~~~~~~~~~~~~

Once charts are available we can deploy the the hw-go using following curl command :

.. code-block:: rst
    
    $ curl --location --request POST "http://<appmgr>:32080/appmgr/ric/v1/xapps"  --header 'Content-Type: application/json'  --data-raw '{"xappName": "hw-go", "helmVersion": "1.0.0"}'
    {"instances":null,"name":"hw-go","status":"deployed","version":"1.0"}


Deployment will be done in ricxapp ns :

.. code-block:: rst

    kubectl get po -n ricxapp
    NAME                             READY   STATUS    RESTARTS   AGE
    ricxapp-hw-go-55ff7549df-kpj6k   1/1     Running   0          2m

.. code-block:: rst

    kubectl get svc -n ricxapp
    NAME                         TYPE        CLUSTER-IP      EXTERNAL-IP   PORT(S)             AGE
    aux-entry                    ClusterIP   IP1             <none>        80/TCP,443/TCP      73d
    service-ricxapp-hw-go-http   ClusterIP   IP2             <none>        8080/TCP            103m
    service-ricxapp-hw-go-rmr    ClusterIP   IP3             <none>        4560/TCP,4561/TCP   103m


Now we can query to appmgr to get list of all the deployed xapps :

.. code-block:: rst

    curl http://service-ricplt-appmgr-http.ricplt:8080/ric/v1/xapps | jq .
    % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
    100   347  100   347    0     0    578      0 --:--:-- --:--:-- --:--:--   579
    [   
    {
     "instances": [
       {
         "ip": "service-ricxapp-hw-go-rmr.ricxapp",
         "name": "hw-go-55ff7549df-kpj6k",
            "policies": [
              1
            ],
            "port": 4560,
            "rxMessages": [
            "RIC_SUB_RESP",
            "A1_POLICY_REQ",
            "RIC_HEALTH_CHECK_REQ"
            ],
            "status": "running",
            "txMessages": [
            "RIC_SUB_REQ",
            "A1_POLICY_RESP",
            "A1_POLICY_QUERY",
            "RIC_HEALTH_CHECK_RESP"
         ]
        }
        ]   ,
        "name": "hw-go",
        "status": "deployed",
        "version": "1.0"
    }
    ]



To view logs from hw-go:

.. code-block:: rst
    
    kubectl logs ricxapp-hw-go-55ff7549df-kpj6k -n ricxapp




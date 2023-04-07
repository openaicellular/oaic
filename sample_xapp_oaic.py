import json
import logging
import signal
import time
import asn1tools
from ricxappframe.xapp_frame import RICMessage, RICRegion, RICService, RICSubscription
from ricxappframe.xapp_frame import RICXapp, RICXappException
from ricxappframe.e2ap.asn1 import IndicationMessage, SubResponseMsg, SubRequestMsg, ControlRequestMsg

e2sm = asn1tools.compile_files('e2sm/e2sm-json-v01.00.asn1', 'uper')
 
# Set up logging
logging.basicConfig(level=logging.DEBUG)
 
# Define the signal handler function
def signal_handler(sig, frame):
    logging.info("Exiting...")
    exit(0)
 
# Define the E2 message handler function
def e2_message_handler(e2event):
    logging.info("Received E2 message: %s", e2event.data)


# ----- code for E2 service model handling -----
# Control messages and indication messages are specific for the E2SM-JSON ASN.1 model

def send_control_msg(self, header_dict, body_dict):
    header = e2sm.encode('E2SM-JSON-ControlHeader', header_dict)
    body = e2sm.encode('E2SM-JSON-ControlMessage', body_dict)

    creq = ControlRequestMsg()
    msg_len, msg = creq.encode(
        requestor_id=???,
        request_sequence_number=???,
        ran_function_id=???,
        call_process_id=None,
        control_header=header,
        control_message=body,
        control_ack_request=None
    )
    # NOTE: find the specification for requestor ID, request sequence number, ran function ID

    self.rmr_send(msg, JSON_CONTROL)

# we are using indication messages like in KPIMON, except that the payloads are just strings
def handle_indication(self, summary, sbuf):
    global current_iq_data

    msg = IndicationMessage.decode(summary['payload'])

    header = e2sm.decode('E2SM-JSON-IndicationHeader', msg.indication_header)
    body = e2sm.decode('E2SM-JSON-IndicationMessage', msg.indication_message)

    j = json.loads(body.stringPayload)  # j is a dictionary

    # do something with dictionary here
    print(j)
   
    self.rmr_free(sbuf)

def handle_control_failure(self, summary, sbuf):
    e2ap.getRICControlFailure(summary['payload'])
    logging.error("Control request was rejected by nodeB")

    self.rmr_free(sbuf)

# we need to see the results of sending a control message to nodeB
def handle_control_outcome(self, summary, sbuf):
    ack = e2ap.getRICControlAcknowledge(summary['payload'])
    # not sure if outcome is different from ACK
    e2sm.getRICControlOutcome(ack)

    self.rmr_free(sbuf)

def handle_subscription_response(self, summary, sbuf):
    logging.info(f"Subscription response received from nodeB")
    logging.debug(f"The SubId in RIC_SUB_RESP is {summary['subscription id']}")
    subscriptionResp = SubResponseMsg()
    subscriptionResp.decode(summary['payload'])
    logging.info(f"RIC Subscription Response message from {summary['meid']} received")
    logging.info(f"SubscriptionID: {summary['subscription id']}")
    logging.info(f"RequestID: {subscriptionResp.request_id}")
    logging.info(f"RequestSequenceNumber: {subscriptionResp.request_sequence_number}")
    logging.info(f"FunctionID: {subscriptionResp.function_id}")

    # implemented in asn1.py
    logging.info("ActionAdmittedList:")
    for i in range(subscriptionResp.action_admitted_list.count):
        logging.info(f"[{i}]ActionID: {subscriptionResp.action_admitted_list.request_id[i]}")

    # implemented in asn1.py
    logging.info("ActionNotAdmittedList:")
    for i in range(subscriptionResp.action_not_admitted_list.count):
        logging.info(f"[{i}]ActionID: {subscriptionResp.action_not_admitted_list.request_id[i]}")
        logging.info(f"[{i}]CauseType: {subscriptionResp.action_not_admitted_list.causeList[i].cause_type}    CauseID: {subscriptionResp.action_not_admitted_list.causeList[i].cause_id}")

    self.rmr_free(sbuf)

def handle_subscription_failure(self, summary, sbuf):
    logging.error("Subscription failure received from nodeB")

    self.rmr_free(sbuf)

def handle_subscription_delete_response(self, summary, sbuf):
    logging.info("Subscription deleted from nodeB")

    self.rmr_free(sbuf)

def handle_subscription_delete_failure(self, summary, sbuf):
    logging.info("nodeB failed to delete subscription")

    self.rmr_free(sbuf)

# ----- end code for E2 service model handling -----



# ----- code for RMR messages -----
# This code doesn't consider the E2 callbacks in RICXapp so they may need to be adjusted

# Default handler for RMR messages
def default_handler(self, summary, sbuf):
    """
    Function that processes messages for which no handler is defined
	    """
    self.def_hand_called += 1
    logging.warning("default_handler unexpected message type {}".format(summary[rmr.RMR_MS_MSG_TYPE]))
    self.rmr_free(sbuf)

# This dictionary allows us to look up functions based on the message type ID
# This makes it so we don't have to create an if/elif statement for every message type
handler_function = {
    control_outcome: handle_control_outcome,
    control_failure: handle_control_failure,
    subscription_response: handle_subscription_response,
    subscription_failure: handle_subscription_failure,
    subscription_delete_response: handle_subscription_delete_response,
    subscription_delete_failure: handle_subscription_delete_failure
}

def receive_msg(self):
    for summary, sbuf in self.rmr_get_messages():
        if summary['mtype'] in handler_function:
            handler_function[summary['mtype']](self, summary, sbuf)
        else:
            default_handler(self, summary, sbuf)
# ----- end code for RMR messages -----
            
 
# Define the xApp class
class HelloWorldXapp(RICXapp):
    def __init__(self):
        super().__init__("HelloWorldXapp")
        self.subscription_id = None
        self.region_id = None
        self.service_id = None
        self.subscribed = False
 
    # Override the RICXapp.run() method
    def run(self):
        # Register the signal handler
        signal.signal(signal.SIGINT, signal_handler)
 
        # Initialize the E2 interface
        self.e2_init()
        self.e2_register_callback(e2_message_handler)
 
        # Register the RIC subscription
        self.subscription_id = self.register_subscription(RICSubscription.RIC_SUB_REQ_SUBDEL, None, self.subscription_callback)
        logging.info("Registered subscription with subscription ID: %s", self.subscription_id)
 
        # Register the RIC region
        self.region_id = self.register_ric_region(RICRegion.RIC_REGION_NOTIFY, None, None)
        logging.info("Registered RIC region with region ID: %s", self.region_id)
 
        # Register the RIC service
        self.service_id = self.register_ric_service(RICService.RIC_SERVICE_UPDATE, self.service_callback)
        logging.info("Registered RIC service with service ID: %s", self.service_id)
 
        # Wait for events
        while True:
            time.sleep(1)
 
    # Define the RIC subscription callback function
    def subscription_callback(self, sub_id, notification):
        logging.info("Received RIC subscription notification: %s", notification)
        self.subscribed = True
 
    # Define the RIC service callback function
    def service_callback(self, service_data):
        logging.info("Received RIC service update: %s", service_data)
 
        # Send an E2 message
        e2_message = {"message": "Hello, E2!"}
        self.e2_send_message(json.dumps(e2_message).encode(), 1234)
 
if __name__ == "__main__":
    try:
        # Create and run the xApp
        xapp = HelloWorldXapp()
        xapp.run()
    except RICXappException as e:
        logging.error(str(e))
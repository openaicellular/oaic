

import os
import sys
#sys.path.append('C:/Users/Mohammadreza/Desktop/My Class/Proj-DC/My Works/Scheduling/xApp/mr7-main/mr9_github')
sys.path.append('.')
import schedule
import datetime
import apscheduler
from time import sleep
from apscheduler.schedulers.background import BackgroundScheduler
from zipfile import ZipFile
import json
from os import getenv
from ricxappframe.xapp_frame import RMRXapp, rmr, Xapp
#from mr import sdl

import logging
import numpy as np
import tensorflow as tf
from numpy import zeros, newaxis

from mr import populate
from populate import INSERTDATA
from mr.db import DATABASE
#import mr.populate as populate

from tensorflow import keras
from tensorflow.keras import layers
import gym

import numpy as np
import pandas as pd
import statistics
from statistics import mean
import matplotlib.pyplot as plt
import IPython
from IPython import display

from tensorflow.keras.models import Sequential 
from tensorflow.keras.layers import LSTM, Dense  
from tensorflow.keras.layers import Activation  
from tensorflow.keras.optimizers import Adam
from _thread import *
import socket
import threading
import errno


xapp = None
pos = 0
RAN_data = None
rmr_xapp = None

HEADER = 64
SC_PORT = 8585
SERVER = socket.gethostbyname(socket.gethostname())
print('SERVER=',SERVER)
print('socket.gethostname()=',socket.gethostname())
ADDR = (SERVER, SC_PORT)
print('ADDR=', ADDR)
FORMAT = 'UTF-8'
DISCONNECT_MESSAGE = "!DISCONNECT"

# server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# server.bind(ADDR)

reg_actor_list = []

scheduler = BackgroundScheduler()

class UENotFound(BaseException):
    pass
class CellNotFound(BaseException):
    pass

def post_init(self):
    print('///////enter def post_init__/////////////////')
    """
    Function that runs when xapp initialization is complete
    """
    self.def_hand_called = 0
    self.traffic_steering_requests = 0


def handle_config_change(self, config):
    print('////////enter def handle_config_change//////////////')
    """
    Function that runs at start and on every configuration file change.
    """
    self.logger.debug("handle_config_change: config: {}".format(config))


def default_handler(self, summary, sbuf):
    print('/////////enter def default_handler///////////////')
    """
    Function that processes messages for which no handler is defined
    """
    self.def_hand_called += 1
    print('self.def_hand_called += 1=', self.def_hand_called)
    self.logger.warning("default_handler unexpected message type {}".format(summary[rmr.RMR_MS_MSG_TYPE]))
    self.rmr_free(sbuf)


# def entry():
#     print('////////////enter def entry///////////////')
#     """  Read from DB in an infinite loop and run prediction every second
#       TODO: do training as needed in the future
#     """
#     job = schedule.every(1).seconds.do(start_server_listening, connectdb)
#     print('/////////pass 1 entry schedule.every(1).seconds.do(connectdb)/////')
#     connectdb()
#     #while True:
#         #print('////while True in entry/////') 
#         #schedule.run_pending()  

def entry():
    print('////////////enter def entry///////////////')   
    #scheduler.add_job(id='Scheduled1 task', func=start_server_listening, trigger='interval', seconds=1)
    scheduler.add_job(id='Scheduled2 task', func=connectdb, trigger='interval', seconds=1)
    scheduler.start()
    start_new_thread(waiting_actor_registration, ())
    #new_thread = threading.Thread(target=waiting_actor_registration, args=())
    #new_thread.start()
    #print('/////////pass both entry schedule.every(1).seconds.do(connectdb)/////')
    while True:
        sleep(1)
        

def connectdb():
    print('////////////////////enter def connectdb///////////////////')
    # Create a connection to InfluxDB if thread=True, otherwise it will create a dummy data instance
    #global db
    global RAN_data
    
    db2 = DATABASE('kpimon')
    print('db2 =  DATABASE(kpimon)=', db2) 
    #db2.read_data("ricIndication_UeMetrics")
    db2.read_data("ricIndication_UeMetrics")
    print('////came back from db2.DATABASE2-read-data to connectdb.else///////')
    #print('db2.read_data("ueMetrics")=', db2.read_data("ueMetrics"))
    ue_data_kpimon = db2.data.values.tolist()  # needs to be updated in future when live feed will be coming through KPIMON to influxDB
    print('ue_data_kpimon = db2.data.values.tolist()=', ue_data_kpimon)
    
    db3 = DATABASE('kpimon')
    print('db3 =  DATABASE(kpimon)=', db3) 
    db3.read_data("ricIndication_cellMetrics")
    print('////came back from db3.DATABASE2-read-data to connectdb.else///////')
    #print('db3.read_data("cellMetrics")=', db3.read_data("cellMetrics"))
    cell_data_kpimon = db3.data.values.tolist()  # needs to be updated in future when live feed will be coming through KPIMON to influxDB
    print('cell_data_kpimon = db3.data.values.tolist()=', cell_data_kpimon)
    
    message_sent = {"type": "KPI xApp", "UE Metrics": ue_data_kpimon, "Cell Metrics": cell_data_kpimon}
    
    data = json.dumps(message_sent)
    
    for actor_socket in reg_actor_list:
        print("sending msg to the actor ....")
        try:
            actor_socket.sendall(bytes(data, encoding="utf-8"))
        except socket.error as e:
            if e.errno != errno.EPIPE:
                reg_actor_list.remove_actor(actor_socket)
                print("BrokenPIPI: Actor Disconnected!!")
            
        print("Done!")
        
    #for actor_socket in reg_actor_list:
        #print("sending msg to the actor ....")
        #actor_socket.sendall(bytes(data, encoding="utf-8"))
        #print("Done!")
        
        
    #print('msg=', msg)
    #df = pd.DataFrame(msg)
    #print('df=', df)
    #data = time(df)
    #print('df2 = time(df)=', data)
    
    #cell_ue_collection = ue_data_kpimon + cell_data_kpimon
    #print('cell_ue_collection=', cell_ue_collection)
    #cell_ue_collection = json.dumps(cell_ue_collection)
    
    ue_data = pd.DataFrame(ue_data_kpimon)
    cell_data = pd.DataFrame(cell_data_kpimon)
    print('ue_data=', ue_data)
    print('cell_data=', cell_data)
    ue_data = time(ue_data)
    cell_data = time(cell_data)
    print('df2 = time(ue_data)=', ue_data)
    print('df3 = time(cell_data)=', cell_data)
    

def handle_client(conn, addr):
    print(f"[NEW CONNECTION] {addr} connected.")
    connected = True
    while connected:
        msg_length = conn.recv(HEADER).decode(FORMAT)
        if msg_length:
            msg_length = int(msg_length)
            msg = conn.recv(msg_length).decode(FORMAT)
            print(f"[{addr}] {msg}")
            if msg == DISCONNECT_MESSAGE:
                connected = False
            print(f"[{addr}] {msg}")
        #conn.send("Msg received".encode(FORMAT))
        #conn.send(connectdb())

    conn.close()

# def start_server_listening():
#     server.listen()
#     print(f"[LISTENING] Server is listening on {SERVER}")
#     while True:
#         conn, addr = server.accept()
#         print('in start_srver_func conn=', conn)
#         print('in start_server_function addr=', addr)
#         thread = threading.Thread(target=handle_client, args=(conn, addr))
#         thread.start()
#         print(f"[ACTIVE CONNECTIONS] {threading.activeCount() - 1}")
        
# def send(msg):
#     message = msg.encode(FORMAT)
#     msg_length = len(message)
#     send_length = str(msg_length).encode(FORMAT)
#     send_length += b' ' * (HEADER - len(send_length))
#     client.send(send_length)
#     client.send(message)
#     print(client.recv(2048).decode(FORMAT))
    

def waiting_actor_registration():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(ADDR)
    s.listen(5)
    
    while True:
        actor_socket, addr = s.accept()
        reg_actor_list.append(actor_socket) 
        print('Connected to : ' + str(addr[0]) + ':' + str(addr[1]))
        start_new_thread(waiting_actor_thread, (actor_socket,))
        
    s.close()
    
def waiting_actor_thread(actor_socket):
    while True:
        data = actor_socket.recv(1024)
        data = data.decode("utf-8")
        print("<<-- Received message: {}".format(data) + " from the actor")
        #if not data:
        if len(data) == 0:
            print("Actor disconnected!")
            break

        message = json.loads(data, strict=False)
        #message_handler.handle(actor, message, self)

    # connection closed
    reg_actor_list.remove_actor(actor_socket)
    actor_socket.close()



def start(thread=False):
 
    print('////////////////entered Starrrrrrrrrrrt///////////////////')
    """
    This is a convenience function that allows this xapp to run in Docker
    for "real" (no thread, real SDL), but also easily modified for unit testing
    (e.g., use_fake_sdl). The defaults for this function are for the Dockerized xapp.
    """
    global xapp

    #fake_sdl = getenv("USE_FAKE_SDL", None)
    #xapp = Xapp(entrypoint=entry, rmr_port=4560, use_fake_sdl=False)
    #print('xapp = Xapp(entrypoint=entry, rmr_port=4560, use_fake_sdl=fake_sdl)=', xapp)
  
    use_fake_sdl=False
    rmr_port=4560
    
    #start_server_listening()
    #print("[STARTING] server is start listening...")
    
    entry()
    
    #xapp.run()


def stop():
    print('/////////////enter def stop//////////////////')      
    """
    can only be called if thread=True when started
    """
    xapp.stop()


def get_stats():
    print('//////////////////enter def get_stats()////////////////////')
    """
    hacky for now, will evolve
    """
    print('DefCalled:rmr_xapp.def_hand_called=', rmr_xapp.def_hand_called)
    print('SteeringRequests:rmr_xapp.traffic_steering_requests=', rmr_xapp.traffic_steering_requests) 
    return {"DefCalled": rmr_xapp.def_hand_called,
            "SteeringRequests": rmr_xapp.traffic_steering_requests}



def time(df):
    print('///////////////enter def time//////////////')
    df.index = pd.date_range(start=datetime.datetime.now(), freq='1s', periods=len(df))

    print(df)

    df[0] = df[0].apply(lambda x: str(x))

    return df


    



def mr_req_handler(self, summary, sbuf):
    print('///////////enter def mr_req handler/////////////')
    """
    This is the main handler for this xapp, which handles load prediction requests.
    This app fetches a set of data from SDL, and calls the predict method to perform
    prediction based on the data

    The incoming message that this function handles looks like:
        {"UEPredictionSet" : ["UEId1","UEId2","UEId3"]}
    """
    #self.traffic_steering_requests += 1
    # we don't use rts here; free the buffer
    self.rmr_free(sbuf)

    ue_list = []
    try:
        print('////enter first try in mr_req_handler////')
        print('rmr.RMR_MS_PAYLOAD=', rmr.RMR_MS_PAYLOAD)
        print('summary[rmr.RMR_MS_PAYLOAD]=', summary[rmr.RMR_MS_PAYLOAD])
        req = json.loads(summary[rmr.RMR_MS_PAYLOAD])  # input should be a json encoded as bytes
        print('req = json.loads(summary[rmr.RMR_MS_PAYLOAD])=', req)
        ue_list = req["UEPredictionSet"]
        print('ue_list=req["UEPredictionSet"] =', ue_list)
        self.logger.debug("mr_req_handler processing request for UE list {}".format(ue_list))
    except (json.decoder.JSONDecodeError, KeyError):
        print('////enter first except in mr_req_handler////')
        self.logger.warning("mr_req_handler failed to parse request: {}".format(summary[rmr.RMR_MS_PAYLOAD]))
        return
    print('ue_list mr_req_handler aftr 1st try=', ue_list)
    # iterate over the UEs, fetches data for each UE and perform prediction
    for ueid in ue_list:
        try:
            print('////enter second try in mr_req_handler////')
            uedata = sdl.get_uedata(self, ueid)
            print('uedata = sdl.get_uedata(self, ueid)=', uedata)
            predict(self, uedata)
            print('predict(self, uedata)=', predict(self, uedata))
        except UENotFound:
            print('////enter second except in mr_req_handler////')
            print('enter UENotFound in mr_req_handler')
            self.logger.warning("mr_req_handler received a TS Request for a UE that does not exist!")    
    
 


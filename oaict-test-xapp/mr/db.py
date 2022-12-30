#!/usr/bin/env python
# coding: utf-8


import redis
import influxdb
from influxdb import InfluxDBClient
import pandas as pd
from influxdb import DataFrameClient
import datetime
import influxdb_client




class DBCreateDrop:
    print('///////////enter class DBCreatDrop/////////////')
    def __init__(self, dbname):
        print('enter insert init')

        
class DATABASE(object):
    print('///////////enter class DATABASE(object)////////////////////')
    r""" DATABASE takes an input as database name. It creates a client connection
      to influxDB and It reads/ writes UE data for a given dabtabase and a measurement.
    Parameters
    ----------
    host: str (default='r4-influxdb.ricplt.svc.cluster.local')
        hostname to connect to InfluxDB
    port: int (default='8086')
        port to connect to InfluxDB
    username: str (default='root')
        user to connect
    password: str (default='root')
        password of the use
    Attributes
    ----------
    client: influxDB client
        DataFrameClient api to connect influxDB
    data: DataFrame
        fetched data from database
    """
    host = 'ricplt-influxdb.ricplt'
    #host = 'localhost'
    #def __init__(self, dbname, user='root', password='root', host, port='8086'):
    def __init__(self, dbname, host=host, port='8086'):    
        print('///////enter def __init__ in class DATABASE////////////////')
        self.data = None
        self.client = DataFrameClient(host, port, dbname)
        print('self.client=', self.client)

    def read_data(self, meas, limit=1):
        print('///////enter def read_data(self, meas, limit=1): in class DATABASE///////////')
        """Read data method for a given measurement and limit
        Parameters
        ----------
        meas: str (default='RANMeasReport')
        limit:int (defualt=1)
        """
        #dbname = 'RANData'  ///error: not defined
        #print('dbname=', db) /// error: not defined
        
        print('meas=', meas)
        print('limit=', limit)
        print('str(limit)=', str(limit))
        
        print('self.client.get_list_database()=', self.client.get_list_database())
        self.client.switch_database('kpimon')
        print('self.client.get_list_measurements()=', self.client.get_list_measurements())
        #print('self.client.query(select * from RANData)=', self.client.query('select * from RANData'))
        #print('self.client.query(select * from db)=', self.client.query('select * from db'))
        
        result = self.client.query('select * from ' + meas + ' limit ' + str(limit))
        print('result=', result)
        print("Querying data : " + meas + " : size - " + str(len(result[meas])))
        try:
            print('/////enter try in read-data def///')
            print('result[meas]=', result[meas])
            print('len(result[meas])=', len(result[meas]))
            if len(result[meas]) != 0:
                self.data = result[meas]
                print('self.data==', self.data)
                #self.data['measTimeStampRf'] = self.data.index
                print('self.data.index=', self.data.index)
                print('pd.to_numeric(self.data.index)=', pd.to_numeric(self.data.index))
                #self.data['measTimeStampRf'] = pd.to_numeric(self.data.index)
                #print('self.data[measTimeStampRf]=', self.data['measTimeStampRf'] )
            else:
                print('else:')
                raise NoDataError

        except NoDataError:
            print('except NODataErro:')
            print('Data not found for ' + meas + ' vnf')

    def write_action(self, df, meas='actions'):
        print('///////enter def write_lp_prediction(self, df, meas=LP): in class DATABASE///////////')
        """Write data method for a given measurement
        Parameters
        ----------
        meas: str (default='actions')
        """
        #self.client.write_points(df, meas)
        #print('self.client.write_points(df, meas)=', self.client.write_points(df, meas))
        
        
        
class DATABASE2(object):
    print('///////////enter class DATABASE(object)////////////////////')
    r""" DATABASE takes an input as database name. It creates a client connection
      to influxDB and It reads/ writes UE data for a given dabtabase and a measurement.
    Parameters
    ----------
    host: str (default='r4-influxdb.ricplt.svc.cluster.local')
        hostname to connect to InfluxDB
    port: int (default='8086')
        port to connect to InfluxDB
    username: str (default='root')
        user to connect
    password: str (default='root')
        password of the use
    Attributes
    ----------
    client: influxDB client
        DataFrameClient api to connect influxDB
    data: DataFrame
        fetched data from database
    """
    host = 'ricplt-influxdb.ricplt'
    #host = 'localhost'
    
    bucket = "kpimon"
    org = "my-org"
    token = "client"

    
    #def __init__(self, dbname, user='root', password='root', host, port='8086'):
    def __init__(self, dbname, host=host, port='8086'):    
        print('///////enter def __init__ in class DATABASE////////////////')
        self.data = None
        self.client = DataFrameClient(host, port, dbname)
        print('self.client=', self.client)
        
        self.client2 = influxdb_client.InfluxDBClient(url="http://ricplt-influxdb.ricplt:8086", token = "client", org = "my-org")
        
        print('self.client2=', self.client2)

    def read_data(self, meas, limit=1):
        print('///////enter def read_data(self, meas, limit=1): in class DATABASE///////////')
        """Read data method for a given measurement and limit
        Parameters
        ----------
        meas: str (default='RANMeasReport')
        limit:int (defualt=1)
        """
        #dbname = 'RANData'  ///error: not defined
        #print('dbname=', db) /// error: not defined
        
        print('meas=', meas)
        print('limit=', limit)
        print('str(limit)=', str(limit))
        
        print('self.client.get_list_database()=', self.client.get_list_database())
        self.client.switch_database('kpimon')
        print('self.client.get_list_measurements()=', self.client.get_list_measurements())
        #print('self.client.query(select * from RANData)=', self.client.query('select * from RANData'))
        #print('self.client.query(select * from db)=', self.client.query('select * from db'))
        
        #result1 = self.client.query()
        #print('result1 = self.client.query()=', result1)
        
        query_api = self.client2.query_api()
        print('query_api = self.client.query_api()=', query_api)
        
        
        data_frame = query_api.query_data_frame('from(bucket:"my-bucket") ')
        print(data_frame.to_string())
        
        
        query =' from(bucket:"kpimon")\
        |> filter(fn:(r) => r._measurement == "ricIndication_UeMetrics") '
        result2 = query_api.query(org="my-org", query = query)
        print('result2 = query_api.query(org=my-org)=', result2)

        
        result = self.client.query('select * from ' + meas + ' limit ' + str(limit))
        print('result=', result)
        print("Querying data : " + meas + " : size - " + str(len(result[meas])))
        try:
            print('/////enter try in read-data def///')
            print('result[meas]=', result[meas])
            print('len(result[meas])=', len(result[meas]))
            if len(result[meas]) != 0:
                self.data = result[meas]
                print('self.data==', self.data)
                #self.data['measTimeStampRf'] = self.data.index
                print('self.data.index=', self.data.index)
                print('pd.to_numeric(self.data.index)=', pd.to_numeric(self.data.index))
                #self.data['measTimeStampRf'] = pd.to_numeric(self.data.index)
                #print('self.data[measTimeStampRf]=', self.data['measTimeStampRf'] )
            else:
                print('else:')
                raise NoDataError

        except NoDataError:
            print('except NODataErro:')
            print('Data not found for ' + meas + ' vnf')
            
            
            

class Error(Exception):
    print('////////enter class Error in db////////////////')
    """Base class for other exceptions"""
    pass


class NoDataError(Error):
    print('////////enter class NoDataError in db//////////////////')
    """Raised when there is no data available in database for a given measurment"""
    pass



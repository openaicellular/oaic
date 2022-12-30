#!/usr/bin/env python
# coding: utf-8

# In[ ]:


# ==================================================================================
#       Copyright (c) 2020 China Mobile Technology (USA) Inc. Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# ==================================================================================

import pandas as pd
from influxdb import DataFrameClient
import datetime
import tensorflow as tf
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

import os
import sys
#sys.path.append('C:/Users/Mohammadreza/Desktop/My Class/Proj-DC/My Works/Scheduling/xApp/mr7-main')
sys.path.append('.')

from mr import mobile_env
from mobile_env.handlers.central import MComCentralHandler
from mobile_env.core.base import MComCore
from mobile_env.core.entities import BaseStation, UserEquipment






class INSERTDATA:
    print('///////////////enter INSERTDATA class in populate/////////////')

    def __init__(self):
        print('///////enter insert init////////')

    def createdb(self):
        print('///////enter insert createdb//////////')

    def dropdb(self):
        print('//////////enter insert dropdb/////////')

    def dropmeas(self):
        print('//////////enter insert dropmeas/////////////')



    


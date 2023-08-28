# ==================================================================================
#       Copyright (c) 2023 NextG Wireless Lab Intellectual Property.
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
"""

"""

print("STARTUP")
import time
from keras.utils import  img_to_array
from tensorflow import keras
import sctp, socket
from datetime import datetime
import matplotlib
import matplotlib.pyplot as plt
matplotlib.use('Agg')  # backend required for headless usage
import numpy as np
from PIL import Image
from log import *
import os

print("Imported necessary packages")
PROTOCOL = 'SCTP'
ENABLE_DEBUG = False
SAMPLING_RATE = 7.68e6
spectrogram_time = 0.010  # 10 ms
num_of_samples = SAMPLING_RATE * spectrogram_time
SPEC_SIZE = num_of_samples * 8  # size in bytes, where 8 bytes is the size of one sample (complex64)


cmds = {
    'DYNAMIC_SCHEDULING_ON': b'1',
    'DYNAMIC_SCHEDULING_OFF': b'0'
}


def post_init(self):
    global server

    ip_addr = socket.gethostbyname(socket.gethostname())
    port_srsRAN = 5000 # local port to enable connection to srsRAN
    log_info(self,f"connecting using SCTP on {ip_addr}")
    server = sctp.sctpsocket_tcp(socket.AF_INET)
    server.bind((ip_addr, port_srsRAN))
    server.listen()

    log_info(self, 'server started')


def entry(self):
    global current_iq_data, server
    post_init(self)
    i = 0
    while True:
        try:
            conn, addr = server.accept()
            log_info(self, f'Connected by {addr}')
            initial = time.time()
            while True:
                print(initial)
                if time.time()-initial < 1.0:
                    conn.send(f"E2-like request from {PROTOCOL} server at {datetime.now().strftime('%H:%M:%S')}".encode('utf-8'))
                    log_info(self, "Sent E2-like request")
                    print("time e2 like request sent", time.time())
                    
                # data = conn.recv(4)
                data = conn.recv(16384)
                if data:
                    # # brate = np.frombuffer(data, dtype=np.float32)[0]
                    # data = conn.recv(16384)
                    log_info(self, f"Receiving I/Q data...")
                    while len(data) < SPEC_SIZE:
                        data += conn.recv(16384)
                    recv_ts = time.time()
                    log_info(self, f"Received buffer size {len(data)} with ts {data[0:data.find(b'______')].decode() if data.find(b'______') >= 0 else 'not found'}, received at ts {recv_ts}")
                    log_info(self, f"Finished receiving message, processing")
                    
                    current_iq_data = data
                    iq_handler(self, i)
                    interference_result = run_prediction(self)
                    if interference_result == 'soi+ci' or interference_result == "soi+cwi":
                        log_info(self, "SOI+CWI or SOI+CI detected, sending control message to use adaptive MCS")
                        conn.send(cmds['DYNAMIC_SCHEDULING_ON'])
                            # time.sleep(5)
                    elif interference_result == "soi":
                        log_info(self, "SOI detected, sending control message to to use Fixed MCS")
                        conn.send(cmds['DYNAMIC_SCHEDULING_OFF'])
                    i += 1


        except OSError as e:
            log_error(self, e)
    
def iq_to_spectrogram(iq_data, sampling_rate=7.68e6) -> Image:
    complex_data = np.frombuffer(iq_data, dtype=np.complex64)
    # print(complex_data)
    # print(len(complex_data), 'length of IQ data')
    
    # Create new matplotlib figure
    fig = plt.figure()

    # Create spectrogram from data
    plt.specgram(complex_data, Fs=sampling_rate)
    
    # Manually update the canvas
    fig.canvas.draw()

    w, h = [int(i) for i in fig.canvas.get_renderer().get_canvas_width_height()]
    #print(fig.canvas.tostring_rgb()[2000:3000])
    # Convert image to bytes, then read as a PIL image and return
    return Image.frombytes('RGB', (w, h), fig.canvas.tostring_rgb())

# Converts I/Q data to spectrogram and saves it
def iq_handler(self, i):
    global current_iq_data

    # get i/q sample data
    start_time = time.perf_counter()
    sample = iq_to_spectrogram(current_iq_data)
    if ENABLE_DEBUG:
        log_debug(self, f"Total time for I/Q data conversion: {time.perf_counter() - start_time}")
    
    # save I/Q samples somewhere
    # sample.save(f'samples/{i}.png')

    return "Completed spectrogram"

def run_prediction(self):
    global current_iq_data, result
    start_time = time.perf_counter()
    if current_iq_data is None:
        return None
    sample = iq_to_spectrogram(current_iq_data)
    if ENABLE_DEBUG:
        log_debug(
            self, f"Total time for I/Q data conversion: {time.perf_counter() - start_time}")

    sample = process_image(sample)

    start_time = time.perf_counter()
    result = predict_newdata(sample)
    if ENABLE_DEBUG:
        log_debug(
            self, f"Total time for prediction: {time.perf_counter() - start_time}")

        log_info(self, f"Prediction result: {result}")

    return result


#Load model
def load_model():
    best_model = keras.models.load_model('/home/sdhungel/workarea/oaicrelease/oaic/ric-app-ic/ml-models/icmodel.keras')
    return best_model


# Process the image for appropriate shape to be fed into the model
def process_image(new_img):
    image_width = 128
    image_height = 128
    crop_size= (80,60,557,425)
    new_img = new_img.convert('L')
    processed_image = new_img
    processed_image = processed_image.crop(crop_size)
    processed_image = processed_image.resize((image_width, image_height))
    processed_image = img_to_array(processed_image)
    processed_image = np.array(processed_image).astype(np.float32)
    processed_image = processed_image/255.0
    processed_image = np.expand_dims(processed_image, axis=0)
    return processed_image


def predict_newdata(sample):
    model = load_model()
    prob_pred = model.predict(sample)
    predicted_label = np.argmax(prob_pred, axis=1)
    predicted_label = predicted_label[0]

    if predicted_label == 0:
        predicted_label = 'soi'
    elif predicted_label == 1:
        predicted_label = 'soi+cwi'
    else:
        predicted_label = 'soi+ci'
    return predicted_label



def start(thread=False):
    entry(None)




if __name__ == '__main__':
    # ai_model = load_model_parameter()
    start()

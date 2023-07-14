import sctp, socket
import time

# Spectrogram generation
import matplotlib
import matplotlib.pyplot as plt
matplotlib.use('Agg')  # backend required for headless usage
import numpy as np
from PIL import Image

from log import *

CONFIDENCE_THRESHOLD = 0.5
SAMPLING_RATE = 15360000

spectrogram_time = 0.005  # 5 ms
num_of_samples = SAMPLING_RATE * spectrogram_time
spectrogram_size = num_of_samples * 8  # size in bytes, where 8 bytes is the size of one sample (complex64)

cmds = {
    'BASE_STATION_ON': b'y',
    'BASE_STATION_OFF': b'n'
}

current_iq_data = None
last_cmd = None
server = None


def init_e2(self):
    global server

    # This will automatically find a correct IP address to use, and works well in the RIC.
    ip_addr = socket.gethostbyname(socket.gethostname())

    # 0.0.0.0 will accept connections from all interfaces, which works great for development.
    # However, inside the RIC, this will cause the SCTP connection to disconnect immediately
    # after any connection begins. More info in the link below:
    # https://github.com/kubernetes/kubernetes/issues/83306#issuecomment-1065778854
    #ip_addr = "0.0.0.0"

    port = 5000

    log_info(self, f"E2-like enabled, connecting using SCTP on {ip_addr}")

    server = sctp.sctpsocket_tcp(socket.AF_INET)
    server.bind((ip_addr, port)) 
    server.listen()

    log_info(self, 'Server started')


def entry(self):
    global current_iq_data, last_cmd, server

    # Initialize the E2-like interface
    init_e2(self)

    # E2-like interface main loop
    while True:
        try:
            # Accept SCTP connections
            conn, addr = server.accept()

            log_info(self, f'Connected by {addr}')

            initial = time.time()  # initial timestamp

            # Loop which runs if an SCTP connection is established
            while True:
                # Send an E2-like request within the first second to ask nodeB to send I/Q data
                if initial - time.time() < 1.0:
                    conn.send(f"E2-like request at {datetime.now().strftime('%H:%M:%S')}".encode('utf-8'))
                    log_info(self, "Sent E2-like request")

                # Sending too much SCTP data in a single message will freeze the connection up, so we have srsRAN split our data
                # into chunks of 16384 bytes. The data in this case is I/Q data sourced from the RU (radio unit).
                # This section of code will receive enough I/Q data to make one 10ms spectrogram.
                data = conn.recv(16384)
                if data:
                    log_info(self, f"Receiving I/Q data...")
                    while len(data) < spectrogram_size:
                        data += conn.recv(16384)
                
                    log_info(self, f"Received buffer size {len(data)}")
                    log_info(self, f"Finished receiving message, processing")
                    
                    # Point our global variable to the I/Q data we just received, and use our machine learning model
                    # to make a prediction.
                    current_iq_data = data
                    result = run_prediction(self)

                    # If there is interference, send a command to turn the base station off.
                    # We can do this by setting the transmit (TX) gain of the radio to a very weak amount (-10000dB)
                    # If the interference goes away, send a command to turn it back on.
                    if result == 'Interference':
                        log_info(self, "Interference signal detected, sending control message to turn nodeB off")
                        conn.send(cmds['BASE_STATION_OFF'])
                        #last_cmd = cmds['BASE_STATION_OFF']
                    elif result in ('5G', 'LTE'): #and last_cmd == cmds['BASE_STATION_OFF']:
                        log_info(self, "Interference signal no longer detected, sending control message to turn nodeB on")
                        conn.send(cmds['BASE_STATION_ON'])
                        #last_cmd = cmds['BASE_STATION_ON']

        # Log any errors with the SCTP connection, but continue to run
        except OSError as e:
            log_error(self, e)


def run_prediction(self):
    global current_iq_data

    # convert I/Q data into a spectrogram that our machine learning model can use as input
    sample = iq_to_spectrogram(current_iq_data)
    # Make a prediction with our spectrogram and get the result
    result = predict(self, sample)

    return result


def iq_to_spectrogram(iq_data, sampling_rate=SAMPLING_RATE) -> np.ndarray:
    # Convert I/Q data in 1-dimensional array into a spectrogram image as a numpy array

    # The I/Q data is in [I,Q,I,Q,...] format
    # Each one is a 32-bit float so we can combine them easily by reading the array
    # as complex64 (made of two 32-bit floats)
    complex_data = np.frombuffer(iq_data, dtype=np.complex64)
    
    # Create new matplotlib figure
    fig = plt.figure()

    # Create spectrogram from data
    plt.specgram(complex_data, Fs=sampling_rate)
    
    # Manually update matplotlib's canvas
    fig.canvas.draw()

    # Get width and height of the canvas
    w, h = [int(i) for i in fig.canvas.get_renderer().get_canvas_width_height()]

    # Convert image to bytes, then read as a PIL image and return
    return np.array(Image.frombytes('RGB', (w, h), fig.canvas.tostring_rgb()))


def predict(self, data) -> str:
    # Actually do the prediction. This will be dependent on your model.
    prediction, confidence = model_predict(ai_model, data)

    classifiers = ['5G', 'LTE', 'Interference']

    return classifiers[prediction] if confidence > CONFIDENCE_THRESHOLD else None


def model_predict(model, unseen_data):
    # Instead of implementing a real model, we will simply use random values

    prediction = random.randint(0,2)
    confidence = random.random()

    return prediction, confidence


def load_model_parameter():
    # In this code, we don't actually include a model, but this is where you would load your model and return it.
    return None


def start(thread=False):
    global ai_model
    ai_model = load_model_parameter()
    
    # For an E2-compliant xApp, we would have to create and pass an xApp instance to our entry function.
    # However, for E2-like we simply can pass None.
    entry(None)


if __name__ == '__main__':
    start()
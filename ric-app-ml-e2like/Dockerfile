# Load a miniconda setup for our base Docker image which contains Python
FROM continuumio/miniconda3

# Install all necessary libraries
RUN apt-get update && apt-get -y install build-essential musl-dev libjpeg-dev zlib1g-dev libgl1-mesa-dev wget dpkg

# Copy all the files in the current directory to /tmp/ml in our Docker image
COPY . /tmp/ml

# Go to /tmp/ml
WORKDIR /tmp/ml

# Install requirements.txt
RUN pip install --upgrade pip && pip install -r requirements.txt

# Set our xApp to run immediately when deployed
ENV PYTHONUNBUFFERED 1
CMD python3 app.py

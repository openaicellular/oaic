
# FROM ubuntu
# RUN apt-get update
# RUN apt-get install -y redis-server
# RUN service redis-server start
# RUN service redis-server start
# RUN redis-server --daemonize yes

# FROM redis:6.2.1-alpine
# RUN apk -U upgrade \
#   && apk --no-cache --update add build-base
# EXPOSE 6379
# # ENV REDIS_URL=redis://redis:6379/1

FROM ubuntu
RUN apt-get update       
RUN apt-get install --assume-yes git
FROM frolvlad/alpine-miniconda3


# RMR setup
RUN mkdir -p /opt/route_mr/

# copy rmr files from builder image in lieu of an Alpine package
COPY --from=nexus3.o-ran-sc.org:10002/o-ran-sc/bldr-alpine3-rmr:4.0.5 /usr/local/lib64/librmr* /usr/local/lib64/

COPY --from=nexus3.o-ran-sc.org:10002/o-ran-sc/bldr-alpine3-rmr:4.0.5 /usr/local/bin/rmr* /usr/local/bin/
ENV LD_LIBRARY_PATH /usr/local/lib/:/usr/local/lib64
COPY local.rt /opt/route_mr/local.rt
ENV RMR_SEED_RT /opt/route_mr/local.rt


RUN apk update && apk add gcc musl-dev && apk add git
# RUN apt-get update \        
#      apt-get install -y git
RUN pip install ricxappframe

# RUN apt-get update  \
#   && apt-get install -y
#RUN cd /opt/route_mr
#RUN git clone https://github.com/o-ran-sc/ric-plt-sdlpy.git
#WORKDIR ric-plt-sdlpy/ricsdl-package
#RUN python3 setup.py install

RUN python3 -m pip install ricsdl

EXPOSE 8585

# Install
COPY setup.py /tmp
COPY LICENSE.txt /tmp/
# RUN mkdir -p /tmp/ad/
COPY mr/ /mr
RUN pip install /tmp


ENV PYTHONUNBUFFERED=1
ENV PYTHONIOENCODING=UTF-8


#ENV PYTHONPATH "${PYTHONPATH}:."
#CMD run-mr.py

#CMD python3 -u /mr/main.py

CMD PYTHONPATH=/mr:/usr/lib/python3.7/site-packages/:$PYTHONPATH run-mr.py

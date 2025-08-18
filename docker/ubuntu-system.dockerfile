FROM ubuntu:25.04
WORKDIR /home/ubuntu
ENV DISPLAY=:0

RUN apt update
RUN apt install -y sudo git \
    openjdk-8-jdk \
    gcc g++ \
    cmake make \
    libboost-all-dev \
    tcl-dev tk-dev \
    emacs

USER ubuntu
FROM gcc
FROM python

RUN apt update
RUN apt install -y linux-perf
RUN apt install -y cmake

RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
RUN mkdir /build

CMD ["/bin/bash"]
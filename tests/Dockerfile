FROM gcc

FROM python
COPY --from=0 /usr/local /usr/local

RUN apt update
RUN apt install -y linux-perf

ENV CMAKE_VER=3.13.2
COPY cmake-*-Linux-x86_64.sh /cmake_install.sh
RUN ls cmake_install.sh || wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VER/cmake-$CMAKE_VER-Linux-x86_64.sh \
    -O cmake_install.sh
RUN chmod a+x cmake_install.sh && mkdir /opt/cmake
RUN ./cmake_install.sh --skip-license --prefix=/opt/cmake && rm cmake_install.sh
ENV PATH="/opt/cmake/bin:${PATH}"

RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
ENV CC=/usr/local/bin/gcc
ENV CXX=/usr/local/bin/g++
RUN mkdir /build

CMD ["/bin/bash"]
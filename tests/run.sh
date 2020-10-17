#!/bin/bash
eval $(docker-machine env --shell bash)
docker run --rm -v "/$PWD":/rpp rikorose/gcc-cmake bash -c \
    "mkdir build && cd build && cmake /rpp && make && cp rpp /rpp"
docker run --rm -v "/$PWD":/rpp python bash -c \
    "cd rpp && PYTHONPATH=/rpp python /rpp/tests/run_tests.py"

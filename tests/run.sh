#!/bin/bash
eval $(docker-machine env --shell bash)
docker run --privileged --rm -v "/$PWD":/rpp rpp bash -c \
"cd build && cmake /rpp  && make && \
PYTHONPATH=/rpp python /rpp/tests/run_tests.py && \
PYTHONPATH=/rpp python /rpp/tests/run_benchmarks.py"
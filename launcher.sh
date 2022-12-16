#!/bin/bash

trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM EXIT

./ServoServer.py &
./FaceposeEstimation.exe &

wait


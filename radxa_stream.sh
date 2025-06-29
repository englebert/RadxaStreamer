#!/bin/bash
# bin/bash ./radxa_stream.sh -dbg -n -cd H264 -fl -w 1280 -h 720 -fps 60 -g 15 -b 14400000 -ih -sl 4 -pf high -lev 4.2 -if both -br 50 -co 0 -sa -40 -sh 0 -drc off -ex off -awb off -mm backlit -ag 2.0 -awbg 1.4,1.5 -log -t 0 -o -
killall radxa_streamer
./radxa_streamer $@

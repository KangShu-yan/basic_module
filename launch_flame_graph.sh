#!/bin/bash

echo "launch_flame_graph ... "
echo $1

sudo perf record -F 99 -p $1 -g -- sleep 60
sudo perf script -i perf.data &> perf.unfold

sudo ./stackcollapse-perf.pl perf.unfold &> perf.folded

sudo ./flamegraph.pl perf.folded > perf.svg



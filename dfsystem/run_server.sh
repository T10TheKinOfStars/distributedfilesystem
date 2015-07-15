#!/bin/sh

./server 9090 &
sleep 1s
./server 9091 &
sleep 1s
./server 9092 &
sleep 1s
./server 9093 &

#!/bin/bash
gcc 00_marblerun.c -o marblerun -Wall -std=gnu99 -g -lrt `sdl2-config --cflags --libs`
exit 0

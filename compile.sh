#!/bin/bash
gcc marblerun.c -o marblerun -Wall -std=gnu99 -g -lm -lrt `sdl2-config --cflags --libs`
exit 0

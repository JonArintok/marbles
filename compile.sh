#!/bin/bash
gcc marblerun.c -o marblerun.o -Wall -std=gnu99 -g -lrt `sdl2-config --cflags --libs`
exit 0

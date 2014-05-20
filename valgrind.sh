#!/bin/bash
valgrind --leak-check=full ./marblerun $1
exit 0

#!/bin/bash

rm -rf MASTER.PRG mastera.o

#Build assembly helper-functions
ca65 -t cx16 -o mastera.o mastermind.asm
#Build and link main program
cl65 -t cx16 \
     -m mastermind.map -Ln mastermaind.sym -o MASTER.PRG \
     mastermind.c mastera.o

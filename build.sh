#!/bin/bash

rm -rf COMMIND.PRG comminda.o

#Build assembly helper-functions
ca65 -t cx16 -o comminda.o commandermind.asm
#Build and link main program
cl65 -t cx16 \
     -m commandermind.map -Ln commandermind.sym -o COMMIND.PRG \
     commandermind.c comminda.o

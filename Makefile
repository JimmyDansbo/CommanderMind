all:
	ca65 -t cx16 -o comminda.o commandermind.asm
	cl65 -t cx16 -m commandermind.map -Ln commandermind.sym -o COMMIND.PRG commandermind.c comminda.o
clean:
	rm -rf COMMIND.PRG commandermind.o commandermind.map commandermind.sym

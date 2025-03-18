all:
	ca65 -t cx16 -o mastera.o mastermind.asm
	cl65 -t cx16 -m mastermind.map -Ln mastermind.sym -o MASTER.PRG mastermind.c mastera.o
clean:
	rm -rf MASTER.PRG mastera.o mastermind.map mastermind.sym

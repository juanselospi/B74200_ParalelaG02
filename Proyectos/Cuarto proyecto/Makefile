all: sequences.out

%.o: %.cc
	mpic++ -g -c $<

%.o: %.c
	mpicc -g -c $<

sequences.out: sequences.o adn.o lcs.o
	mpic++ -g sequences.o adn.o lcs.o -o sequences.out

clean:
	rm -f *.o *.out

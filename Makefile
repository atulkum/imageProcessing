CC = g++
CFLAGS = -g -Wall
LIBS = -lresolv

all:  HalfToning EdgeDetector Morphing preContrast preNoiseRemoval

preNoiseRemoval: preNoiseRemoval.o
	$(CC) $(CFLAGS) preNoiseRemoval.o -o preNoiseRemoval

preNoiseRemoval.o: preNoiseRemoval.cpp
	$(CC) -c $(CFLAGS) preNoiseRemoval.cpp

preContrast: preContrast.o
	$(CC) $(CFLAGS) preContrast.o -o preContrast

preContrast.o: preContrast.cpp
	$(CC) -c $(CFLAGS) preContrast.cpp

HalfToning: HalfToning.o
	$(CC) $(CFLAGS) HalfToning.o -o HalfToning

HalfToning.o: HalfToning.cpp
	$(CC) -c $(CFLAGS) HalfToning.cpp

EdgeDetector: EdgeDetector.o
	$(CC) $(CFLAGS) EdgeDetector.o -o EdgeDetector

EdgeDetector.o: EdgeDetector.cpp
	$(CC) -c $(CFLAGS) EdgeDetector.cpp

Morphing: Morphing.o
	$(CC) $(CFLAGS) Morphing.o -o Morphing

Morphing.o: Morphing.cpp
	$(CC) -c $(CFLAGS) Morphing.cpp

clean:
	rm -f *~ *.o *.bak core HalfToning EdgeDetector Morphing preContrast preNoiseRemoval

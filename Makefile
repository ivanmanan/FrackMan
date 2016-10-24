CC = g++    # On lnxsrv07 or 09, replace this with /usr/local/cs/bin/g++
LIBS = -lglut -lGL -lGLU
RPATH = 

#Actor.o : Actor.cpp Actor.h
#	cc -c Actor.cpp


# On lnxsrv07 or 09, uncommment the following line
#RPATH = -Wl,--rpath=/usr/local/cs/lib64

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

.PHONY: default all clean

all: frackman

%.o: %.cpp $(HEADERS)
	$(CC) -c -std=c++11 $< -o $@

frackman: $(OBJECTS) 
	$(CC) $(RPATH) $(OBJECTS) $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f FrackMan

FILES = grabcut main
LDFLAGS = `pkg-config --cflags --libs opencv`

all: main

main: 
	g++ -Wall -o main treshold.cpp main.cpp $(LDFLAGS)



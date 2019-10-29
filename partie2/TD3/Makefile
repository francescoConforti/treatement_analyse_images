FILES = grabcut main
LDFLAGS = `pkg-config --cflags --libs opencv`

all: division_fusion grabcut main clean

main: main.o
	g++ -Wall -o main main.o $(LDFLAGS)

main.o: main.cpp
	g++ -Wall -c main.cpp $(LDFLAGS)

grabcut: grabcut.o
	g++ -Wall -o grabcut grabcut.o $(LDFLAGS)

grabcut.o: grabcut.cpp
	g++ -Wall -c grabcut.cpp $(LDFLAGS)
	
division_fusion: division_fusion.o
	g++ -Wall -o division_fusion division_fusion.o $(LDFLAGS)

division_fusion.o: division_fusion.cpp
	g++ -Wall -c division_fusion.cpp $(LDFLAGS)

clean:
	rm *.o

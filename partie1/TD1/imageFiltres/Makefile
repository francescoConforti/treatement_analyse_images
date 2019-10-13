FILES = filtrage_Q1_TP freeman_extraction
LDFLAGS = `pkg-config --cflags --libs opencv`

all: filtrage_Q1_TP freeman_extraction

freeman_extraction: freeman_extraction.o
	g++ -Wall -o freeman_extraction freeman_extraction.o $(LDFLAGS)

freeman_extraction.o: freeman_extraction.cpp
	g++ -Wall -c freeman_extraction.cpp $(LDFLAGS)

filtrage_Q1_TP: filtrage_Q1_TP.o
	g++ -Wall -o filtrage_Q1_TP filtrage_Q1_TP.o $(LDFLAGS)

filtrage_Q1_TP.o: filtrage_Q1_TP.cpp
	g++ -Wall -c filtrage_Q1_TP.cpp $(LDFLAGS)

clean:
	rm -f *.o *~ $(FILES)

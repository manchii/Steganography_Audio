CC=g++
CFLAGS=-std=gnu++11 -lm -lfftw3
SRC=main_encoder.cpp AudioFile.cpp
SRCDEC=main_decoder.cpp AudioFile.cpp

main: main_encoder.cpp
	$(CC) -o proj.o $(SRC) $(CFLAGS)
encoder: main_encoder.cpp
	$(CC) -o encoder.o $(SRC) $(CFLAGS)
decoder: main_decoder.cpp
	$(CC) -o decoder.o $(SRCDEC) $(CFLAGS)
clean:
	rm *.o

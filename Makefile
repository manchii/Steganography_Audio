CC=g++
CFLAGS=-std=c++11 -lm -lfftw3
SRCDIR=src
BINDIR=bin

SRCENC=$(SRCDIR)/main_encoder.cpp $(SRCDIR)/AudioFile.cpp
SRCDEC=$(SRCDIR)/main_decoder.cpp $(SRCDIR)/AudioFile.cpp
SRCERR=$(SRCDIR)/error.cpp
OBJ = $(SRCDIR)/main_encoder.cpp $(SRCDIR)/main_decoder.cpp $(SRCDIR)/error.cpp

ENCEXE = encoder
DECEXE = decoder
ERREXE = error

main: $(OBJ)
	$(CC) -o $(BINDIR)/$(ENCEXE) $(SRCENC) $(CFLAGS)
	$(CC) -o $(BINDIR)/$(DECEXE) $(SRCDEC) $(CFLAGS)
	$(CC) -o $(BINDIR)/$(ERREXE) $(SRCERR) $(CFLAGS)
encoder: main_encoder.cpp
	$(CC) -o $(BINDIR)/$(ENCEXE) $(SRCENC) $(CFLAGS)
decoder: main_decoder.cpp
	$(CC) -o $(BINDIR)/$(DECEXE) $(SRCDEC) $(CFLAGS)
error: error.cpp
	$(CC) -o $(BINDIR)/$(ERREXE) $(SRCERR) $(CFLAGS)
clean:
	rm $(BINDIR)/$(ENCEXE)
	rm $(BINDIR)/$(DECEXE)
	rm $(BINDIR)/$(ERREXE)

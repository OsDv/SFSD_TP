IDIR =include
CC=gcc
CFLAGS=-I$(IDIR) -g

ODIR=bin
SDIR=src

_DEPS = utils.h TOF.h TOVS.h lib.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o utils.o TOF.o TOVS.o lib.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

FilesManager: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o

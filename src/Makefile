
INCLUDEDIR = -I/usr/local/include
CC=gcc
EXEC = http_server
SRCDIR = .
SRC = $(wildcard $(SRCDIR)/*.c)

LIBS = -lpthread

all: $(EXEC) 
$(EXEC): $(SRC:.cpp=.o)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -W
	
clean:
	-rm -f $(EXEC) *.o 




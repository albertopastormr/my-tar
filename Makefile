TARGET = mytar
CC = gcc
CFLAGS = -g -Wall 
OBJS = mytar.o mytar_routines.o crc.o
SOURCES = $(addsuffix .c, $(basename $(OBJS)))
HEADERS = mytar.h crc.h

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

#.c.o: 
#	$(CC) $(CFLAGS) -c  $< -o $@

$(OBJS): $(HEADERS)

clean: 
	-rm -f *.o $(TARGET) 

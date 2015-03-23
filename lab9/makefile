CC = gcc
PORT=58298
CFLAGS = -DPORT=$(PORT) -g -Wall

all: readserver randclient bufserver

readserver: readserver.o
	${CC} ${CFLAGS} -o $@ readserver.o

randclient: randclient.o
	${CC} ${CFLAGS} -o $@ randclient.o

bufserver: bufserver.o
	${CC} ${CFLAGS} -o $@ bufserver.o

%.o: %.c
	${CC} ${CFLAGS}  -c $<

clean:
	rm *.o readserver randclient bufserver

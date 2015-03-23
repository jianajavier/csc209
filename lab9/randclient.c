#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#ifndef PORT
  #define PORT 30000
#endif

#define TIMES 5 // number of times to send the message
#define MINCHARS 3
#define MAXCHARS 7

int main(int argc, char* argv[]) {
  int soc, err;
  char port[10];
  char message[18] = "A stitch in time\r\n";
  struct addrinfo *info, hints;
  struct sockaddr_in peer;

  int current_byte, bytes_left, total_bytes, howmany;
  int i;
  char piece[MAXCHARS];

  sprintf (port, "%d", PORT);

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if ( argc != 2) {
    fprintf(stderr, "Usage: %s hostname\n", argv[0]);
    exit(1);
  }
  if ((err = getaddrinfo(argv[1], port, &hints, &info))) {
    fprintf (stderr, "%s\n", gai_strerror(err));
    exit (1);
  }
  peer = *(struct sockaddr_in *)(info->ai_addr);

  if ((soc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror ("socket");
    exit (1);
  }

  if (connect(soc, (struct sockaddr *)&peer, sizeof(peer)) == -1) {
    perror ("connect");
    exit (1);
  }

  total_bytes = TIMES * sizeof(message);
  current_byte = 0;
  while (current_byte < total_bytes) {
    howmany = random() % (MAXCHARS - MINCHARS + 1) + MINCHARS;
    bytes_left = total_bytes - current_byte;
    if (howmany > bytes_left)
      howmany = bytes_left;
    for (i = 0; i < howmany; i++)
      piece[i] = message[(current_byte + i) % sizeof(message)];
    write(soc, piece, howmany);
    current_byte += howmany;
  }
  close(soc);
  return(0);
}

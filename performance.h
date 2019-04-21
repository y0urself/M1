#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define UDP_OVERHEAD 8
#define TEST_RUNS 10000 /* Number of runs for each test */
#define RTT_RUNS 200

struct conn
{
  int socket;
  struct sockaddr_in addr;
  socklen_t size;
};

void error(char *msg)
{
  perror(msg);
  exit(1);
}

int max(int x, int y) 
{ 
  if (x > y)
  {
    return x; 
  }
  else
  {
    return y; 
  }
} 

int sending(struct conn udp, char** buffer);

int receiving(struct conn udp, char** buffer);

#endif /* PERFORMANCE_H */
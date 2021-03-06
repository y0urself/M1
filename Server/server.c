#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/select.h>

#include <arpa/inet.h>

#include <time.h>
#include <unistd.h>
#include "../performance.h"
#include <sys/time.h>

#include <inttypes.h>

void rtt(struct conn udp, int client);
void ploss(struct conn udp, struct conn tcp, int client);
void bwidth(struct conn udp, struct conn tcp, int client);
void bneck(struct conn udp, int client);

int main(int argc, char** argv)
{
  int mode = 0;
  int ready_fd, max_fd;
  /* TCP SOCKET PROPERTIES */
  int tcp_server_fd = 0;
  int tcp_client_fd = 0;
  int port;

  struct sockaddr_in tcp_server_address;
  struct sockaddr_in tcp_client_address;

  struct timeval t_val;
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;

  tcp_server_fd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&tcp_server_address, 0x00, sizeof(tcp_server_address));
  memset(&tcp_client_address, 0x00, sizeof(tcp_client_address));

  port = 5000; // TODO give udp_port over args

  tcp_server_address.sin_family = AF_INET;
  tcp_server_address.sin_addr.s_addr = INADDR_ANY;
  tcp_server_address.sin_port = htons(port); 

  struct conn tcp;
  tcp.socket = tcp_server_fd;
  tcp.addr = tcp_server_address;
  tcp.size = sizeof(tcp_client_address);

  /* UDP SOCKET PROPERTIES */
  int udp_server_fd = 0;
  int udp_client_fd = 0;
  int optval;
  int len;

  struct sockaddr_in udp_server_address;
  struct sockaddr_in udp_client_address;

  socklen_t udp_client_len;

  char buffer[BUF_SIZE];

  time_t tick;

  int i = 0;

  udp_server_fd = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&udp_server_address, 0x00, sizeof(udp_server_address));
  memset(&udp_client_address, 0x00, sizeof(udp_client_address));
  memset(buffer, 0x00, sizeof(buffer));

  port = 5000; // TODO give udp_port over args

  udp_server_address.sin_family = AF_INET;
  udp_server_address.sin_addr.s_addr = INADDR_ANY;
  udp_server_address.sin_port = htons(port);

  struct conn udp;
  udp.socket = udp_server_fd;
  udp.addr = udp_server_address;
  udp.size = sizeof(udp_client_address);

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   * https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/udpserver.c
   */
  optval = 1;
  setsockopt(udp.socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(optval));
  setsockopt(tcp.socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(optval));

  // bind the sockets to the specific address
  if(bind(tcp.socket, (struct sockaddr*)&tcp.addr, tcp.size) < 0)
  {
    error("ERROR on binding");
  }

  if(listen(tcp.socket, 1) < 0)
  {
    error("ERROR on listen");
  }

  tcp_client_fd = accept(tcp.socket, (struct sockaddr*)&tcp.addr, &tcp.size);

  recv(tcp_client_fd, buffer, sizeof(buffer), 0);
  printf("%s\n", buffer);
  if(strcmp(buffer, "rtt") == 0)
  {
    mode = 1;
  }
  if(strcmp(buffer, "ploss") == 0)
  {
    mode = 2;
  }
  else if(strcmp(buffer, "bwidth") == 0)
  {
    mode = 3;
  }
  else if(strcmp(buffer, "bneck") == 0)
  {
    mode = 4;
    int timeopt = 1;
    setsockopt(udp.socket, SOL_SOCKET, SO_TIMESTAMP, (const void *)&timeopt , sizeof(timeopt));
  }
  
  if(bind(udp.socket, (struct sockaddr*)&udp.addr, udp.size) < 0)
  {
    error("ERROR on binding");
  }

  printf("TCP established\n");

  send(tcp_client_fd, buffer, sizeof(buffer), 0);

  if(mode == 1)
  {
    rtt(udp, tcp_client_fd);
  }
  else if(mode == 2)
  {
    ploss(udp, tcp, tcp_client_fd);
  }
  else if(mode == 3)
  {
    bwidth(udp, tcp, tcp_client_fd);
  }
  else if(mode == 4)
  {
    bneck(udp, tcp_client_fd);
  }
  close(udp_server_fd);
  close(tcp_server_fd);
}

void rtt(struct conn udp, int client)
{
  int i = 0;
  char buffer[BUF_SIZE];
  char back[BUF_SIZE];
  struct timeval t_val;
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;
  int rx, tx;

  int max_fd = 0;
  int ready_fd = 0;
  fd_set rset;

  max_fd = max(client, udp.socket) + 1;

  while(42)
  {
    FD_SET(client, &rset);
    FD_SET(udp.socket, &rset);

    ready_fd = select(max_fd, &rset, NULL, NULL, &t_val);

    //printf("%d\n", i);

    if(ready_fd < 0)
    {
      error("Error in select?");
    }
    else
    {
        if(FD_ISSET(client, &rset))
        {
        printf("hello %d\n", i);

        recv(client, back, sizeof(back), 0);

        printf("%s\n", back);

        if(strcmp(back, "back") == 0)
        {
          break;
        }
      }

      if(FD_ISSET(udp.socket, &rset))
      {
        rx = recvfrom(udp.socket, buffer, BUF_SIZE, 0, (struct sockaddr *)&udp.addr, &udp.size);
        if (rx < 0)
        {
          error("ERROR in recvfrom");
        }

        tx = sendto(udp.socket, buffer, strlen(buffer), 0, (struct sockaddr *)&udp.addr, udp.size);
        if (tx < 0)
        {
          error("ERROR in sendto");
        }
      }
    }
  }
}

void ploss(struct conn udp, struct conn tcp, int client)
{
  char buffer[BUF_SIZE];
  char back[5];
  int max_fd, ready_fd;
  int i = 0;
  int n = TEST_RUNS;
  int rx, tx;
  fd_set rset;
  fd_set wset;

  struct timeval t_val;
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;

  max_fd = max(client, udp.socket) + 1;
  while(42)
  {
  FD_SET(client, &rset);
  FD_SET(udp.socket, &rset);

  ready_fd = select(max_fd, &rset, NULL, NULL, &t_val);

  if(ready_fd < 0)
  {
    error("Error in select?");
  }
  else
  {
    if(FD_ISSET(client, &rset))
    {
      printf("hello %d\n", i);

      recv(client, back, sizeof(back), 0);

      printf("%s\n", back);

      if(strcmp(back, "back") == 0)
      {
        break;
      }
    }

    if(FD_ISSET(udp.socket, &rset))
    {
      rx = recvfrom(udp.socket, buffer, BUF_SIZE, 0, (struct sockaddr *)&udp.addr, &udp.size);
      if (rx < 0)
      {
        error("ERROR in recvfrom");
      }

      i++;
      }
    }
  }

  /* send number of packets received back to client */
  memcpy(back, (char*)&i, sizeof(int));

  //printf("TCP closing\n");

  send(client, back, sizeof(back), 0);

  sleep(3);

  strcpy(buffer, "Ja ich weiß, es tut mir wirklich schrecklich leid!");
  i = 0;

  while(i < n)
  {
    tx = sendto(udp.socket, buffer, strlen(buffer), 0, (struct sockaddr *)&udp.addr, udp.size);
    if (tx < 0) 
    {
      error("ERROR in sendto");
    }  
    //printf("Sended, %d\n", i );
    i++;
    usleep(10);
  }
  printf("%d\n", i);

  strcpy(back, "end.");
  send(client, back, sizeof(back), 0);
  recv(client, back, sizeof(back), 0);
}

void bwidth(struct conn udp, struct conn tcp, int client)
{
  printf("bwidth\n");
  char buffer[BUF_SIZE];
  char back[9];
  memset(back, 0x00, sizeof(back));
  memset(buffer, 0x00, sizeof(buffer));

  fd_set rset;
  int ready_fd;
  int i = 0;
  int rx;
  int n = TEST_RUNS;

  struct timeval t_val;
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;

  int max_fd = max(client, udp.socket) + 1;
  FD_ZERO(&rset);
  while(42)
  {
    FD_SET(client, &rset);
    FD_SET(udp.socket, &rset);

    ready_fd = select(max_fd, &rset, NULL, NULL, &t_val); 

    //printf("%d\n", i);

    if(ready_fd < 0)
    {
      error("Error in select?");
    }
    // else if(ready_fd == 0)
    // {
    //   printf("wtf\n");
    // }
    else
    {
      if(FD_ISSET(client, &rset))
      {
        //printf("hello %d\n", i);

        recv(client, buffer, sizeof(buffer), 0);

        memcpy(buffer, (char*)&i, sizeof(int));

        printf("TCP closing\n");

        send(client, buffer, sizeof(buffer), 0);
        break;
      }
    
      if(FD_ISSET(udp.socket, &rset))
      {
        rx = recvfrom(udp.socket, buffer, BUF_SIZE, 0, (struct sockaddr *)&udp.addr, &udp.size);
        if (rx < 0)
        {
          error("ERROR in recvfrom");
        }
        //FD_CLR(udp.socket, &rset);
        i++;
        //printf("%s, %d\n", buffer, i);
      }
      // FD_ZERO(&rset);
      // usleep(10); 
    }
  }

  struct timeval before;
  before.tv_sec = 0;
  before.tv_usec = 0;

  struct timeval after;
  after.tv_sec = 0;
  after.tv_usec = 0;

  i = 0;

  gettimeofday(&before, NULL);
  while(i < n)
  { 
    // FD_ZERO(&rset);
    // FD_SET(udp.socket, &rset);

    // if(FD_ISSET(udp.socket, &rset))
    // {
      rx = sendto(udp.socket, buffer, strlen(buffer), 0, (struct sockaddr *)&udp.addr, udp.size);
      //udp_len = send(udp.socket, buffer, strlen(buffer), 0);
      if (rx < 0) 
      {
        error("ERROR in sendto");
      }
      //printf("Sent %s, len %d\n", buffer, rx);
    //   FD_CLR(udp.socket, &rset);
    // }    
    i++;
    //printf("%d\n", i);
  }
  gettimeofday(&after, NULL);

  uint64_t m = 1000000;
  uint64_t  stamp0, stamp1;
  stamp0 = before.tv_sec * m + before.tv_usec;
  stamp1 = after.tv_sec * m + after.tv_usec;
  uint64_t time = stamp1 - stamp0;

  memcpy(back, (char*)&time, sizeof(uint64_t));
  send(client, back, sizeof(back), 0);

  recv(client, back, sizeof(back), 0);
}

void bneck(struct conn udp, int client)
{
  uint64_t m = 1000000;
  struct timeval t1, t2;
  int rx1 = 0, rx2 = 0;
  int i = 0;
  int max_fd;

  char back[10];
  char buffer[BUF_SIZE];
  char buffer2[BUF_SIZE];
  int len = strlen(buffer);

  memset(buffer, 0, sizeof(buffer));
  memset(buffer2, 0, sizeof(buffer2));

  struct iovec io_vec1;
  io_vec1.iov_base = buffer;
  io_vec1.iov_len = BUF_SIZE;
  struct iovec io_vec2;
  io_vec2.iov_base = buffer2;
  io_vec2.iov_len = BUF_SIZE;

  unsigned char cbuffer[BUF_SIZE];
  memset(cbuffer, 0, sizeof(cbuffer));
  int clen = sizeof(cbuffer);
  unsigned char cbuffer2[BUF_SIZE];
  memset(cbuffer2, 0, sizeof(cbuffer2));
  int clen2 = sizeof(cbuffer2);

  fd_set rset;
  int ready_fd;

  struct timeval t_val;
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;
  
  max_fd = max(client, udp.socket) + 1;

  FD_ZERO(&rset);
  while(42)
  {
    FD_SET(client, &rset);
    FD_SET(udp.socket, &rset);
    ready_fd = select(max_fd, &rset, NULL, NULL, &t_val);

    struct msghdr msg = {};
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = &udp.addr;
    msg.msg_namelen = udp.size;
    msg.msg_iov = &io_vec1;
    msg.msg_iovlen = 1;
    msg.msg_control = cbuffer;
    msg.msg_controllen = BUF_SIZE;
    msg.msg_flags = 0;

    struct msghdr msg2 = {};
    memset(&msg2, 0, sizeof(msg2));
    msg2.msg_name = &udp.addr;
    msg2.msg_namelen = udp.size;
    msg2.msg_iov = &io_vec2;
    msg2.msg_iovlen = 1;
    msg2.msg_control = cbuffer2;
    msg2.msg_controllen = BUF_SIZE;
    msg2.msg_flags = 0;


    if(FD_ISSET(udp.socket, &rset))
    {
      //rx1 = recvfrom(udp.socket, buffer, BUF_SIZE, 0, (struct sockaddr *)&udp.addr, &udp.size);
      rx1 = recvmsg(udp.socket, &msg, 0);
      //printf("%d\n", rx1);
      //rx2 = recvfrom(udp.socket, buffer, BUF_SIZE, 0, (struct sockaddr *)&udp.addr, &udp.size);
      rx2 = recvmsg(udp.socket, &msg2, 0);
      //printf("%d\n", rx2);

      struct cmsghdr *cmsg;
      for(cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg))
      {
        if(cmsg->cmsg_level == SOL_SOCKET)
        {
          if(cmsg->cmsg_type == SO_TIMESTAMP)
          {
            //printf("yo1");
            memcpy(&t1, CMSG_DATA(cmsg), sizeof(t1));
          }
        }
      }
      struct cmsghdr *cmsg2;
      for(cmsg2 = CMSG_FIRSTHDR(&msg2); cmsg2 != NULL; cmsg2 = CMSG_NXTHDR(&msg2, cmsg2))
      {
        if(cmsg2->cmsg_level == SOL_SOCKET)
        {
          if(cmsg2->cmsg_type == SO_TIMESTAMP)
          {
            //printf("yo2");
            memcpy(&t2, CMSG_DATA(cmsg2), sizeof(t2));
          }
        }
      }
      uint64_t stamp0, stamp1;
      stamp0 = t1.tv_sec * m + t1.tv_usec;
      stamp1 = t2.tv_sec * m + t2.tv_usec;
      uint64_t diff = stamp1 - stamp0;

      memset(back, 0, sizeof(back));
      memcpy(back, (char*)&diff, sizeof(uint64_t));

      //printf("bluuuu\n");
      send(client, back, sizeof(back), 0);
      //printf("bluuuub %d\n", i++);
    }
    

    if(FD_ISSET(client, &rset))
    {
      //printf("dropped\n");
      recv(client, back, sizeof(back), 0);
      break;
    }
  }

  printf("finished\n");

  //printf("%d bytes sent, %d bytes send second packet\n", rx1, rx2);
  //printf("difference between the packets was %"PRIu64" microseconds\n", diff);
}


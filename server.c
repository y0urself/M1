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
#include "performance.h"

void ploss(struct conn udp, struct conn tcp, int client);
void bwidth(struct conn udp, struct conn tcp, int client);


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
  tcp_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
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
  udp_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
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
  if(bind(udp.socket, (struct sockaddr*)&udp.addr, udp.size) < 0)
  {
    error("ERROR on binding");
  }

  if(listen(tcp.socket, 1) < 0)
  {
    error("ERROR on listen");
  }

  tcp_client_fd = accept(tcp.socket, (struct sockaddr_in*)&tcp.addr, &tcp.size);

  read(tcp_client_fd, buffer, sizeof(buffer));
  printf("%s\n", buffer);
  if(strcmp(buffer, "ploss") == 0)
  {
    mode = 2;
  }
  else if(strcmp(buffer, "bwidth") == 0)
  {
    mode = 3;
  }

  printf("TCP established\n");

  write(tcp_client_fd, buffer, sizeof(buffer));

  if(mode == 3)
  {
    bwidth(udp, tcp, tcp_client_fd);
  }
  else if(mode == 2)
  {
    ploss(udp, tcp, tcp_client_fd);
    // fd_set rset;
    // fd_set wset;

    // max_fd = max(tcp_client_fd, udp.socket) + 1;
    // while(42)
    // {
    //   FD_SET(tcp_client_fd, &rset);
    //   FD_SET(udp_server_fd, &rset);

    //   ready_fd = select(max_fd, &rset, NULL, NULL, &t_val);

    //   //printf("%d\n", i);

    //   if(ready_fd < 0)
    //   {
    //     error("Error in select?");
    //   }
    //   else
    //   {
    //     if(FD_ISSET(tcp_client_fd, &rset))
    //     {
    //       printf("hello %d\n", i);

    //       read(tcp_client_fd, buffer, sizeof(buffer));

    //       memcpy(buffer, "meeh\n", 6);

    //       printf("TCP closing\n");

    //       write(tcp_client_fd, buffer, sizeof(buffer));

    //       break;
    //     }

    //     if(FD_ISSET(udp_server_fd, &rset))
    //     {
    //       len = recvfrom(udp_server_fd, buffer, BUF_SIZE, 0, (struct sockaddr *)&udp_client_address, &udp_client_len);
    //       if (len < 0)
    //       {
    //         error("ERROR in recvfrom");
    //       }

    //       /*
    //       * gethostbyaddr: determine who sent the datagram
    //       */
    //       // udp_client = gethostbyaddr((const char *)&udp_client_address.sin_addr.s_addr, sizeof(udp_client_address.sin_addr.s_addr), AF_INET);
    //       // if (udp_client == NULL)
    //       // {
    //       //   error("ERROR on gethostbyaddr");
    //       // }
    //       // host_address_pointer = inet_ntoa(udp_client_address.sin_addr);
    //       // if (host_address_pointer == NULL)
    //       // {
    //       //   error("ERROR on inet_ntoa\n");
    //       // }

    //       // TODO Change the prints :D
    //       //printf("server received datagram from %s (%s)\n", udp_client->h_name, host_address_pointer);

    //       // TODO Change the prints :D
    //       //printf("server received %lu/%d bytes: %s\n", strlen(buffer), len, buffer);

    //       len = sendto(udp_server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&udp_client_address, udp_client_len);
    //       if (len < 0)
    //       {
    //         error("ERROR in sendto");
    //       }

    //       i++;
    //       //printf("%d \n", i++);

    //     }

    //   }
    // }
  }
  close(udp_server_fd);
  close(tcp_server_fd);
}

void ploss(struct conn udp, struct conn tcp, int client)
{
  char* buffer[BUF_SIZE];
  int max_fd, ready_fd;
  int i = 0;
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

      read(client, buffer, sizeof(buffer));

      memcpy(buffer, "meeh\n", 6);

      printf("TCP closing\n");

      write(client, buffer, sizeof(buffer));

      break;
    }

    if(FD_ISSET(udp.socket, &rset))
    {
      rx = recvfrom(udp.socket, buffer, BUF_SIZE, 0, (struct sockaddr *)&udp.addr, &udp.size);
      if (rx < 0)
      {
        error("ERROR in recvfrom");
      }

      // tx = sendto(udp.socket, buffer, strlen(buffer), 0, (struct sockaddr *)&udp.addr, udp.size);
      // if (tx < 0)
      // {
      //   error("ERROR in sendto");
      // }

      i++;
      //printf("%d \n", i++);
      }
    }
  }
}

void bwidth(struct conn udp, struct conn tcp, int client)
{
  printf("bwidth\n");
  char buffer[BUF_SIZE];
  memset(buffer, 0x00, sizeof(buffer));

  fd_set rset;
  int ready_fd;
  int i = 0;
  int rx;

  struct timeval t_val;
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;

  int max_fd = max(client, udp.socket) + 1;
  //FD_ZERO(&rset);
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

        read(client, buffer, sizeof(buffer));

        memcpy(buffer, (char*)&i, sizeof(int));

        printf("TCP closing\n");

        write(client, buffer, sizeof(buffer));

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
        printf("%s, %d\n", buffer, i);
      }
      // FD_ZERO(&rset);
      // usleep(10); 
    }
  }
}

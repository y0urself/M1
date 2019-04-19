#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>

#include <inttypes.h>
#include <unistd.h>
#include <time.h>

#include "performance.h"

void rtt(struct conn udp);
void ploss(struct conn udp, struct conn tcp);
void bwidth(struct conn udp, struct conn tcp);
void bneck(struct conn udp);

void usage()
{
  printf("./client <mode> <host> <port>\nmodes: rtt, ploss, bwidth, bneck\n");
}

int main(int argc, char** argv)
{
  int mode = 0;
  char buffer[BUF_SIZE];

  memset(buffer, 0x00, sizeof(buffer));

  if(argc < 4)
  {
    usage();
    return 0;
  }
  if(strcmp(argv[1], "rtt") == 0)
  {
    mode = 1;
    memcpy(buffer, "rtt", 4);
  }
  else if(strcmp(argv[1], "ploss") == 0)
  {
    mode = 2;
    memcpy(buffer, "ploss", 6);
  }
  else if(strcmp(argv[1], "bwidth") == 0)
  {
    mode = 3;
    memcpy(buffer, "bwidth", 7);
  }
  else if(strcmp(argv[1], "bneck") == 0)
  {
    mode = 4;
    memcpy(buffer, "bneck", 6);
  }

  if(mode == 0)
  {
    usage();
    return 0;
  }

  int ready_fd, max_fd;

  struct timeval t_val; /* timeval for select() */
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;

  char *hostname;
  hostname = argv[2];

  /* TCP SOCKET PROPERTIES */
  int tcp_socket_fd = 0;
  int port;
  int tcp_len;

  struct cmsghdr cmsg;

  cmsg.cmsg_type = SO_TIMESTAMP;

  struct sockaddr_in tcp_server_address;
  struct hostent *tcp_server;

  char *e;
  port = strtol(argv[3], &e, 10);
  if(port < 0 || port > 65535)
  {
    printf("Port should be between 0 and 65535.\n");
    usage();
    return 0;
  }
  printf("%d\n", port);

  memset(&tcp_server_address, 0x00, sizeof(tcp_server_address));

  tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (tcp_socket_fd < 0)
  {
    error("ERROR opening socket");
  }

  // TODO: Get Host by IP/address
  /* gethostbyname: get the server's DNS entry */
  tcp_server = gethostbyname(hostname);
  if (tcp_server == NULL) 
  {
    // TODO Change the prints :D
    fprintf(stderr,"ERROR, no such host as %s\n", hostname);
    exit(0);
  }

  /* UDP SOCKET PROPERTIES */
  int udp_socket_fd = 0;

  fd_set rset;

  struct sockaddr_in udp_server_address;
  struct hostent *udp_server;

  memset(&udp_server_address, 0x00, sizeof(udp_server_address));
  //memset(buffer, 0x00, sizeof(buffer));

  udp_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (udp_socket_fd < 0)
  {
    error("ERROR opening socket");
  }

  // TODO: Get Host by IP/address
  udp_server = gethostbyname(hostname);
  if (udp_server == NULL)
  {
    // TODO Change the prints :D
    fprintf(stderr,"ERROR, no such host as %s\n", hostname);
    exit(0);
  }

  struct conn udp;
  udp.socket = udp_socket_fd;
  udp.addr = udp_server_address;
  udp.size = sizeof(udp_server_address);

  struct conn tcp;
  tcp.socket = tcp_socket_fd;
  tcp.addr = tcp_server_address;
  tcp.size = sizeof(tcp_server_address);

  udp.addr.sin_family = AF_INET;
  memcpy((char *)&udp.addr.sin_addr.s_addr, (char *)udp_server->h_addr, udp_server->h_length);
  udp.addr.sin_port = htons(port);

  tcp.addr.sin_family = AF_INET;
  memcpy((char *)&tcp.addr.sin_addr.s_addr, (char *)tcp_server->h_addr, tcp_server->h_length);
  tcp.addr.sin_port = htons(port);

  int timeopt = 1;
  setsockopt(udp.socket, SOL_SOCKET, SO_TIMESTAMP, (const void *)&timeopt , sizeof(timeopt));

  printf("Connecting via TCP\n");

  if(connect(tcp.socket, (struct sockaddr*)&tcp.addr, tcp.size) < 0)
  { 
      printf("connection with the server failed...\n"); 
  } 

  send(tcp.socket, buffer, sizeof(buffer), 0);

  recv(tcp.socket, buffer, sizeof(buffer), 0);

  printf("%s\n", buffer);

  printf("TCP established\n");

  uint64_t time = 0;
  int bytes = 0;

  if(mode == 1)
  {
    rtt(udp);
  }
  else if(mode == 2)
  {
    ploss(udp, tcp);
  }
  else if(mode == 3)
  {
    bwidth(udp, tcp);
  }
  else if(mode == 4)
  {
    bneck(udp);
  }
  
  //printf("TCP me again\n");

  send(tcp.socket, buffer, sizeof(buffer), 0);

  recv(tcp.socket, buffer, sizeof(buffer), 0);

  if(mode == 3)
  {
    //int i = 0;
    //memcpy((char*)&i, buffer, sizeof(int));
    //printf("Packets received: %d, with size of %d byte in %"PRIu64" microsecs\n", i, bytes, time);

    //double throughput = (i * bytes) * ((double)1000000 / time);
    //printf("Throughput: %f bytes/s\n", throughput);
  }

  //printf("closing sockets.\n");

  close(tcp.socket);
  close(udp.socket);

  return 0;
}

void rtt(struct conn udp)
{
  uint64_t times[RTT_RUNS];
  uint64_t median, middle, min, max;
  uint64_t sum = 0;

  // union
  // {
  //   struct cmsghdr cm;
  //   char control[CMSG_SPACE(sizeof(struct timeval))];
  // }
  // cmsg_union;

  // memset(&cmsg_union, 0, sizeof(cmsg_union));

  char message[] = "Der Satz hatte zu viele Silben, entschuldige dich!";
  char buf[] = "Ja ich weiß, es tut mir wirklich schrecklich leid!";
  int len = strlen(message);
  int i = 0;
  int n = RTT_RUNS;
  uint64_t m = 1000000;

  int rx, tx;

  while(i < n)
  {
    struct iovec io_vec[1] = {{message, len}};

    unsigned char cbuf[45] = {0};
    int clen = sizeof(cbuf);

    /* MSG HEADER */
    struct msghdr msg = {};
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = &udp.addr;
    msg.msg_namelen = udp.size;
    msg.msg_iov = io_vec;
    msg.msg_iovlen = 1;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;

    struct timeval tvsend, tvrecv;
    if (gettimeofday(&tvsend, NULL) < 0)
    {
      error("gettimeofday");
      exit(1);
    }
    tx = sendmsg(udp.socket, &msg, 0);
    if (tx < 0) 
    {
      error("ERROR in sendmsg");
    }

    io_vec[0].iov_base = buf;
    msg.msg_control = cbuf;
    msg.msg_controllen = clen;

    rx = recvmsg(udp.socket, &msg, 0);
    if (rx < 0)
    {
      error("ERROR in recvmsg");
    }

    tvrecv.tv_sec = 0;
    tvrecv.tv_usec = 0;
    struct cmsghdr *cmsg;
    for(cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg))
    {
      if(cmsg->cmsg_level == SOL_SOCKET)
      {
        //printf("%d\n", cmsg->cmsg_type);
        if(cmsg->cmsg_type == SO_TIMESTAMP)
        {
          memcpy(&tvrecv, CMSG_DATA(cmsg), sizeof(tvrecv));
        }
      }
    }
    //printf("SO_TIMESTAMP %ld.%06ld \n", (long)tvrecv.tv_sec, (long)tvrecv.tv_usec);

    uint64_t stamp0, stamp1;
    stamp0 = tvsend.tv_sec * m + tvsend.tv_usec;
    stamp1 = tvrecv.tv_sec * m + tvrecv.tv_usec;
    uint64_t rttime = stamp1 - stamp0;
    times[i] = rttime;
    sum = sum + rttime;

    printf("%d bytes sent, %d bytes received\n", tx, rx);
    printf("round-trip time was %"PRIu64" microseconds\n", rttime);

    i++;
    printf("%d\n", i);
  }
  middle = sum / n;
  median = times[100];
  min = times[0];
  max = times[199];

  printf("in this test %d bytes were sent, %d bytes were received\n", tx, rx);
  printf("number of runs: %d\n", n);
  printf("minimum:    %"PRIu64"\n", min);
  printf("maximum:    %"PRIu64"\n", max);
  printf("average:    %"PRIu64"\n", middle);
  printf("median:     %"PRIu64"\n", median);
}

void ploss(struct conn udp, struct conn tcp)
{
// TODO: Both connections? TX / RX?
  int rx, tx;
  int n = TEST_RUNS;

  int ready_fd, max_fd;
  fd_set rset;
  char buffer[BUF_SIZE];
  memset(buffer, 0x00, sizeof(buffer));

  struct timeval t_val; /* timeval for select() */
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;


  strcpy(buffer, "Der Satz hatte zu viele Silben, entschuldige dich!");
  //strcpy(buffer, "Ja ich weiß, es tut mir wirklich schrecklich leid!");
  // LOOPOOOOP
  int i = 0;

  FD_ZERO(&rset);

  while(i < n)
  { 
    tx = sendto(udp.socket, buffer, strlen(buffer), 0, (struct sockaddr *)&udp.addr, udp.size);
    if (tx < 0) 
    {
      error("ERROR in sendto");
    }  
    i++;
    //printf("%d \n", i);
    usleep(10);
  }

  char back[5] = "back";

  send(tcp.socket, back, sizeof(back), 0);
  recv(tcp.socket, back, sizeof(back), 0);

  int k = 0;
  memcpy((char*)&k, back, sizeof(int));
  printf("Server received: %d/%d packets. Thats a loss of %f\n", k, n, (double)((n - k) / (double)n));

  max_fd = max(tcp.socket, udp.socket) + 1;
  i = 0;

  FD_ZERO(&rset);
  
  while(42)
  {
    FD_SET(tcp.socket, &rset);
    FD_SET(udp.socket, &rset);

    ready_fd = select(max_fd, &rset, NULL, NULL, &t_val);

    if(ready_fd < 0)
    {
      error("Error in select?");
    }
    else
    {
      if(FD_ISSET(tcp.socket, &rset))
      {
        recv(tcp.socket, back, sizeof(back), 0);
        if(strcmp(back, "end.") == 0)
        {
          strcpy(back, "isok");
          send(tcp.socket, back, sizeof(back), 0);
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
        //printf("%d \n", i++);
      }
    }
  }

  printf("Client received: %d/%d packets. Thats a loss of %f\n", i, n, (double)((n - i) / (double)n));
}

void bwidth(struct conn udp, struct conn tcp)
{
  int bytes;
  uint64_t time;
  struct timeval before;
  before.tv_sec = 0;
  before.tv_usec = 0;

  struct timeval after;
  after.tv_sec = 0;
  after.tv_usec = 0;

  int back[9];
  memset(back, 0, sizeof(back));

  int n = TEST_RUNS;
  int max_fd;
  int ready_fd;
  fd_set rset;

  int rx = 0;

  char buffer[BUF_SIZE];

  strcpy(buffer, "111111111Lo111111111Loremrem111111111Lorem111111111Lorem111111111Lorem111111111Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum.\n");

  bytes = strlen(buffer) + 1 + UDP_OVERHEAD;

  // LOOPOOOOP
  int i = 0;
  int k = 0;

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

  //printf("%d, %d, ..., %d, %d\n", before.tv_sec, before.tv_usec, after.tv_sec, after.tv_usec);

  uint64_t m = 1000000;
  uint64_t  stamp0, stamp1;
  stamp0 = before.tv_sec * m + before.tv_usec;
  stamp1 = after.tv_sec * m + after.tv_usec;
  time = stamp1 - stamp0;
  //printf("%"PRIu64"\n", time);

  send(tcp.socket, back, sizeof(buffer), 0);

  recv(tcp.socket, back, sizeof(buffer), 0);

  memcpy((char*)&i, back, sizeof(int));
  printf("Server received: %d, with size of %d byte in %"PRIu64" microsecs\n", i, bytes, time);

  double throughput = (i * bytes) * ((double)1000000 / time);
  printf("Throughput Upload: %f bytes/s\n", throughput);

  struct timeval t_val;
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;

  i = 0;

  max_fd = max(udp.socket, tcp.socket) + 1;

  while(42)
  {
    FD_SET(tcp.socket, &rset);
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
      if(FD_ISSET(tcp.socket, &rset))
      {
        //printf("hello %d\n", i);

        recv(tcp.socket, back, sizeof(back), 0);

        memcpy((char*)&time, back, sizeof(uint64_t));

        //printf("TCP closing\n");

        send(tcp.socket, back, sizeof(back), 0);
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

  printf("Client received: %d, with size of %d byte in %"PRIu64" microsecs\n", i, bytes, time);

  throughput = (i * bytes) * ((double)1000000 / time);
  printf("Throughput Download: %f bytes/s\n", throughput);
  
}

void bneck(struct conn udp)
{
  int tx1, tx2;
  char buffer[BUF_SIZE];
  strcpy(buffer, "Lo111111111Loremrem111111111Lorem111111111Lorem111111111Lorem111111111Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum.\n");

  tx1 = sendto(udp.socket, buffer, strlen(buffer), 0, (struct sockaddr *)&udp.addr, udp.size);
  tx2 = sendto(udp.socket, buffer, strlen(buffer), 0, (struct sockaddr *)&udp.addr, udp.size);

  printf("sizes send: %d, %d", tx1, tx2);
}
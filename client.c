#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <arpa/inet.h>
#include <sys/select.h>

#include <inttypes.h>

#include <time.h>

#include "performance.h"

void rtt(struct conn udp);
void ploss(struct conn udp, struct conn tcp);
void bwidth(struct conn udp, u_int64_t *time, int *bytes);

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
  //struct msghdr *msg;

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
  int udp_len;


  fd_set rset;

  struct sockaddr_in udp_server_address;
  struct hostent *udp_server;

  socklen_t udp_server_len;

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



  // struct timeval	myto = { 5, 0 };
	// int		tlen = sizeof( myto );
	// if ( setsockopt( udp_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &myto, tlen ) < 0 )
	// 	{ perror( "setsockopt RCVTIMEO" ); exit(1);  }

  int timeopt = 1;
  setsockopt(udp.socket, SOL_SOCKET, SO_TIMESTAMP, (const void *)&timeopt , sizeof(timeopt));

	char	message[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890 =";
	char	buf[]     = "----------------------------------------";
	int	len = strlen(message);

	struct iovec io_vec[1] = { { message, len } };

  unsigned char	cbuf[ 40 ] = { 0 };
	int		clen = sizeof( cbuf );

  /* MSG HEADER */

  struct msghdr	msg = { 0 };
	msg.msg_name	= &udp_server_address;
	msg.msg_namelen	= udp_server_len;
	msg.msg_iov	= io_vec;
	msg.msg_iovlen	= 1;
	msg.msg_control	= NULL;
	msg.msg_controllen	= 0;
	msg.msg_flags	= 0;

  printf("Connecting via TCP\n");

  if(connect(tcp.socket, (struct sockaddr*)&tcp.addr, tcp.size) < 0)
  { 
      printf("connection with the server failed...\n"); 
  } 

  write(tcp.socket, buffer, sizeof(buffer));

  read(tcp.socket, buffer, sizeof(buffer));

  printf("%s\n", buffer);

  printf("TCP established\n");

  u_int64_t time = 0;
  int bytes = 0;

  if(mode == 1)
  {
    rtt(udp);
  }
  else if(mode == 2)
  {

  }
  else if(mode == 3)
  {
    bwidth(udp, &time, &bytes);
  }
  
  printf("TCP me again\n");

  write(tcp_socket_fd, buffer, sizeof(buffer));

  read(tcp_socket_fd, buffer, sizeof(buffer));

  if(mode == 3)
  {
    int i = 0;
    memcpy((char*)&i, buffer, sizeof(int));
    printf("Packets received: %d, with size of %d byte in %"PRIu64" microsecs\n", i, bytes, time);

    double throughput = (i * bytes) * ((double)1000000 / time);
    printf("Throughput: %f bytes/s\n", throughput);
  }


  printf("closing\n");

  close(tcp_socket_fd);

  //printf("Echo from server: %s", buffer);
  return 0;
}

void rtt(struct conn udp)
{
  union
  {
    struct cmsghdr cm;
    char control[CMSG_SPACE(sizeof(struct timeval))];
  }
  cmsg_union;

  memset(&cmsg_union, 0, sizeof(cmsg_union));

  char message[] = "....1....1....1";
  char buf[]     = "--------------------------------------------";
  int len = strlen(message);

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

  int rx, tx;

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
      printf("%d\n", cmsg->cmsg_type);
        if(cmsg->cmsg_type == SO_TIMESTAMP)
        {
          printf("yo\n");
          memcpy(&tvrecv, CMSG_DATA(cmsg), sizeof(tvrecv));
        }
    }
  }

  printf("SO_TIMESTAMP %ld.%06ld \n", (long)tvrecv.tv_sec, (long)tvrecv.tv_usec);

  unsigned long long stamp0, stamp1;
  stamp0 = tvsend.tv_sec * 1000000LL + tvsend.tv_usec;
  stamp1 = tvrecv.tv_sec * 1000000LL + tvrecv.tv_usec;
  unsigned long long rtt = stamp1 - stamp0;

  printf("%d bytes sent, %d bytes received \n", tx, rx);
  printf("round-trip time was %llu microseconds \n\n", rtt);
}

void ploss(struct conn udp, struct conn tcp,)
{
// TODO: Both connections? TX / RX?
  int rx, tx;

  int ready_fd;
  fd_set rset;
  char buffer[BUF_SIZE];
  memset(buffer, 0x00, sizeof(buffer));

  struct timeval t_val; /* timeval for select() */
  t_val.tv_sec = 0;
  t_val.tv_usec = 0;


  if(strcmp(buffer, "ploss") == 0)
  {
    strcpy(buffer, "Der Satz hatte zu viele Silben, entschuldige dich!");
    strcpy(buffer, "Ja ich weiß, es tut mir wirklich schrecklich leid!");
    // LOOPOOOOP
    int i = 0;

    FD_ZERO(&rset);

    while(i < 10000)
    {
      // FD_SET(udp.socket, &rset);

      // ready_fd = select(udp.socket + 1, &rset, NULL, NULL, &t_val); 

      tx = sendto(udp.socket, buffer, strlen(buffer), 0, &udp.addr, udp.size);
      if (tx < 0) 
      {
        error("ERROR in sendto");
      }  

      // if(FD_ISSET(udp.socket, &rset))
      // {
      //   rx = recvfrom(udp.socket, buffer, strlen(buffer), 0, &udp.addr, &udp.size);
      //   if (rx < 0) 
      //   {
      //     error("ERROR in recvfrom");
      //   }
      // }
      i++;
      printf("%d \n", i);
    }

    write(tcp_socket_fd, buffer, sizeof(buffer));

    read(tcp_socket_fd, buffer, sizeof(buffer));
  }
}

void bwidth(struct conn udp, u_int64_t* time, int* bytes)
{
  struct timeval before; /* timeval for select() */
  before.tv_sec = 0;
  before.tv_usec = 0;

  struct timeval after; /* timeval for select() */
  after.tv_sec = 0;
  after.tv_usec = 0;

  int max_fd;
  int ready_fd;
  fd_set rset;
  fd_set wset;

  int rx = 0;

  char buffer[BUF_SIZE];

  strcpy(buffer, "Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum. Lorem ipsum.\n");

  *bytes = strlen(buffer) + 1 + UDP_OVERHEAD;

  // LOOPOOOOP
  int i = 0;
  int k = 0;

  gettimeofday(&before, NULL);
  while(i < 100)
  { 
    // FD_ZERO(&rset);
    // FD_SET(udp.socket, &rset);

    // if(FD_ISSET(udp.socket, &rset))
    // {
      rx = sendto(udp.socket, buffer, strlen(buffer), 0, &udp.addr, udp.size);
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

  printf("%d, %d, ..., %d, %d\n", before.tv_sec, before.tv_usec, after.tv_sec, after.tv_usec);

  u_int64_t m = 1000000;
  u_int64_t  stamp0, stamp1;
  stamp0 = before.tv_sec * m + before.tv_usec;
  stamp1 = after.tv_sec * m + after.tv_usec;
  *time = stamp1 - stamp0;
  printf("%"PRIu64"\n", *time);
}
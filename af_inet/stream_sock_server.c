#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>


#define BUFFER_LEN 250
#define SERVER_PORT 50012

void main (int argc, void **argv)
{
   int sock = -1 , sock_accept = -1, rc, length;
   struct sockaddr_in addr;
   char on = 1;
   char buffer[BUFFER_LEN];
   struct pollfd fds;
    nfds_t nfds = 1;
   int timeout;
   do 
   {
      sock = socket(AF_INET,SOCK_STREAM,0);
      if(sock < 0)
      {
         perror("Socket Failed\n");
         break;
      }

      rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
      if (rc < 0)
      {
         perror("setsockopt() API failed\n");
         break;
      }

      memset(&addr, 0, sizeof(addr));
      addr.sin_family = AF_INET;
      addr.sin_port = htons(SERVER_PORT);
      inet_aton ("10.10.10.1", &addr.sin_addr);

      rc = bind (sock, (struct sockaddr*) &addr, sizeof(addr));
      if (rc < 0)
      {
         perror("bind() failed");
         break;
      }

      rc = listen (sock, 10);
      if (rc < 0)
      {
         perror("listen() failed");
         break;
      }

      printf("Ready for client\n");

      sock_accept = accept (sock, NULL, NULL);
      if (sock_accept < 0)
      {
         perror("listen() failed");
         break;
      }

      timeout = 30000;
      memset (&fds, 0 ,sizeof(fds));
      fds.fd = sock_accept;
      fds.events = POLLIN;
      fds.revents = 0;

      rc = poll (&fds,nfds,timeout);
      if (rc < 0)
      {
         perror("poll() failed");
         break;
      }

      if (rc == 0)
      {
         perror("poll timeou");
         break;
      }

      length = BUFFER_LEN;
      rc = setsockopt (sock_accept, SOL_SOCKET, SO_RCVLOWAT, (char *) &length, sizeof(length));

      if (rc < 0)
      {
         perror("setsockopt failed");
         break;
      }

      rc = recv (sock_accept, buffer, sizeof(buffer), 0);
      printf("%d bytes of data were received\n", rc);
      if (rc == 0 ||  rc < sizeof(buffer))
      {
         printf("The client closed the connection before all of the\n");
         printf("data was sent\n");
         break;
      }

      rc = send(sock_accept, buffer, sizeof(buffer), 0);
      if (rc < 0)
      {
         perror("send() failed");
         break;
      }
   } while (0);

   if (sock != -1)
      close(sock);

   if (sock_accept != -1)
      close(sock_accept);
   return;
}

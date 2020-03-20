#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>

#define BUFFER_LENGTH 250
#define SERVER_PATH "/tmp/server"
void main (int argc, char *argv[])
{
   int sock = -1, sock_accept = -1;
   int rc = 0, length;
   char buffer[BUFFER_LENGTH];
   struct sockaddr_un svr_addr;
   do
   {
     sock = socket(AF_UNIX, SOCK_STREAM, 0);
     if (sock < 0)
     {
        perror("socket() failed\n");
        break;
     }

     memset(&svr_addr, 0, sizeof(svr_addr));
     svr_addr.sun_family = AF_UNIX;
     strcpy(svr_addr.sun_path, SERVER_PATH);
     rc = bind (sock, (struct sockaddr*) &svr_addr, sizeof(svr_addr));
     if (rc < 0)
     {
       perror("bind() failed");
       break;
     }

     rc = listen (sock, 10);
     if (rc < 0)
     {
        perror("listen() failed\n");
        break;
     }

     sock_accept = accept (sock, NULL, NULL);
     if (sock_accept < 0)
     {
        perror("accept() failed\n");
        break;
     }
     
     length = BUFFER_LENGTH;
     rc = setsockopt(sock_accept, SOL_SOCKET, SO_RCVLOWAT, (char *)&length, sizeof(length));
     if (rc < 0)
     {
       perror("setsockopt() failed\n");
       break;
     }

     rc = recv(sock_accept, buffer, sizeof(buffer), 0);
     if (rc < 0)
     {
       perror("recv() failed\n");
       break;
     }

     printf("%d bytes received from client\n",rc);
   } while(0);

   if (sock != -1)
      close(sock);

   if (sock_accept != -1)
      close(sock);
   
   return;
}

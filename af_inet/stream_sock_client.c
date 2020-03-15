#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define SERVER_PORT 50011
#define BUFFER_LEN 250
#define SERVER_ADDRESS "10.10.10.1"

void main (int argc, const char *argv[])
{
   struct sockaddr_in addr;
   char buffer[BUFFER_LEN];
   char server[255];
   int sock = -1, rc, bytesReceived;

   do
   {
      sock = socket (AF_INET, SOCK_STREAM, 0);
      if (sock < 0)
      {
         perror("socket() API failure\n");
         break;
      }
      
      if (argc > 1)
        strcpy(server, argv[1]);
      else
        strcpy(server, SERVER_ADDRESS);
      
      memset(&addr,0,sizeof(addr));
      addr.sin_family = AF_INET;
      addr.sin_port = htons (SERVER_PORT);
      inet_aton(server, &addr.sin_addr);

      rc = connect (sock, (struct sockaddr*) &addr, sizeof(addr));
      if (rc < 0)
      {
         perror ("connect API failed\n");
         break;
      }

      memset (buffer, 'a', sizeof(buffer));
      rc = send (sock, buffer, sizeof(buffer), 0);
      if (rc < 0)
      {
         perror ("send() API failed\n");
         break;
      }
      bytesReceived = 0;
      while (bytesReceived < BUFFER_LEN)
      {
         rc = recv(sock, &buffer[bytesReceived],BUFFER_LEN-bytesReceived, 0);
         if (rc < 0)
         {
            perror("recv() failed");
            break;
         }
         else if (rc == 0)
         {
            printf("Server closed the connection \n");
            break;
         }
         bytesReceived +=rc;
      }
   }while (0);

   if (sock != -1)
   {
      close(sock);
   }
   return;
}

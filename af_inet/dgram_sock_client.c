#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define SERVER_PORT 3555
#define SERVER_ADDR "10.10.10.1"
void main (int argc, char *argv[])
{
   int sock = -1, rc = 0; 
   char server[100];
   char buffer[100];
   struct sockaddr_in server_addr;
   int server_addr_len = sizeof(server_addr);;
   do
   {
      sock = socket (AF_INET, SOCK_DGRAM, 0);
      if (sock < 0)
      {
         perror ("socket() failed");
         break;
      }

      if (argc > 1)
      {
         strcpy(server, argv[1]);
      }
      else
      {
         strcpy (server, SERVER_ADDR);
      }

      memset (&server_addr, 0, sizeof(server_addr));
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(SERVER_PORT);
      inet_aton (server, &server_addr.sin_addr);

      memset(buffer, 0, sizeof(buffer));
      strcpy(buffer,"Clinet Request");

      rc = sendto (sock, buffer, sizeof(buffer), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
      if (rc < 0)
      {
         perror("sendto() failed");
         break;
      }

      rc = recvfrom (sock, buffer, sizeof(buffer), 0, (struct sockaddr*) &server_addr, &server_addr_len);
      if (rc < 0)
      {
         perror("sendto() failed");
         break;
      }

      inet_ntop (AF_INET, &server_addr.sin_addr.s_addr, server, sizeof(buffer));
      printf ("Received data from server: %s on port %d\n", server, ntohs(server_addr.sin_port));
      printf("================= DATA ================\n");
      printf("%s\n",buffer);
   }while(0);
   
   if (sock != -1)
   {
      close(sock);
   }
   return;
}


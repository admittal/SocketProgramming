#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LEN 100
#define DEFAULT_ADDRES "0.0.0.0"
#define SERVER_PORT 3555
void main (int argc, char *argv[])
{
   int sock = -1, rc = 0, addr_len_client = 0;
   char buffer[BUFFER_LEN], server[100], client_name[100];
   struct sockaddr_in addr, addr_client;

   do
   {
     sock = socket (AF_INET, SOCK_DGRAM, 0);
     if (sock < 0)
     {
        perror("Socket() failed\n");
        break;
     }

     memset(&addr, 0, sizeof(addr));
     addr.sin_family = AF_INET;
     addr.sin_port = htons(SERVER_PORT);
     
     if (argc > 1)
     {
        strcpy(server,argv[1]);
     }     
     else
     {
        strcpy(server,DEFAULT_ADDRES);
     }
     inet_aton (server,&addr.sin_addr);

     rc = bind (sock, (struct sockaddr*) &addr, sizeof(addr));
     if (rc < 0)
     {
        perror("bind() failed\n");
        break;
     }

     memset(&addr, 0, sizeof(addr_client));
     rc = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*) &addr_client, &addr_len_client);
     if (rc < 0)
     {
        perror("recvfrom() failed");
        break;
     }

     inet_ntop (AF_INET, &addr_client.sin_addr.s_addr, client_name, sizeof(client_name));
     printf("Received Data from client %s on port %d\n", client_name, ntohs(addr_client.sin_port)); 
     printf("============= DATA ==================\n");
     printf("%s\n", buffer);

     rc = sendto (sock, buffer, sizeof(buffer), 0, (struct sockaddr*) &addr_client, sizeof(addr_client));
     if (rc < 0)
     {
       perror ("sendto() failed\n");
       break;
     }
   } while (0);

   if (sock != -1)
      close (sock);

   return;
}

#include <sys/types.h>
#include <sys/socket.h>

void main (int argc, void **argv)
{
   int fd_s;
   struct sockaddr_in sin_addr;

   fd_s = socket(AF_INET,SOCK_STREAM,0);
   
   if(fd_s < 0)
   {
      P_ERROR ("Socket Failed\n");
      return;
   }

   memset(&sin_addr,0,sizeof(sockaddr_in));
}

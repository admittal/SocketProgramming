#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <poll.h>

void oam_debug_print_frame(u_char *pFrameContent, uint32_t frameSize)
{
#define OCTETS_PER_LINE 16

   char    paystr[(OCTETS_PER_LINE * 3) + 1];
   uint32_t  i;
   uint32_t  strloc = 0;

   for (i = 0; i < frameSize; i++)
   {
      strloc += snprintf(&paystr[strloc], 4, "%02X ", pFrameContent[i]);

      if (((i + 1) % OCTETS_PER_LINE) == 0)
      {
         printf("%s\n",paystr);
         strloc = 0;
      }
   }

   if (((i + 1) % OCTETS_PER_LINE) > 0)
   {
      printf("%s\n",paystr);
   }
}

int main (void)
{
   char buffer[65537];
   struct ifreq ifr;
   struct sockaddr_ll addr;
   struct socklen_t *addr_len = NULL;
   const char *if_name = {"eth0"};
   int ifindex;
   struct pollfd fds;
   nfds_t nfds = 1;
   int rc =0;
   int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
   if (fd == -1) {
      perror("socket");
      exit(1);
   }
   printf("fd=%d\n",fd);

   size_t if_name_len = strlen(if_name);
   if (if_name_len < sizeof(ifr.ifr_name)) {
      memcpy(ifr.ifr_name, if_name, if_name_len);
      ifr.ifr_name[if_name_len] = 0;
   } else {
      fprintf(stderr, "interface name is too long\n");
      exit(1);
   }
   if (ioctl(fd,SIOCGIFINDEX, &ifr) == -1) {
      perror("ioctl");
      exit(1);
   }
   ifindex=ifr.ifr_ifindex;
   printf("ifindex = %d\n",ifindex);
   memset(&addr,0,sizeof(addr));
   addr.sll_family = AF_PACKET;
   addr.sll_ifindex = ifindex;
   addr.sll_protocol = htons(ETH_P_ALL);
   if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      perror("bind");
      exit(1);
   }
   memset(&fds, 0, sizeof(fds));
   fds.fd =fd;
   fds.events = POLLIN;
   fds.revents = 0;
   rc = poll(&fds, nfds, 30000);
   if (rc < 0)
   {
      perror("poll() failed");
   }
   if (rc == 0)
   {
      printf("poll() timed out.\n");
   }
   for (;;)
   {
   ssize_t count = read(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);
   if (count == -1) {
          perror("recvfrom");
              exit(1);
   } else if (count == sizeof(buffer)) {
          fprintf(stderr, "frame too large for buffer: truncated\n");
   } else {
          printf("==================================\n")      ; 
          oam_debug_print_frame (buffer, count);
          printf("==================================\n")       ;
   }
   }
   return 0;
}

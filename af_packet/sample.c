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
#include <linux/if_ether.h>
#include <linux/filter.h>


struct sock_filter BPF_MAC_FILTER [] = {
   {BPF_LD | BPF_W | BPF_ABS, 0 , 0, 0},
   {BPF_JMP | BPF_JEQ | BPF_K , 5, 0, 0xb827eb52},
   {BPF_LD | BPF_W | BPF_ABS, 0 , 0, 3},
   {BPF_JMP | BPF_JEQ | BPF_K, 5, 0, 0xb75c},
   {BPF_RET | BPF_K, 0, 0, 0xff},
   {BPF_RET | BPF_K, 0, 0, 0},
};

unsigned short filterLen = (sizeof BPF_MAC_FILTER / sizeof BPF_MAC_FILTER[0]);
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
   const char *if_name = {"enp4s0"};
   int ifindex;
   struct pollfd fds;
   struct sock_fprog fprog;
   struct sock_filter filter;
   nfds_t nfds = 1;
   int rc =0;
   int fd = socket(AF_PACKET, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK , htons(ETH_P_8021Q));
   if (fd == -1) {
      perror("socket");
      exit(1);
   }
   printf("fd=%d\n",fd);
   /*
   memset(&fprog,0,sizeof(fprog));
   fprog.len = filterLen;
   fprog.filter = BPF_MAC_FILTER;
   if (rc = setsockopt (fd, SOL_SOCKET, SO_ATTACH_FILTER, &fprog, sizeof(fprog)) < 0)
   {
      perror("filter attach failed\n");
   }
   */
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
   for (;;)
   {
      rc = poll(&fds, nfds, 30000);
      if (rc < 0)
      {
         perror("poll() failed");
      }
      if (rc == 0)
      {
         printf("poll() timed out.\n");
      }
      printf("Something on socket to read\n");
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

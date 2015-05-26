#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "color.h"

int main(int argc,char **argv) {
  char *addr = argv[1];
  led pkt[10];
  int port = atoi(argv[2]);
  struct sockaddr_in dest;
  int i;
  int j;
  int sock=socket(AF_INET,SOCK_DGRAM,0);
  memset(&dest,0,sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr=inet_addr(addr);
  dest.sin_port=htons(port);
  while(1) {
    usleep(10000);
    for(i=0;i<sizeof(pkt)/sizeof(pkt[0]);i++) {
      set_hsv(pkt,i,j,1,1);
    }
    j++;
    j%=360;
    sendto(sock,pkt,sizeof(pkt),0,(struct sockaddr *)&dest,sizeof(dest));
  }
  return 0;
}



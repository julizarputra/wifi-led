#include "ota_conf.h"
#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "globals.h"

static struct espconn otaConn;
static esp_tcp otaTcp;

os_event_t otaq[2];

int connected=0;

void disconnectcb(void *arg) {
  os_printf("disconnect.\n");
  connected=0;
}

void recvcb(void *arg,char *data,unsigned short len) {
  char *pw=0;
  struct flashconfig newconfig;
  int i;
  os_printf("received data\n");
  if(data[0]!='A'&&data[1]!='P'&&data[2]!=':') {
    espconn_disconnect((struct espconn *)arg);
    return;
  }
  for(i=3;i<len;i++) {
    if(data[i]==':') {
      data[i]=0;
      pw=&data[i+1];
    }
    if(data[i]=='\n') {
      data[i]=0;
    }
  }
  os_strcpy(newconfig.ssid,&data[3]);
  if(pw) {
    os_strcpy(newconfig.password,pw);
  }
  os_printf("ssid: \"%s\"\npw: \"%s\"\n",newconfig.ssid,newconfig.password);
  spi_flash_erase_sector(0x3D);
  spi_flash_write(0x3D000,(uint32 *)&newconfig,sizeof(newconfig));
  os_delay_us(100000);
  system_restart();
}

void connectcb(void *arg) {
  os_printf("connected!\n");
  if(connected) {
    os_printf("already connected. Cancelling.\n");
    espconn_disconnect((struct espconn *)arg);
  } else {
    espconn_regist_recvcb((struct espconn *)arg,recvcb);
  } 
  return;
}

void thing(os_event_t *event) {
  os_printf("hello! %d\n",event->sig);
  os_delay_us(10000);
}

void ota_conf_init() {
  os_printf("Config corrupted or not saved.\n");
  struct softap_config ap_conf;
  os_memcpy(&ap_conf.ssid,"LEDSTRIP",9);
  os_memcpy(&ap_conf.password,"",1);
  ap_conf.ssid_len=0;
  ap_conf.authmode=0;
  ap_conf.ssid_hidden=0;
  ap_conf.max_connection=1;
  ap_conf.beacon_interval=100;
  wifi_set_opmode(0x2);
  wifi_softap_set_config(&ap_conf);
  otaConn.type=ESPCONN_TCP;
  otaConn.state=ESPCONN_NONE;
  otaTcp.local_port=1337;
  otaConn.proto.tcp=&otaTcp;
  if(wifi_softap_dhcps_start()) {
    os_printf("dhcp success!\n");
  } else {
    os_printf("dhcp failure!\n");
  }
  udp_init();
	espconn_regist_connectcb(&otaConn, connectcb);
	espconn_accept(&otaConn);
}

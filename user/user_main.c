#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
#include "globals.h"

#define led_prio        0
#define led_qlen    1
os_event_t led_queue[led_qlen];
os_timer_t connect_timer;

void update_led(os_event_t *events);

void check_connected(void *arg) {
  static int timeout=5;
  os_printf("Checking for connection... %d\n",timeout);
  os_timer_disarm(&connect_timer);
  if(wifi_station_get_connect_status()==STATION_GOT_IP) {
    system_os_task(update_led,led_prio,led_queue,led_qlen);
    system_os_post(led_prio,0,0);
  } else {
    if(timeout == 0) {
      spi_flash_erase_sector(0x3D);
      system_restart();
    }
    timeout--;
    os_timer_arm(&connect_timer, 2000, 0);
  }
}

void update_led(os_event_t *events) {
  os_printf("hello!\n");
  os_delay_us(1000000);
  system_os_post(led_prio, 0, 0 );
  return;
}

struct flashconfig config;

void real_init(os_event_t *ignore);

//Init function 
void user_init() {
  uart_div_modify(0, UART_CLK_FREQ/115200);
  system_os_task(real_init, 0,led_queue, 1);
  system_os_post(0, 0, 0 );
}

void real_init(os_event_t *ignore) {
  os_printf("starting up\n");
  struct station_config stationConf;
  spi_flash_read(0x3D000,(uint32 *)&config,sizeof(config));
  config.ssid[31]=0;
  config.password[63]=0;
  os_delay_us(100000);
  if(!(config.programmed&1)) {
    os_printf("ssid: \"%s\"\npw: \"%s\"\n",config.ssid,config.password);
    os_memcpy(&stationConf.ssid,config.ssid,32);
    os_memcpy(&stationConf.password,config.password,64);
    wifi_set_opmode(0x1);
    wifi_station_set_config(&stationConf);
    wifi_station_connect();
    os_timer_disarm(&connect_timer);
    os_timer_setfn(&connect_timer, (os_timer_func_t *)check_connected, 0);
    os_timer_arm(&connect_timer,2000,0);
  } else {
    ota_conf_init();
    /* serve website asking for network info */
  }
}

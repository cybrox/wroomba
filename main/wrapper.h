#ifndef WRAPPER_H
#define WRAPPER_H

  #include "freertos/FreeRTOS.h"
  #include "freertos/event_groups.h"

  #include "esp_wifi.h"
  #include "esp_system.h"
  #include "esp_event.h"
  #include "esp_event_loop.h"
  #include "esp_log.h"

  #include "lwip/err.h"
  #include "lwip/sockets.h"
  #include "lwip/sys.h"
  #include "lwip/opt.h"
  #include "lwip/arch.h"
  #include "lwip/api.h"
  #include "lwip/udp.h"

  #include "nvs_flash.h"

  #include "driver/gpio.h"

  #include "mdns.h"




  #define MILLISECONDS(ms) (ms / portTICK_PERIOD_MS)
  #define SECONDS(s) (MILLISECONDS(s * 1000))
  
  #define CLEAR_BITS_TRUE      true
  #define CLEAR_BITS_FALSE     false

  #define WAIT_ALL_BITS_TRUE   true
  #define WAIT_ALL_BITS_FALSE  false

  #define BIT_READY            0x01
  #define BIT_CONNECTED        0x02
  #define BIT_DISCONNECTED     0x04

  
  // System Globals
  mdns_server_t *mdns;
  struct udp_pcb *udp_server;


  // System Tasks
  TaskHandle_t xWroombaTask;
  TaskHandle_t xUdpHandlerTask;


  // System Event Groups
  EventGroupHandle_t xWiFiEventGroup;

#endif

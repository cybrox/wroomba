#include "task_udphandler.h"

static const char* TAG = "udphandler";

void vATaskUdpHandler(void *pvParameters) {
    // Wait until we have a WiFi connection
    xEventGroupWaitBits(
      xWiFiEventGroup,
      (BIT_CONNECTED | BIT_DISCONNECTED),
      CLEAR_BITS_FALSE,
      WAIT_ALL_BITS_FALSE,
      SECONDS(10)
    );


    // Cancel our task if we have no WiFi connection
    EventBits_t eventGroupBits = xEventGroupGetBits(xWiFiEventGroup);
    if ((eventGroupBits & BIT_CONNECTED) == 0) {
      ESP_LOGE(TAG, "WiFi: Not found, cancelling!");
      vTaskDelete(NULL);
      return;
    }
    

    vTaskDelete(NULL);
}

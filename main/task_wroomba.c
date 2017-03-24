#include "task_wroomba.h"

static const char* TAG = "wroomba";

void vATaskWroomba(void *pvParameters) {
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
    } else {
      ESP_LOGI(TAG, "Module started successfully!");
    }
    

    // Set up mDNS broadcasting
    ESP_ERROR_CHECK(mdns_init(TCPIP_ADAPTER_IF_STA, &mdns));
    mdns_set_hostname(mdns, "wroomba");
    mdns_set_instance(mdns, "wroomba");


    for(;;) {
        ESP_LOGI(TAG, "Doing some stuff!");
        vTaskDelay(SECONDS(10));
    }

    vTaskDelete(NULL);
}

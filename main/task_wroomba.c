#include "task_wroomba.h"

static const char* TAG = "wroomba";

void vATaskWroomba(void *pvParameters) {
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    // Wait until we have a WiFi connection
    xEventGroupWaitBits(
      xWiFiEventGroup,
      BIT_CONNECTED,
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

    for(;;) {
        ESP_LOGI(TAG, "Doing some stuff!");
        vTaskDelay(xDelay);
    }

    vTaskDelete(NULL);
}

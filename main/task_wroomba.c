#include "task_wroomba.h"

static const char* TAG = "wroomba";

void vATaskWroomba(void *pvParameters) {
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    for(;;) {
        ESP_LOGI(TAG, "Doing some stuff!");
        vTaskDelay(xDelay);
    }

    vTaskDelete(NULL);
}

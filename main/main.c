#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "secret/wifi.h"


// Static file tag identifier
static const char* TAG = "wroomba";
static const char* VER = "0.1";


esp_err_t event_handler(void *ctx, system_event_t *event) {
    return ESP_OK;
}


void vATaskWroomba(void *pvParameters) {
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

    for(;;) {
        ESP_LOGI(TAG, "Doing some stuff!");
        vTaskDelay(xDelay);
    }

    vTaskDelete(NULL);
}


void app_main(void) {
    nvs_flash_init();
    tcpip_adapter_init();

    // WiFi STA configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    // Start ESP main event loop
    ESP_LOGI(TAG, "Starting wroomba software v%s", VER);
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    // Check start and connect WiFi
    ESP_ERROR_CHECK(esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );
    ESP_ERROR_CHECK(esp_wifi_connect() );

    // Create our main wroomba task
    TaskHandle_t xHandle = NULL;
    BaseType_t xReturned = xTaskCreate(
        vATaskWroomba,
        "wroomba",
        4000, // TODO: Determine actually needed stack depth
        (void*)1,
        (2 | portPRIVILEGE_BIT),
        &xHandle
    );

    if (xReturned == pdPASS) {
        ESP_LOGI(TAG, "Successfully created main task");
    } else {
        ESP_LOGE(TAG, "Failed to create main task");
    }

    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    int level = 0;
    while (true) {
        gpio_set_level(GPIO_NUM_4, level);
        level = !level;
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

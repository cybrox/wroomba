#include "wrapper.h"
#include "secret/wifi.h"
#include "task_wroomba.h"


// Static file tag identifier
static const char* TAG = "main";
static const char* VER = "0.1";


// ESP Main event handler
// This will handle all incoming events from the ESP system!
//
// parameter ctx: The context in which the event was sent
// parameter event: The event that was sent from the system
//
// returns: ESP_OK
esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch(event->event_id) {
        case SYSTEM_EVENT_WIFI_READY: {
            ESP_LOGI(TAG, "WiFi: Ready!");
            xEventGroupSetBits(xWiFiEventGroup, BIT_READY);
            break;
        }

        case SYSTEM_EVENT_STA_CONNECTED: {
            ESP_LOGI(TAG, "WiFi: Connected!");
            xEventGroupSetBits(xWiFiEventGroup, BIT_CONNECTED);
            break;
        }

        case SYSTEM_EVENT_STA_DISCONNECTED: {
            ESP_LOGI(TAG, "WiFi: Disconnected!");
            xEventGroupSetBits(xWiFiEventGroup, BIT_DISCONNECTED);
            break;
        }

        default: {
            break;
        }
    }

    return ESP_OK;
}


// ESP Main appllication
// This will be responsible for initializing and configuring our software
//
// returns: void
void app_main(void) {
    nvs_flash_init();
    tcpip_adapter_init();

    // Start ESP main event loop
    ESP_LOGI(TAG, "Starting wroomba software v%s", VER);
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    // Create our wifi event group
    xWiFiEventGroup = xEventGroupCreate();
    xEventGroupClearBits(xWiFiEventGroup, (BIT_READY | BIT_CONNECTED | BIT_DISCONNECTED));

    // Create our main wroomba task
    BaseType_t xReturned = xTaskCreate(
        vATaskWroomba,
        "wroomba",
        4000, // TODO: Determine actually needed stack depth
        (void*)1,
        (2 | portPRIVILEGE_BIT),
        &xWroombaTask
    );

    if (xReturned == pdPASS) {
        ESP_LOGI(TAG, "Successfully created main task");

        // WiFi STA configuration
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

        // Check start and connect WiFi
        ESP_ERROR_CHECK(esp_wifi_init(&cfg) );
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM) );
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
        ESP_ERROR_CHECK(esp_wifi_start() );
        ESP_ERROR_CHECK(esp_wifi_connect() );
    } else {
        ESP_LOGE(TAG, "Failed to create main task");
    }

    // Keep main task alive
    for(;;) {}
}

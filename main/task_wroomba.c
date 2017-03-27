#include "task_wroomba.h"

#define BUF_SIZE 1024

static const char* TAG = "wroomba";
static QueueHandle_t uart0_queue;
static wroomba_task_state_t taskState;

// Helper function for sending roomba command
void sendRoombaCommand(wroomba_task_state_t cmd) {
  char string[1];
  string[0] = cmd;
  uart_write_bytes(UART_NUM_1, (const char*)string, 1);
}

void vAUartTask(void *pvParameters) {
  int uart_num = (int) pvParameters;
  uart_event_t event;
  size_t buffered_size;
  uint8_t* dtmp = (uint8_t*) malloc(BUF_SIZE);

  for(;;) {
    if(xQueueReceive(uart0_queue, (void*)&event, (portTickType)portMAX_DELAY)) {
      switch(event.type) {
        case UART_DATA: {
          uart_get_buffered_data_len(uart_num, &buffered_size);
          ESP_LOGI(TAG, "data, len: %d; buffered len: %d", event.size, buffered_size);
          uart_read_bytes(uart_num, dtmp, buffered_size, 10);
          ESP_LOGI(TAG, "byte0: %d, byte1: %d", dtmp[0], dtmp[1]);
          break;
        }

        case UART_FIFO_OVF:
        case UART_BUFFER_FULL:
        case UART_BREAK:
        case UART_PARITY_ERR:
        case UART_FRAME_ERR:
        case UART_PATTERN_DET:
        default: {
          ESP_LOGI(TAG, "uart event type: %d\n", event.type);
          break;
        }
      }
    }
  }

  free(dtmp);
  for (uint16_t i = 0; i < BUF_SIZE; i++) dtmp[i] = (uint8_t)0;
  vTaskDelete(NULL);
}


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


    // Set up UART transmission and receiving pins
    gpio_set_direction(GPIO_NUM_22, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_23, GPIO_MODE_INPUT);

    uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 122
    };

    // Set up UART handling on UART_NUM_1 (UART_NUM_0 is used for serial comm)
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, GPIO_NUM_22, GPIO_NUM_23, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, BUF_SIZE * 2, BUF_SIZE * 2, 10, &uart0_queue, 0));
    xTaskCreate(vAUartTask, "uart_handler", 2048, (void*)UART_NUM_1, 12, NULL);
    taskState = WROOMBA_TASK_IDLE;


    for(;;) {
      xEventGroupWaitBits(
        xCleanEventGroup,
        (BIT_START_CLEAN | BIT_STOP_CLEAN),
        CLEAR_BITS_FALSE,
        WAIT_ALL_BITS_FALSE,
        SECONDS(60)
      );


      EventBits_t eventGroupBits = xEventGroupGetBits(xCleanEventGroup);
      if ((eventGroupBits & BIT_START_CLEAN) == 1) {
        taskState = WROOMBA_TASK_START;
        ESP_LOGI(TAG, "Sending Command: Start Cleaning!");
        sendRoombaCommand(ROOMBA_COMMAND_START);
        vTaskDelay(MILLISECONDS(10));
        sendRoombaCommand(ROOMBA_COMMAND_CLEAN);
      }

      if ((eventGroupBits & BIT_STOP_CLEAN) == 1) {
        taskState = WROOMBA_TASK_STOP;
        ESP_LOGI(TAG, "Sending Command: Stop Cleaning!");
        sendRoombaCommand(ROOMBA_COMMAND_START);
        vTaskDelay(MILLISECONDS(10));
        sendRoombaCommand(ROOMBA_COMMAND_DOCK);
      }

      xEventGroupClearBits(xCleanEventGroup, (BIT_START_CLEAN | BIT_STOP_CLEAN));
    }

    vTaskDelete(NULL);
}

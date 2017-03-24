#include "task_httphandler.h"

static const char* TAG = "http_handler";

void handle_http_request(struct netconn *connection);

void vATaskHttpHandler(void *pvParameters) {
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
    

    // Start HTTP interface
    struct netconn *conn, *newconn;
    err_t err;

    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);

    do {
      err = netconn_accept(conn, &newconn);
      if (err == ERR_OK) {
        ESP_LOGI(TAG, "Received HTTP request chunk!");
        handle_http_request(newconn);
        netconn_delete(newconn);
      }
    } while (err == ERR_OK);

    vTaskDelete(NULL);
}

static const char header_success[] = "HTTP/1.1 200 OK\r\nContent-type: text\r\n\r\n";
static const char header_notfound[] = "HTTP/1.1 404 Not Found\r\nContent-type: text\r\n\r\n";
static const char body_info[] = "wroomba!";
static const char body_clean[] = "roger!";
static const char body_nf[] = "not found";

void handle_http_request(struct netconn *conn) {
  struct netbuf *inbuf;
  char *buf;
  u16_t buflen;
  err_t err;

  err = netconn_recv(conn, &inbuf);

  if (err == ERR_OK) {
    netbuf_data(inbuf, (void**)&buf, &buflen);

    // Router
    do {
  
      // GET /info
      if (String_hasSegment(buf, "GET /info", 9, 0)) {
        netconn_write(conn, header_success, sizeof(header_success) - 1, NETCONN_NOCOPY);
        netconn_write(conn, body_info, sizeof(body_info) - 1, NETCONN_NOCOPY);
        ESP_LOGI(TAG, "Received info request!");
        break;
      }

      //POST /clean
      if (String_hasSegment(buf, "POST /clean", 11, 0)) {
        netconn_write(conn, header_success, sizeof(header_success) - 1, NETCONN_NOCOPY);
        netconn_write(conn, body_clean, sizeof(body_clean) - 1, NETCONN_NOCOPY);
        ESP_LOGI(TAG, "Received cleaning request!");
        xEventGroupSetBits(xCleanEventGroup, BIT_START_CLEAN);
        break;
      }

      ESP_LOGI(TAG, "No route matched, dispatching 404");
      netconn_write(conn, header_notfound, sizeof(header_notfound) - 1, NETCONN_NOCOPY);
      netconn_write(conn, body_nf, sizeof(body_nf) - 1, NETCONN_NOCOPY);
    } while(false);
  }

  netconn_close(conn);
  netbuf_delete(inbuf);
}

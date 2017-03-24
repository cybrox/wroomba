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
    

    // Start UDP interface
    struct netconn *conn, *newconn;
    err_t err;

    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);

    do {
      err = netconn_accept(conn, &newconn);
      if (err == ERR_OK) {
        ESP_LOGI(TAG, "Received HTTP request!");
        handle_http_request(newconn);
        netconn_delete(newconn);
      }
    } while (err == ERR_OK);

    vTaskDelete(NULL);
}

const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char http_index_html[] = "<h1>wroom!</h1>";

void handle_http_request(struct netconn *conn) {
  struct netbuf *inbuf;
  char *buf;
  u16_t buflen;
  err_t err;

  err = netconn_recv(conn, &inbuf);

  if (err == ERR_OK) {
    netbuf_data(inbuf, (void**)&buf, &buflen);

    // Check if header starts with "GET /"
    if (buflen >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' && buf[3] == ' ' && buf[4] == '/') {
      netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);
      netconn_write(conn, http_index_html, sizeof(http_index_html) - 1, NETCONN_NOCOPY);
    }
  }

  netconn_close(conn);
  netbuf_delete(inbuf);
}

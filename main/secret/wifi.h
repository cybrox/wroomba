#ifndef WIFI_SECRET
#define WIFI_SECRET

  wifi_config_t sta_config = {
      .sta = {
          .ssid = "username",
          .password = "password",
          .bssid_set = false
      }
  };

#endif

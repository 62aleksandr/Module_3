#include "wifi.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

static const char *TAG = "WIFI";

static void wifi_event_handler(void *arg,
							   esp_event_base_t event_base,
							   int32_t event_id,
							   void *event_data);

#ifndef WIFI_SSID
#error "WIFI_SSID is not defined"
#endif

#ifndef WIFI_PASSWORD
#error "WIFI_PASSWORD is not defined"
#endif

esp_err_t wifi_init(void)
{
	esp_err_t err;

	err = nvs_flash_init();
	if (err != ESP_OK)
	{
		return err;
	}

	err = esp_netif_init();
	if (err != ESP_OK)
	{
		return err;
	}

	err = esp_event_loop_create_default();
	if (err != ESP_OK)
	{
		return err;
	}

	esp_event_handler_instance_register(
		WIFI_EVENT,
		ESP_EVENT_ANY_ID,
		&wifi_event_handler,
		NULL,
		NULL);

	esp_event_handler_instance_register(
		IP_EVENT,
		IP_EVENT_STA_GOT_IP,
		&wifi_event_handler,
		NULL,
		NULL);

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	err = esp_wifi_init(&cfg);
	if (err != ESP_OK)
	{
		return err;
	}

	wifi_config_t wifi_config =
		{
			.sta =
				{
					.ssid = WIFI_SSID,
					.password = WIFI_PASSWORD,
				},
		};

	err = esp_wifi_set_mode(WIFI_MODE_STA);
	if (err != ESP_OK)
	{
		return err;
	}

	err = esp_wifi_set_config(
		WIFI_IF_STA, &wifi_config);
	if (err != ESP_OK)
	{
		return err;
	}

	err = esp_wifi_start();
	if (err == ESP_OK)
	{
		ESP_LOGI(TAG, "WiFi started");
	}
	return err;
}

static void wifi_event_handler(void *arg,
							   esp_event_base_t event_base,
							   int32_t event_id,
							   void *event_data)
{
	if (event_base == WIFI_EVENT &&
		event_id == WIFI_EVENT_STA_START)
	{
		ESP_LOGI(TAG, "Connecting to WiFi...");
		esp_wifi_connect();
	}

	else if (event_base == WIFI_EVENT &&
			 event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
		ESP_LOGW(TAG, "WiFi disconnected");
		esp_wifi_connect();
	}

	else if (event_base == IP_EVENT &&
			 event_id == IP_EVENT_STA_GOT_IP)
	{
		ip_event_got_ip_t *event =
			(ip_event_got_ip_t *)event_data;

		ESP_LOGI(TAG,
				 "Got IP: " IPSTR,
				 IP2STR(&event->ip_info.ip));
	}
}

bool wifi_is_connected(void)
{
	esp_netif_t *netif =
		esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

	if (netif == NULL)
	{
		return false;
	}

	esp_netif_ip_info_t ip_info;

	if (esp_netif_get_ip_info(netif, &ip_info) != ESP_OK)
	{
		return false;
	}

	return ip_info.ip.addr != 0;
}
bool wifi_wait_connected(uint32_t timeout_ms)
{
	uint32_t elapsed = 0;
	while (!wifi_is_connected())
	{
		vTaskDelay(pdMS_TO_TICKS(100));
		elapsed += 100;

		if (elapsed >= timeout_ms)
		{
			return false;
		}
	}
	return true;
}
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>
#include "sht31.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_spiffs.h"
#include "sdmmc_cmd.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "mdns.h"
#include "lwip/apps/netbiosns.h"
#include "protocol_examples_common.h"


#define MDNS_INSTANCE "esp home web server"

#define CONFIG_MDNS_HOSTNAME "esp-home"
#define WEB_MOUNT_POINT "/www"

static char *TAG = "MAIN";

esp_err_t start_rest_server(const char *base_path);

static void initialise_mdns(void)
{
	mdns_init();
	mdns_hostname_set(CONFIG_MDNS_HOSTNAME);
	mdns_instance_name_set(MDNS_INSTANCE);

	mdns_txt_item_t serviceTxtData[] = {
		{"board", "esp32"},
		{"path", "/"}
	};

	ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
									sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));

}

/* Web deploy SF */
esp_err_t init_fs(void)
{
	esp_vfs_spiffs_conf_t conf = {
		.base_path = "/www",
		.partition_label = NULL,
		.max_files = 5,
		.format_if_mount_failed = true
	};
	esp_err_t ret = esp_vfs_spiffs_register(&conf);

	if (ret != ESP_OK)
	{
		if (ret == ESP_FAIL)
		{
			ESP_LOGE(TAG, "Failed to mount or format filesystem");
		}
		else if (ret == ESP_ERR_NOT_FOUND)
		{
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		}
		else
		{
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		}
		return ESP_FAIL;
		
	}

	size_t total = 0;
	size_t used = 0;
	ret = esp_spiffs_info(NULL, &total, &used);
	if (ret != ESP_OK)
	{
		ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
	}
	else
	{
		ESP_LOGE(TAG, "Partition size: total: %d, used: %d", total, used);
	}
	return ESP_OK;
}

void app_main() {

	/* Call initialization functions */
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	initialise_mdns();
	netbiosns_init();
	netbiosns_set_name(CONFIG_MDNS_HOSTNAME);

	//TODO: Implement full WiFi connection handling instead of example
	ESP_ERROR_CHECK(example_connect());
	
	ESP_ERROR_CHECK(init_fs());
	ESP_ERROR_CHECK(start_rest_server(WEB_MOUNT_POINT));



	ESP_LOGD(TAG, "sht31 initialize");
	sht31_init();

} 
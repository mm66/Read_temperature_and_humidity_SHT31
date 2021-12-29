/*
 * Copyright (c) 2017 Junhyuk Lee
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
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

static void initialise_mdsn(void)
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

	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	initialise_mdsn();
	netbiosns_init();
	netbiosns_set_name(CONFIG_MDNS_HOSTNAME);

	ESP_ERROR_CHECK(example_connect());
	ESP_ERROR_CHECK(init_fs());
	ESP_ERROR_CHECK(start_rest_server(WEB_MOUNT_POINT));



	ESP_LOGD(TAG, "sht31 initialize");
	sht31_init();



	// while (1) {
	// 	// TODO something
	// 	if (sht31_readTempHum()) {
	// 		float h = sht31_readHumidity();
	// 		float t = sht31_readTemperature();
	// 		ESP_LOGI(tag, "H, T : %.2f, %.2f", h, t);
	// 	} else {
	// 		ESP_LOGI(tag, "sht31_readTempHum : failed");
	// 	}

	// 	vTaskDelay(10000 / portTICK_PERIOD_MS);
	// }

	// vTaskDelete(NULL);
} 
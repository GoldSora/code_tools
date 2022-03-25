#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "esp32c3/rom/ets_sys.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "math.h"

#define ADC1_EXAMPLE_CHAN0          ADC1_CHANNEL_2
#define ADC2_EXAMPLE_CHAN0          ADC2_CHANNEL_0
#define ADC_EXAMPLE_ATTEN           ADC_ATTEN_DB_11
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#define ADC2_EXAMPLE_CHANNEL    CONFIG_EXAMPLE_ADC2_CHANNEL

static int adc_raw[2][10];
static esp_adc_cal_characteristics_t adc2_chars;

static void MQ_135(void)
{
    esp_err_t ret = ESP_OK;
    uint32_t voltage = 0;
    double temp_135 = 0;
    double ppm = 0;
    //bool cali_enable = adc_calibration_init();

    //ADC2 config
    ESP_ERROR_CHECK(adc2_config_channel_atten(ADC2_EXAMPLE_CHAN0, ADC_EXAMPLE_ATTEN));
    
    while (1) {
        ret = adc2_get_raw(ADC2_EXAMPLE_CHAN0, ADC_WIDTH_BIT_DEFAULT, &adc_raw[1][0]);
        temp_135 = adc_raw[1][0];
        ppm = temp_135 * 0.004882814;
        ESP_ERROR_CHECK(ret);
        printf("135 ppm: %f\r\n", ppm);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}


void app_main(void)
{
    xTaskCreate(MQ_135, "MQ_135", 4000, NULL, 10, NULL);
    while (1)
    {
        printf("main task\r\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

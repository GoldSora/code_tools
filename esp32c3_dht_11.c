#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "esp32c3/rom/ets_sys.h"


#define DHT11_PIN   GPIO_NUM_4

uint8_t re_data = 0;
uint8_t temp = 0;
int hue_high,hue_low,temp_high,temp_low,crc;

//delay time ms
void delay_ms(uint32_t x){
    ets_delay_us(x * 1000L);
  
}
//delay time us
void delay_us(uint32_t x){
    ets_delay_us(x);
}
//set high level
void set_out_high(){
    gpio_pad_select_gpio(DHT11_PIN);
    gpio_set_direction(DHT11_PIN,GPIO_MODE_OUTPUT);
    gpio_set_level(DHT11_PIN, 1);
}
//set low level
void set_out_low(){
    gpio_pad_select_gpio(DHT11_PIN);
    gpio_set_direction(DHT11_PIN,GPIO_MODE_OUTPUT);
    gpio_set_level(DHT11_PIN, 0);
}
//set input
void get_input_state(){
    gpio_pad_select_gpio(DHT11_PIN);
    gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);
}
//get Pin state
static uint8_t get_data()
{
	return gpio_get_level(DHT11_PIN);
}
//stop dht-11
void stop(void)//输出1
{
  gpio_pad_select_gpio(DHT11_PIN);
  gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(DHT11_PIN, 1);
}

//read bit and set bits
 int get_dht_11_bits(void)
{
    uint8_t i;
    re_data = 0;
    for(i=0;i<8;i++)
    {
        while( (get_data()==0) ){
            delay_us(10);
        }
        delay_us(35);//延时35us
        temp=0;

        if(get_data()==1) 
        temp=1;
        while((get_data()==1)  ){
            delay_us(10);
        }
        re_data <<= 1;
        re_data|=temp;
    }
    return re_data;
}
//Dht-11 main
static void Dht_11_Server(){
    while (1)
    {
        set_out_low();
    delay_ms(19);
    set_out_high();
    get_input_state();
    delay_us(30);
    if(!get_data()){
        while ((!get_data())   )
        {
            delay_us(10);
        }
        while ((get_data())    )
        {
            delay_us(10);
        } 
        hue_high = get_dht_11_bits();
        hue_low = get_dht_11_bits();
        temp_high = get_dht_11_bits();
        temp_low = get_dht_11_bits();
        crc = get_dht_11_bits();
        if ((hue_high+hue_low+temp_high+temp_low) == crc)
        {
            printf("hue:%d.%d  temp:%d.%d\r\n",hue_high,hue_low,temp_high,temp_low); 
        }
        stop(); 
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
void app_main(void)
{
    xTaskCreate(Dht_11_Server, "Dht_11_Server", 2048, NULL, 10, NULL);
    while (1)
    {
        printf("main task\r\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

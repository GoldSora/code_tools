#include "proxy.h"
#include "matterqueue.h"

/*Key Press Global value*/
#define GPIO_KEY_PIN_SEL ((1ULL << 5) | (1ULL << 4))
#define MATTER_KEY_CLUSETER 0x101

int count_num[GPIO_NUM_MAX]     = { 0 };
int count_num_end[GPIO_NUM_MAX] = { 0 };
bool count_flag[GPIO_NUM_MAX]   = { 0 };
/*Key Press Global value End*/

/*Key Press Task*/
static void key_hardware_init()
{
    gpio_config_t gpio_conf = {};
    gpio_conf.intr_type     = GPIO_INTR_DISABLE;
    gpio_conf.mode          = GPIO_MODE_INPUT;
    gpio_conf.pin_bit_mask  = GPIO_KEY_PIN_SEL;
    gpio_conf.pull_down_en  = 0;
    gpio_conf.pull_up_en    = 0;
    gpio_config(&gpio_conf);
}

void key_scan_test(gpio_num_t gpio_num)
{
    elite_queue key_queue = { 0 };

    if (gpio_get_level(gpio_num))
    {
        count_num[gpio_num]++;
        count_flag[gpio_num] = 1;
        printf("gpio_%d_count:%d\r\n", gpio_num, count_num[gpio_num]);
    }
    else if (count_flag[gpio_num] == 1)
    {
        count_num_end[gpio_num] = count_num[gpio_num];
        count_num[gpio_num]     = 0;
        count_flag[gpio_num]    = 0;

        printf("tick_conut_end_%d_count:%d\r\n", gpio_num, count_num_end[gpio_num]);
        if (count_num_end[gpio_num] > 0 && count_num_end[gpio_num] < 10)
        {
            printf("_____GPIO_%d_short_key_____\r\n", gpio_num);
            /***Short press function code ***/

            /***Short press function code end ***/
        }
        else if (count_num_end[gpio_num] >= 10 && count_num_end[gpio_num] < 100)
        {
            printf("_____GPIO_%d_long_key_____\r\n", gpio_num);
            /***Long press function code ***/

            /***Long press function code end ***/
        }
        else if (count_num_end[gpio_num] >= 100)
        {
            printf("_____reset system_____\r\n");
            /***Over press function code ***/

            /***Over press function code end ***/
        }
    }
}
static void gpio_key_scan_loop_task()
{

    key_hardware_init();
    static TickType_t key_tick_waken;
    const TickType_t xTime_key = pdMS_TO_TICKS(100);
    key_tick_waken             = xTaskGetTickCount();

    while (1)
    {
        vTaskDelayUntil(&key_tick_waken, xTime_key);
        key_scan_test(GPIO_NUM_5);
        key_scan_test(GPIO_NUM_4);
    }
}
/*Key Press Task End*/

/* Queue task */
static void Queue_Receive_task()
{
    elite_queue Receive_info = { 0 };
    BaseType_t xReturn       = pdPASS;
    while (1)
    {
        xReturn = xQueueReceive(Test_Queue_handle, &Receive_info, portMAX_DELAY);
        if (xReturn == pdPASS)
        {
            if (Receive_info.clusterId == 0x0006 && Receive_info.attributeId == 0x0000)
            {
                printf("OnOff Cluster set value: %d\r\n", Receive_info.value);
                printf("Minimum free heap size: %d bytes\n\r", esp_get_minimum_free_heap_size());
            }
        }
        else
        {
            printf("Queue Recevie failed\r\n");
        }
    }
}
/* Queue task End */

void start_task()
{
    Test_Queue_handle = xQueueCreate(3, sizeof(elite_queue));
    if (Test_Queue_handle != NULL)
    {
        printf("Queue was created susseccfully!\r\n");
    }
    else
    {
        printf("Queue was created failed!\r\n");
    }

    if (xTaskCreate(Queue_Receive_task, "Queue_Receive_task", 2048, NULL, 10, NULL) == pdPASS)
    {
        printf("gpio_key_scan_loop_task was created successfully\r\n");
    }

    if (xTaskCreate(gpio_key_scan_loop_task, "gpio_key_scan_loop_task", 2048, NULL, 10, NULL) == pdPASS)
    {
        printf("gpio_key_scan_loop_task was created successfully\r\n");
    }
}

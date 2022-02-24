#include "freertos/queue.h"

QueueHandle_t Test_Queue_handle;

typedef struct Elite_queue
{
    uint16_t endpointId;
    uint32_t clusterId;
    uint32_t attributeId;
    uint8_t mask;
    uint8_t type;
    uint16_t size;
    uint8_t value;
} elite_queue;

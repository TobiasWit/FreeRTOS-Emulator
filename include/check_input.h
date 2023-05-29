// #ifndef __DEMO_TASKS_H__
// #define __DEMO_TASKS_H__

#include "FreeRTOS.h"
#include "task.h"

void vCheckInputTask(void *pvParameters);

int checkButtonPress(unsigned char keycode, TickType_t *last_pressed, TickType_t *debounce_delay);
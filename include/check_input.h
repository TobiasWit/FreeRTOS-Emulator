// #ifndef __DEMO_TASKS_H__
// #define __DEMO_TASKS_H__

#include "FreeRTOS.h"
#include "task.h"


/**
 * @brief Prints how often the buttons A, B, C and D were pressed
*/
void writePressedButtonsCount(void);

/**
 * @brief Checks if the left mouse or the buttons A, B, C and D were pressed
*/
void vCheckInputTaskStateOne(void *pvParameters);

void vCheckInputTaskStateTwo(void *pvParameters);

int checkButtonPress(unsigned char keycode, TickType_t *last_pressed, TickType_t *debounce_delay);
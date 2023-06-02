// #ifndef __DEMO_TASKS_H__
// #define __DEMO_TASKS_H__

#include "FreeRTOS.h"
#include "task.h"


/**
 * @brief Prints how often the buttons A, B, C and D were pressed
*/
void writePressedButtonsCount(void);

/**
 * @brief Checks if the left mouse or the buttons A, B, C and D were pressed for state one
*/
void vCheckInputTaskStateOne(void *pvParameters);
/**
 * @brief Is the checkInput task for state two, checks if R, T or S was pressed
*/
void vCheckInputTaskStateTwo(void *pvParameters);
/**
 * @brief Debounce Task
*/
int checkButtonPress(unsigned char keycode, TickType_t *last_pressed, TickType_t *debounce_delay);
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "buttons.h"
#include "main.h"
#include "demo_task.h"
#include "state_machine.h"
#include "states.h"

TaskHandle_t Task1 = NULL;

void vStateOneEnter(void)
{
    vTaskResume(Task1);
}

void vStateOneExit(void)
{
    vTaskSuspend(Task1);
}

int vCheckStateInput(void)
{
    if (xSemaphoreTake(buttons.lock, 0) == pdTRUE) {
        if (buttons.buttons[KEYCODE(E)]) {
            buttons.buttons[KEYCODE(E)] = 0;
            xSemaphoreGive(buttons.lock);
            xStatesIncrementState();
            return 0;
        }
        xSemaphoreGive(buttons.lock);
    }

    return 0;
}

void vStateMachineTask(void *pvParameters)
{
    while (1) {
        uStatesRun();
        vTaskDelay(pdMS_TO_TICKS(STATE_MACHINE_PERIOD));
    }
}

int xStateMachineInit(void)
{
    if (uStatesInit()) {
        return -1;
    }
    
    if (xStatesAdd(NULL, vStateOneEnter, NULL, vStateOneExit, STATE_ONE,
                   "State One")) {
        return -1;
    }

    if (xStatesAdd(NULL, vStateTwoEnter, NULL, vStateTwoExit, STATE_TWO,
                   "State Two")) {
        return -1;
    }

    // if (xStatesAdd(NULL, vStateThreeEnter, NULL, vStateThreeExit, STATE_THREE,
    //                "State Three")) {
    //     return -1;
    // }

    return 0;
}
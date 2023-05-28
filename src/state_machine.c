#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "buttons.h"
#include "main.h"
#include "demo_task.h"
#include "moving_object.h"
#include "state_machine.h"
#include "states.h"
#include "buttons_count.h"

static TaskHandle_t MovingObjectsDisplay = NULL;

void vStateOneEnter(void)
{
    vTaskResume(MovingObjectsDisplay);
}

void vStateOneExit(void)
{
    vTaskSuspend(MovingObjectsDisplay);
    printf("###");
}



int vCheckStateInput(void)
{
    if (xSemaphoreTake(buttons.lock, 0) == pdTRUE) {
        printf("www");
        if (buttons.buttons[KEYCODE(E)]) {
            buttons.buttons[KEYCODE(E)] = 0;
            xSemaphoreGive(buttons.lock);
            printf("uuu");
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

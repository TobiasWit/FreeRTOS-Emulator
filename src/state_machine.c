#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "gfx_draw.h"
#include "timers.h"
#include <string.h>

#include "buttons.h"
#include "main.h"
#include "circle_blinking_display.h"
#include "check_input.h"
#include "state_machine.h"
#include "states.h"
#include "scheduling_priorities_test.h"


void vStateOneEnter(void)
{
    vTaskResume(MovingObjectsDisplay);
}

void vStateOneExit(void)
{
    vTaskSuspend(MovingObjectsDisplay);
}

void vStateTwoEnter(void)
{
    vTaskResume(CircleBlinkingDisplay);
    vTaskResume(CircleBlinkingStaticTask);
    vTaskResume(CircleBlinkingDynamicTask);
    vTaskResume(CheckInputTask);
    vTaskResume(NotifyButtonPressTask);
    vTaskResume(SemaphoreButtonPressTask);
    vTaskResume(ResetButtonPressTRTask);
    vTaskResume(SecondsCounterTask);
    xTimerStart(ResetButtonCountTRTimer, 0);
    gfxDrawSetGlobalXOffset(0);
    gfxDrawSetGlobalYOffset(0);
}

void vStateTwoExit(void)
{
    vTaskSuspend(CircleBlinkingDisplay);
    vTaskSuspend(CircleBlinkingStaticTask);
    vTaskSuspend(CircleBlinkingDynamicTask);
    vTaskSuspend(CheckInputTask);
    vTaskSuspend(NotifyButtonPressTask);
    vTaskSuspend(SemaphoreButtonPressTask);
    vTaskSuspend(ResetButtonPressTRTask);
    vTaskSuspend(SecondsCounterTask);
    xTimerStop(ResetButtonCountTRTimer, 0);
}
void vStateThreeEnter(void)
{
    StartingWakeTime = xTaskGetTickCount();
    
    // vTaskResume(DrawScreenExercise4);
    vTaskResume(OutputTask);
    vTaskResume(Task1);
    vTaskResume(Task2);
    vTaskResume(Task3);
    vTaskResume(Task4);
    xSemaphoreGive(StartingState3);

    
}

void vStateThreeExit(void)
{
    // vTaskSuspend(DrawScreenExercise4);
    vTaskSuspend(OutputTask);
    vTaskSuspend(Task1);
    vTaskSuspend(Task2);
    vTaskSuspend(Task3);
    vTaskSuspend(Task4);
    // for(int i=0; i<15; i++){
    // count_array[i]=0;
    // }
    memset(count_array, 0, sizeof(count_array));
    memset(coord_number, 0, sizeof(coord_number));
    // for(int i=0; i<50; i++){
    //     memset(&coord_number[i], 0, sizeof(coord_number_tupel_t));
    // }
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

    if (xStatesAdd(NULL, vStateThreeEnter, NULL, vStateThreeExit, STATE_THREE,
                   "State Three")) {
        return -1;
    }

    if (xStatesAdd(NULL, vStateTwoEnter, NULL, vStateTwoExit, STATE_TWO,
                   "State Two")) {
        return -1;
    }

    return 0;
}
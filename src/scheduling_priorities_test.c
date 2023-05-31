#include "gfx_event.h"
#include "gfx_ball.h"
#include "gfx_sound.h"
#include "gfx_utils.h"
#include "gfx_print.h"
#include "gfx_draw.h"
#include "gfx_font.h"

#include <time.h>
#include "task.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"


#include "main.h"
#include "circle_blinking_display.h"
#include "buttons.h"
#include "state_machine.h"
#include "draw.h"
#include "moving_object.h"
#include "buttons_count.h"
#include "scheduling_priorities_test.h"



#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)2560)

TaskHandle_t DrawScreenExercise4 = NULL;

void vDrawScreenExercise4(void *pvParameters)
{
    while(1){
        if(DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE){
                vGetButtonInput();
                gfxDrawClear(Blue);
                vCheckStateInput();
            }
    }
}

int xCreateSchedulingPrioritiesTestTasks(void)
{
    if (xTaskCreate(vDrawScreenExercise4, "DrawScreenExercise4", mainGENERIC_STACK_SIZE, NULL,
                        mainGENERIC_PRIORITY + 1, &DrawScreenExercise4) != pdPASS) {
            PRINT_TASK_ERROR("DrawScreenExercise4");
            goto err_draw_screen_exercise4;
        }

    vTaskSuspend(DrawScreenExercise4);

    return 0;

    err_draw_screen_exercise4:
    return -1;
}

void vDeleteSchedulingPrioritiesTestTasks(void){
    if(DrawScreenExercise4){
        vTaskDelete(DrawScreenExercise4);
    }
}
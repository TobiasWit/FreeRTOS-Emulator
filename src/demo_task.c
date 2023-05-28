#include "gfx_event.h"
#include "gfx_ball.h"
#include "gfx_sound.h"
#include "gfx_utils.h"
#include "gfx_print.h"
#include "gfx_draw.h"

#include "task.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "main.h"
#include "demo_task.h"
#include "buttons.h"
#include "state_machine.h"
#include "draw.h"
#include "moving_object.h"
#include "buttons_count.h"



#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)2560)
#define STACK_SIZE_STATIC 200
#define TIME_PERIOD_CIRCLE_BLINKING_STATIC 1000
#define TIME_PERIOD_CIRCLE_BLINKING_DYNAMIC 500

TaskHandle_t Task2 = NULL;
TaskHandle_t CircleBlinkingStaticTask = NULL;
TaskHandle_t CircleBlinkingDynamicTask = NULL;


//static task allocation
StaticTask_t xTaskBuffer;
StackType_t xStack[STACK_SIZE_STATIC];




void vStateTwoEnter(void)
{
    printf("***");
    vTaskResume(Task2);
    vTaskResume(CircleBlinkingStaticTask);
    vTaskResume(CircleBlinkingDynamicTask);
    gfxDrawSetGlobalXOffset(0);
    gfxDrawSetGlobalYOffset(0);
}

void vStateTwoExit(void)
{
    vTaskSuspend(Task2);
    vTaskSuspend(CircleBlinkingStaticTask);
    vTaskSuspend(CircleBlinkingDynamicTask);
}

void vCircleBlinkingStaticTask(void *pvParameters)
{
    TickType_t period_counter = 0;
    while (1){
        
        period_counter = xTaskGetTickCount() % TIME_PERIOD_CIRCLE_BLINKING_STATIC;
        if (period_counter >= TIME_PERIOD_CIRCLE_BLINKING_STATIC / 2){
            xTaskNotify(Task2, 0x01, eSetBits);
        }
        vTaskDelay(1);
    }

}

void vCircleBlinkingDynamicTask(void *pvParameters)
{
    TickType_t period_counter = 0;
    while (1){
        
        period_counter = xTaskGetTickCount() % TIME_PERIOD_CIRCLE_BLINKING_DYNAMIC;
        if (period_counter >= TIME_PERIOD_CIRCLE_BLINKING_DYNAMIC / 2){
            xTaskNotify(Task2, 0x02, eSetBits);
        }
        vTaskDelay(1);
    }

}

void vTask2(void *pvParamters)
{
    circle_moving_t circle_blink_static = {SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2, 30, Green};
    circle_moving_t circle_blink_dynamic = {SCREEN_WIDTH/2 + 40, SCREEN_HEIGHT/2, 30, Blue};

    uint32_t notification = 0;
 
    while(1){
        if (DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE){
                prints("sss");

                vGetButtonInput();
                gfxDrawClear(White);
                notification = ulTaskNotifyTake(pdTRUE, 0);

                if (notification & 0x01){
                    gfxDrawCircle(circle_blink_static.x, circle_blink_static.y,
                                circle_blink_static.radius, circle_blink_static.colour);
                }

                if (notification & 0x02){
                    gfxDrawCircle(circle_blink_dynamic.x, circle_blink_dynamic.y,
                                circle_blink_dynamic.radius, circle_blink_dynamic.colour);
                }
                //vCheckStateInput();
            }

    }
}












int xCreateDemoTask(void)
{

    if (xTaskCreate(vTask2, "Task2", mainGENERIC_STACK_SIZE, NULL,
                        mainGENERIC_PRIORITY + 1, &Task2) != pdPASS) {
            PRINT_TASK_ERROR("Task2");
            goto err_task2;
        }

    if (xTaskCreate(vCircleBlinkingDynamicTask, "CircleBlinkingDynamicTask", 
                        mainGENERIC_STACK_SIZE, NULL,
                        mainGENERIC_PRIORITY, &CircleBlinkingDynamicTask) != pdPASS) {
            PRINT_TASK_ERROR("CircleBlinkingDynamicTask");
            goto err_circle_blinking_dynamic_task;
        }

    CircleBlinkingStaticTask = xTaskCreateStatic(vCircleBlinkingStaticTask,
                        "CircleBlinkingStaticTask", STACK_SIZE_STATIC, NULL,
                        mainGENERIC_PRIORITY, xStack, &xTaskBuffer);
    if (!CircleBlinkingStaticTask){
        PRINT_TASK_ERROR("CircleBlinkingStaticTask");
        goto err_circle_blinking_static_task;
    }
    
    vTaskSuspend(Task2);
    vTaskSuspend(CircleBlinkingDynamicTask);
    vTaskSuspend(CircleBlinkingStaticTask);
 

    return 0;

err_circle_blinking_static_task:
    vTaskDelete(CircleBlinkingDynamicTask);
err_circle_blinking_dynamic_task:
    vTaskDelete(Task2);
err_task2:
    return -1;
}

void vDeleteDemoTask(void)
{
    if (Task2) {
        vTaskDelete(Task2);
    }
    if (CircleBlinkingDynamicTask) {
        vTaskDelete(CircleBlinkingDynamicTask);
    }
    if (CircleBlinkingStaticTask) {
        vTaskDelete(CircleBlinkingStaticTask);
    }
}
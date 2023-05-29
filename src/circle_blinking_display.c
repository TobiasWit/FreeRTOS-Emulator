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



#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)2560)
#define STACK_SIZE_STATIC 200
#define TIME_PERIOD_CIRCLE_BLINKING_STATIC 1000
#define TIME_PERIOD_CIRCLE_BLINKING_DYNAMIC 500

TaskHandle_t CircleBlinkingDisplay = NULL;
TaskHandle_t CircleBlinkingStaticTask = NULL;
TaskHandle_t CircleBlinkingDynamicTask = NULL;
TaskHandle_t NotifyButtonPressTask = NULL;
TaskHandle_t SemaphoreButtonPressTask = NULL;
TaskHandle_t ResetButtonPressTRTask = NULL;
TaskHandle_t SecondsCounterTask = NULL;

SemaphoreHandle_t ButtonPressR = NULL;

TimerHandle_t ResetButtonCountTRTimer = NULL;


//static task allocation
StaticTask_t xTaskBuffer;
StackType_t xStack[STACK_SIZE_STATIC];



button_press_tr_t button_press_TR = {0};
seconds_counter_t seconds_count1 = {0};



void vCircleBlinkingStaticTask(void *pvParameters)
{
    TickType_t period_counter = 0;
    while (1){
        period_counter = xTaskGetTickCount() % TIME_PERIOD_CIRCLE_BLINKING_STATIC;
        if (period_counter >= TIME_PERIOD_CIRCLE_BLINKING_STATIC / 2){
            xTaskNotify(CircleBlinkingDisplay, 0x01, eSetBits);
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
            xTaskNotify(CircleBlinkingDisplay, 0x02, eSetBits);
        }
        vTaskDelay(1);
    }

}

void vNotifyButtonPressTask(void *pvParameters)
{
    uint32_t notification = 0;
    while(1){
        notification = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(notification & 0x01){
            if(xSemaphoreTake(button_press_TR.lock, portMAX_DELAY) == pdTRUE){
                button_press_TR.value[0] ++;
                xSemaphoreGive(button_press_TR.lock);
            }
        }
    }
    vTaskDelay(10);
}

void vSemaphoreButtonPressTask(void *pvPatameters)
{
    while(1){
        if(xSemaphoreTake(ButtonPressR, portMAX_DELAY) == pdTRUE){
            if(xSemaphoreTake(button_press_TR.lock, portMAX_DELAY) == pdTRUE){
                button_press_TR.value[1] ++;
                xSemaphoreGive(button_press_TR.lock);
            }
        }
    }
    vTaskDelay(10);
}

void writePressedButtonsCountTR(void)
{
    static char str[50] = { 0 };
    static int text_width;
    static int text_height;

    gfxFontSetSize((ssize_t)20);

    sprintf(str, "Button T pressed %d times.", button_press_TR.value[0]);
        if (!gfxGetTextSize((char *)str, &text_width, &text_height)){
            gfxDrawText(str, 20, SCREEN_HEIGHT - text_height*3 , Black);
        }
    sprintf(str, "Button R pressed %d times.", button_press_TR.value[1]);
        if (!gfxGetTextSize((char *)str, &text_width, &text_height)){
            gfxDrawText(str, 20, SCREEN_HEIGHT - text_height*2 , Black);
        }
}

void vResetButtonPressTRTask(void *pvParameters)
{
    while(1){
        if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY)){
            if(xSemaphoreTake(button_press_TR.lock, portMAX_DELAY) == pdTRUE){
                button_press_TR.value[0] = 0;
                button_press_TR.value[1] = 0;
                xSemaphoreGive(button_press_TR.lock);
            }
        }

    vTaskDelay(10);
    }
}

void resetButtonCountTRCallback(TimerHandle_t ResetButtonCountTRTimer)
{
    xTaskNotifyGive(ResetButtonPressTRTask);
}

void vSecondsCounterTask(void *pvParameters)
{
    TickType_t lastWakeTime;
    lastWakeTime = xTaskGetTickCount();
    while(1){
        if(xTaskGetTickCount() - lastWakeTime >= pdMS_TO_TICKS(1000)){
            if(xSemaphoreTake(seconds_count1.lock, portMAX_DELAY) == pdTRUE){
                seconds_count1.value++;
                xSemaphoreGive(seconds_count1.lock);
                lastWakeTime = xTaskGetTickCount();
            }
        }
        
        // if(xSemaphoreTake(seconds_count1.lock, portMAX_DELAY) == pdTRUE){
        //         seconds_count1.value ++;
        //         xSemaphoreGive(seconds_count1.lock);
        //     }
        vTaskDelay(5);
    }
    
}

void writeSecondsCount(void)
{
    static char str[50] = { 0 };
    static int text_width;
    static int text_height;

    gfxFontSetSize((ssize_t)20);

    sprintf(str, "Seconds passed: %d", seconds_count1.value);
        if (!gfxGetTextSize((char *)str, &text_width, &text_height)){
            gfxDrawText(str, 20, SCREEN_HEIGHT - text_height*5 , Black);
        }
}

void vCircleBlnkingDisplay(void *pvParamters)
{
    circle_moving_t circle_blink_static = {SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2, 30, Green};
    circle_moving_t circle_blink_dynamic = {SCREEN_WIDTH/2 + 40, SCREEN_HEIGHT/2, 30, Blue};

    uint32_t notification = 0;
 
    while(1){
        if (DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE){

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

                if (notification & 0x04){
                    gfxDrawCircle(40, 40, 20, Red);
                }

                vDrawFPS();

                writePressedButtonsCountTR();

                writeSecondsCount();

                vCheckStateInput();
            }

    }
}



int xCreateDemoTask(void)
{
    

    if (xTaskCreate(vCircleBlnkingDisplay, "CircleBlinkingDisplay", mainGENERIC_STACK_SIZE, NULL,
                        mainGENERIC_PRIORITY + 1, &CircleBlinkingDisplay) != pdPASS) {
            PRINT_TASK_ERROR("CircleBlinkingDisplay");
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

    if (xTaskCreate(vNotifyButtonPressTask, "NotifyButtonPressTask",
                        mainGENERIC_STACK_SIZE, NULL,
                        mainGENERIC_PRIORITY, &NotifyButtonPressTask) != pdPASS) {
            PRINT_TASK_ERROR("NotifyButtonPressTask");
            goto err_notify_button_press_task;
        }

    if (xTaskCreate(vSemaphoreButtonPressTask, "SemaphoreButtonPressTask",
                        mainGENERIC_STACK_SIZE, NULL,
                        mainGENERIC_PRIORITY, &SemaphoreButtonPressTask) != pdPASS) {
            PRINT_TASK_ERROR("SemaphoreButtonPressTask");
            goto err_semaphore_button_press_task;
        }

    if (xTaskCreate(vResetButtonPressTRTask, "ResetButtonPressTRTask",
                        mainGENERIC_STACK_SIZE, NULL,
                        mainGENERIC_PRIORITY, &ResetButtonPressTRTask) != pdPASS) {
            PRINT_TASK_ERROR("ResetButtonPresTRTask");
            goto err_reset_button_press_tr_task;
        }

    if (xTaskCreate(vSecondsCounterTask, "SecondsCounterTask",
                        mainGENERIC_STACK_SIZE, NULL,
                        mainGENERIC_PRIORITY, &SecondsCounterTask) != pdPASS){
            PRINT_TASK_ERROR("SecondsCounterTask");
            goto err_seconds_counter_task;
        }

    button_press_TR.lock = xSemaphoreCreateMutex();
        if (!button_press_TR.lock) {
            PRINT_ERROR("Failed to create button_press_TR lock");
            goto err_button_press_tr_lock;
    }

    ButtonPressR = xSemaphoreCreateBinary();
        if(!ButtonPressR){
            PRINT_ERROR("Failed to create ButtonPresR");
            goto err_button_press_r;
        }

    seconds_count1.lock = xSemaphoreCreateMutex();
        if (!seconds_count1.lock){
            PRINT_ERROR("Failed to create seconds_count1.lock");
            goto err_seconds_count1_lock;
        }

    ResetButtonCountTRTimer = xTimerCreate("ResetButtonCountTRTimer",
                    pdMS_TO_TICKS(15000), pdTRUE, (void *)0,
                    resetButtonCountTRCallback);
	if (!ResetButtonCountTRTimer) {
		PRINT_ERROR("Failed to create ResetButtonCountTRTimer");
		goto err_reset_button_count_tr_timer;
	}
    

    vTaskSuspend(CircleBlinkingDisplay);
    vTaskSuspend(CircleBlinkingDynamicTask);
    vTaskSuspend(CircleBlinkingStaticTask);
    vTaskSuspend(NotifyButtonPressTask);
    vTaskSuspend(SemaphoreButtonPressTask);
    vTaskSuspend(ResetButtonPressTRTask);
    vTaskSuspend(SecondsCounterTask);
 

    return 0;

err_reset_button_count_tr_timer:
    vSemaphoreDelete(seconds_count1.lock);
err_seconds_count1_lock:
    vSemaphoreDelete(ButtonPressR);
err_button_press_r:
    vSemaphoreDelete(button_press_TR.lock);
err_button_press_tr_lock:
    vTaskDelete(SecondsCounterTask);
err_seconds_counter_task:
    vTaskDelete(ResetButtonPressTRTask);
err_reset_button_press_tr_task:
    vTaskDelete(SemaphoreButtonPressTask);
err_semaphore_button_press_task:
    vTaskDelete(NotifyButtonPressTask);
err_notify_button_press_task:
    vTaskDelete(CircleBlinkingStaticTask);
err_circle_blinking_static_task:
    vTaskDelete(CircleBlinkingDynamicTask);
err_circle_blinking_dynamic_task:
    vTaskDelete(CircleBlinkingDisplay);
err_task2:
    return -1;
}

void vDeleteDemoTask(void)
{
    if (CircleBlinkingDisplay) {
        vTaskDelete(CircleBlinkingDisplay);
    }
    if (CircleBlinkingDynamicTask) {
        vTaskDelete(CircleBlinkingDynamicTask);
    }
    if (CircleBlinkingStaticTask) {
        vTaskDelete(CircleBlinkingStaticTask);
    }
    if(NotifyButtonPressTask){
        vTaskDelete(NotifyButtonPressTask);
    }
    if(ResetButtonPressTRTask){
        vTaskDelete(ResetButtonPressTRTask);
    }
    if(SecondsCounterTask){
        vTaskDelete(SecondsCounterTask);
    }
    if(button_press_TR.lock){
        vSemaphoreDelete(button_press_TR.lock);
    }
    if(ButtonPressR){
        vSemaphoreDelete(ButtonPressR);
    }
    if(ResetButtonCountTRTimer){
        xTimerDelete(ResetButtonCountTRTimer, 0);
    }
}
#include "gfx_event.h"
#include "gfx_ball.h"
#include "gfx_sound.h"
#include "gfx_utils.h"
#include "gfx_print.h"

#include "main.h"
#include "demo_task.h"
#include "buttons.h"
#include "state_machine.h"
#include "draw.h"
#include "moving_object.h"
#include "buttons_count.h"
#include "main.h"

#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)2560)

TaskHandle_t Task1 = NULL;
TaskHandle_t Task2 = NULL;
TaskHandle_t TaskCircleBlink1 = NULL;
TaskHandle_t TaskCircleBlink2 = NULL;




circle_moving_t circle_moving1 = {0, 0, 40, Red};
rect_moving_t rect_moving1 = {0, 0, 80, 80, Green};
coord_t triangle1[] = {{SCREEN_WIDTH/2, SCREEN_HEIGHT/2-40}, {SCREEN_WIDTH/2-45,
        SCREEN_HEIGHT/2+40}, {SCREEN_WIDTH/2+45, SCREEN_HEIGHT/2+40}};
text_moving_t text_moving1 = { 0, 0, {0}, 0, 0, Orange, 100, 0 };


void vStateOneEnter(void)
{
    vTaskResume(Task1);
}

void vStateOneExit(void)
{
    vTaskSuspend(Task1);
}

void vTask1(void *pvParameters)
{
    TickType_t xLastWakeTime, prevWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    prevWakeTime = xLastWakeTime;

    while(1){
        if(DrawSignal)
            if(xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE){

                    vGetButtonInput();
                    xLastWakeTime = xTaskGetTickCount();

                    gfxDrawClear(White);
                    
                    updateCirclePosition(&circle_moving1, xLastWakeTime - prevWakeTime);                  
                    gfxDrawCircle(circle_moving1.x, circle_moving1.y, circle_moving1.radius,
                                circle_moving1.colour);

                    updateRectPosition(&rect_moving1, xLastWakeTime - prevWakeTime);
                    gfxDrawFilledBox(rect_moving1.x, rect_moving1.y, rect_moving1.w,
                                rect_moving1.h, rect_moving1.colour);

                    gfxDrawTriangle(&triangle1[0], Blue);

                    writePressedButtonsCount();

                    writeStaticText();
                   
                    writeMovingText(&text_moving1, xLastWakeTime, prevWakeTime);

                    writeMouseCoord();

                    moveScreenInMouseDirection();

                    CheckButtonInput();

                    vCheckStateInput();

                    prevWakeTime = xLastWakeTime;   
                }
    }
}


void vStateTwoEnter(void)
{
    vTaskResume(Task2);
    vTaskResume(TaskCircleBlink1);
    vTaskResume(TaskCircleBlink2);
}

void vStateTwoExit(void)
{
    vTaskSuspend(Task2);
    vTaskSuspend(TaskCircleBlink1);
    vTaskSuspend(TaskCircleBlink2);
}

void vTaskCircleBlink1(void *pvParameters)
{
    while(1){
        
    }
}

void vTaskCircleBlink2(void *pvParameters)
{
    while(1){
        
    }
}

void vTask2(void *pvParamters)
{
    circle_moving_t circle_blink_static = {200, 200, 30, Green};
    circle_moving_t circle_blink_dynamic = {400, 200, 30, Blue};
    while(1){
        if (DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE){
                vGetButtonInput();
                gfxDrawClear(White);


                vCheckStateInput();
            }

    }
}













int xCreateDemoTask(void)
{
    if (xTaskCreate(vTask1, "Task1", mainGENERIC_STACK_SIZE *2, NULL,
                        mainGENERIC_PRIORITY + 1, &Task1) != pdPASS) {
            PRINT_TASK_ERROR("Task1");
            goto err_task1;
        }

    if (xTaskCreate(vTask2, "Task2", mainGENERIC_STACK_SIZE *2, NULL,
                        mainGENERIC_PRIORITY + 1, &Task2) != pdPASS) {
            PRINT_TASK_ERROR("Task2");
            goto err_task2;
        }

    if (xTaskCreate(vTaskCircleBlink1, "TaskCircleBlink1", mainGENERIC_STACK_SIZE *2, NULL,
                        mainGENERIC_PRIORITY + 1, &TaskCircleBlink1) != pdPASS) {
            PRINT_TASK_ERROR("TaskCircleBlink1");
            goto err_task_circle_blink1;
        }

    if (xTaskCreate(vTaskCircleBlink2, "TaskCircleBlink2", mainGENERIC_STACK_SIZE *2, NULL,
                        mainGENERIC_PRIORITY + 1, &TaskCircleBlink2) != pdPASS) {
            PRINT_TASK_ERROR("TaskCircleBlink2");
            goto err_task_circle_blink2;
        }
    
    
    
    vTaskSuspend(Task1);
    vTaskSuspend(Task2);
    vTaskSuspend(TaskCircleBlink1);
    vTaskSuspend(TaskCircleBlink2);
    

    return 0;

err_task_circle_blink2:
    vTaskDelete(TaskCircleBlink1);
err_task_circle_blink1:
    vTaskDelete(Task2);
err_task2:
    vTaskDelete(Task1);
err_task1:
    return -1;
}

void vDeleteDemoTask(void)
{
    if (Task1) {
        vTaskDelete(Task1);
    }
    if (Task2) {
        vTaskDelete(Task2);
    }
    if (TaskCircleBlink1) {
        vTaskDelete(TaskCircleBlink1);
    }
    if (TaskCircleBlink2) {
        vTaskDelete(TaskCircleBlink2);
    }
    
    
}
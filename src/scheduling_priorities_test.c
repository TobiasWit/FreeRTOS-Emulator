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
#include "queue.h"


#include "main.h"
#include "circle_blinking_display.h"
#include "buttons.h"
#include "state_machine.h"
#include "draw.h"
#include "moving_object.h"
#include "scheduling_priorities_test.h"



#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)2560)

// TaskHandle_t DrawScreenExercise4 = NULL;
TaskHandle_t Task1 = NULL;
TaskHandle_t Task2 = NULL;
TaskHandle_t Task3 = NULL;
TaskHandle_t Task4 = NULL;
TaskHandle_t OutputTask = NULL;

SemaphoreHandle_t WakeTask3 = NULL;
SemaphoreHandle_t StartingState3 = NULL;

QueueHandle_t PrintNumbersQueue = NULL;

TickType_t StartingWakeTime = 0;

void addToQueueSuspendTasks(number_t *inQueue, TickType_t *lastWakeTime,
                TaskHandle_t Taskx, TickType_t delay)
{   
    if(xSemaphoreTake(StartingState3, 0) ==pdTRUE){
                    StartingWakeTime = xTaskGetTickCount();
                }
    *lastWakeTime = xTaskGetTickCount();
    if (*lastWakeTime < StartingWakeTime + 15){
        inQueue->tick = *lastWakeTime - StartingWakeTime;
        
        xQueueSend(PrintNumbersQueue, inQueue, portMAX_DELAY);

        if (Taskx == Task2){
            xSemaphoreGive(WakeTask3);
        }
        if(Taskx != Task3){
            vTaskDelayUntil(lastWakeTime, delay);
        }
    } else{
        if(Taskx == Task2){
            vTaskSuspend(Task3);
        }
        vTaskSuspend(Taskx);
    }
}

void vTask1(void * pvParameters)
{
    TickType_t lastWakeTime;
    number_t inQueue = {0};
    inQueue.value = 1;
    while(1){
        addToQueueSuspendTasks(&inQueue, &lastWakeTime, Task1, 1);
    }
}

void vTask2(void * pvParameters)
{
    TickType_t lastWakeTime;
    number_t inQueue = {0};
    inQueue.value = 2;
    while(1){
        addToQueueSuspendTasks(&inQueue, &lastWakeTime, Task2, 2);
    }
}

void vTask3(void * pvParameters)
{
    TickType_t lastWakeTime;
    number_t inQueue = {0};
    inQueue.value = 3;
    while (1){
        if (xSemaphoreTake(WakeTask3, portMAX_DELAY) == pdTRUE){
            addToQueueSuspendTasks(&inQueue, &lastWakeTime, Task3, 0);
        }
    }
    
}

void vTask4(void * pvParameters)
{
    TickType_t lastWakeTime;
    number_t inQueue = {0};
    inQueue.value = 4;
    while(1){
        addToQueueSuspendTasks(&inQueue, &lastWakeTime, Task4, 4);
    }   
}




signed short count_array[15] = {0} ;

    coord_number_tupel_t coord_number[50] = {0};

void vOutputTask(void * pvParameters)
{
    number_t queueBuffer = {0};
    
    

    coord_t printMessageSize = {0};
    coord_t coordOffset = {0};
    
    static char str[50] = {0};
    static int text_width;
    static int text_height;
    
    gfxFontSetSize((ssize_t)20);

    sprintf(str, "Tick 15:");
    gfxGetTextSize((char *)str, &text_width, &text_height);
    printMessageSize.y = text_height + 5;
    coordOffset.x = text_width + 20;
    coordOffset.y = 10;

    sprintf(str, "4");
    gfxGetTextSize((char *)str, &text_width, &text_height);
    printMessageSize.x = text_width + 5;

    int hilf=0;
    TickType_t lastWakeTime;

    while(1){
        if(DrawSignal)
            if (xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE){
                lastWakeTime = xTaskGetTickCount();
                vGetButtonInput();
                gfxDrawClear(Green);

                

                for (int i = 1; i <=15; i++){
                    sprintf(str, "Tick %d:", i);
                    gfxDrawText(str, 10,
                                    coordOffset.y + printMessageSize.y * (i - 1), Black);
                    
                }
                
                hilf=0;
                while(xQueueReceive(PrintNumbersQueue, &queueBuffer, 0) == pdTRUE){

                    coord_number[hilf].value = queueBuffer.value;
                    coord_number[hilf].coord.x = 
                            coordOffset.x + printMessageSize.x * count_array[queueBuffer.tick];
                    coord_number[hilf].coord.y =
                            coordOffset.y + printMessageSize.y * queueBuffer.tick;

                    count_array[queueBuffer.tick] ++;
                    hilf ++;
                }
                
                hilf = 0;
                while(coord_number[hilf].value != 0){
                    sprintf(str, "%d", coord_number[hilf].value);
                    gfxDrawText(str, coord_number[hilf].coord.x, coord_number[hilf].coord.y, Black);
       
                    hilf ++;
                }
                

                vCheckStateInput();
            }
            vTaskDelayUntil(&lastWakeTime, 1);
    }
}


int xCreateSchedulingPrioritiesTestTasks(void)
{

    if (xTaskCreate(vTask1, "Task1", mainGENERIC_STACK_SIZE, NULL, 1,
                        &Task1) != pdPASS){
            PRINT_TASK_ERROR("Task1");
            goto err_task1;
        }
    
    if (xTaskCreate(vTask2, "Task2", mainGENERIC_STACK_SIZE, NULL, 2,
                        &Task2) != pdPASS){
            PRINT_TASK_ERROR("Task2");
            goto err_task2;
        }
    
    if (xTaskCreate(vTask3, "Task3", mainGENERIC_STACK_SIZE, NULL, 3,
                        &Task3) != pdPASS){
            PRINT_TASK_ERROR("Task3");
            goto err_task3;
        }

    if (xTaskCreate(vTask4, "Task4", mainGENERIC_STACK_SIZE, NULL, 4,
                        &Task4) != pdPASS){
            PRINT_TASK_ERROR("Task4");
            goto err_task4;
        }

    if (xTaskCreate(vOutputTask, "OutputTask", mainGENERIC_STACK_SIZE, NULL, 1,
                        &OutputTask) != pdPASS){
            PRINT_TASK_ERROR("OutputTask");
            goto err_output_task;
        }

    PrintNumbersQueue = xQueueCreate(100, sizeof(number_t));
    if(!PrintNumbersQueue){
            PRINT_ERROR("Failed to create PrintNumbersQueue");
            goto err_print_numbers_queue;
    }

    WakeTask3 = xSemaphoreCreateBinary();
    if (!WakeTask3) {
        PRINT_ERROR("Failed to create WakeTask3");
        goto err_wake_task3;
    }

    StartingState3 = xSemaphoreCreateBinary();
    if (!StartingState3) {
        PRINT_ERROR("Failed to create WakeTask3");
        goto err_starting_state3;
    }

    // vTaskSuspend(DrawScreenExercise4);
    vTaskSuspend(Task1);
    vTaskSuspend(Task2);
    vTaskSuspend(Task3);
    vTaskSuspend(Task4);
    vTaskSuspend(OutputTask);

    return 0;

err_starting_state3:
    vQueueDelete(WakeTask3);
err_wake_task3:
    vQueueDelete(PrintNumbersQueue);
err_print_numbers_queue:
    vTaskDelete(OutputTask);
err_output_task:
    vTaskDelete(Task4);
err_task4:
    vTaskDelete(Task3);
err_task3:
    vTaskDelete(Task2);
err_task2:
    vTaskDelete(Task1);
err_task1:
//     vTaskDelete(DrawScreenExercise4);
// err_draw_screen_exercise4:
    return -1;
}

void vDeleteSchedulingPrioritiesTestTasks(void)
{
    // if(DrawScreenExercise4){
    //     vTaskDelete(DrawScreenExercise4);
    // }
    if(Task1){
        vTaskDelete(Task1);
    }
    if(Task2){
        vTaskDelete(Task2);
    }
    if(Task3){
        vTaskDelete(Task3);
    }
    if(Task4){
        vTaskDelete(Task4);
    }
    if(OutputTask){
        vTaskDelete(OutputTask);
    }

    //delete the semaphores and queues
}
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
#include "buttons_count.h"
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

QueueHandle_t PrintNumbersQueue = NULL;

void addToQueueSuspendTasks(number_t *inQueue, TickType_t *lastWakeTime,
                TaskHandle_t Taskx, TickType_t delay)
{
    *lastWakeTime = xTaskGetTickCount();
    if (*lastWakeTime <= StartingWakeTime + 15){
        inQueue->tick = *lastWakeTime;
        xQueueSend(PrintNumbersQueue, inQueue, portMAX_DELAY);

        if (Taskx == Task2){
            xSemaphoreGive(WakeTask3);
        }

        vTaskDelayUntil(lastWakeTime, delay);
    } else{
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
    
}

void vTask3(void * pvParameters)
{
    
}

void vTask4(void * pvParameters)
{
    
}

void vOutputTask(void * pvParameters)
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



// void vDrawScreenExercise4(void *pvParameters)
// {
//     while(1){
//         if(DrawSignal)
//             if (xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE){
//                 vGetButtonInput();
//                 gfxDrawClear(Blue);
//                 vCheckStateInput();
//             }
//     }
// }

int xCreateSchedulingPrioritiesTestTasks(void)
{
    // if (xTaskCreate(vDrawScreenExercise4, "DrawScreenExercise4", mainGENERIC_STACK_SIZE, NULL,
    //                     mainGENERIC_PRIORITY + 1, &DrawScreenExercise4) != pdPASS) {
    //         PRINT_TASK_ERROR("DrawScreenExercise4");
    //         goto err_draw_screen_exercise4;
    //     }

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

    // vTaskSuspend(DrawScreenExercise4);
    vTaskSuspend(Task1);
    vTaskSuspend(Task2);
    vTaskSuspend(Task3);
    vTaskSuspend(Task4);
    vTaskSuspend(OutputTask);

    return 0;

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
}
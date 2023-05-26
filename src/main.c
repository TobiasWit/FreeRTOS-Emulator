#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#include <SDL2/SDL_scancode.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"


#include "gfx_draw.h"
#include "gfx_font.h"
#include "gfx_event.h"
#include "gfx_sound.h"
#include "gfx_utils.h"
#include "gfx_FreeRTOS_utils.h"
#include "gfx_print.h"
#include "draw.h"
#include "buttons.h"


#include "moving_object.h"
#include "buttons_count.h"
#include "demo_task.h"
#include "state_machine.h"

#ifdef TRACE_FUNCTIONS
#include "tracer.h"
#endif

#define KEYCODE(CHAR) SDL_SCANCODE_##CHAR

#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)2560)


static TaskHandle_t BufferSwap = NULL;
static TaskHandle_t Task1 = NULL;
static TaskHandle_t Task2 = NULL;
static TaskHandle_t CheckInputTask = NULL;
static TaskHandle_t StateMachine = NULL;

SemaphoreHandle_t DrawSignal = NULL;
SemaphoreHandle_t ScreenLock = NULL;


void vSwapBuffers(void *pvParameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t frameratePeriod = 20;

    while (1) {
        gfxDrawUpdateScreen();
        gfxEventFetchEvents(FETCH_EVENT_BLOCK);
        xSemaphoreGive(DrawSignal);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(frameratePeriod));
    }
}


int main(int argc, char *argv[])
{
    char *bin_folder_path = gfxUtilGetBinFolderPath(argv[0]);

    prints("Initializing: ");


    if (gfxDrawInit(bin_folder_path)) {
        PRINT_ERROR("Failed to intialize drawing");
        goto err_init_drawing;
    }
    else {
        prints("drawing");
    }

    if (gfxEventInit()) {
        PRINT_ERROR("Failed to initialize events");
        goto err_init_events;
    }
    else {
        prints(", events");
    }

    if (gfxSoundInit(bin_folder_path)) {
        PRINT_ERROR("Failed to initialize audio");
        goto err_init_audio;
    }
    else {
        prints(", and audio\n");
    }

    if (gfxSafePrintInit()) {
        PRINT_ERROR("Failed to init safe print");
        goto err_init_safe_print;
    }

    if (xButtonsInit()){
        PRINT_ERROR("Failed to init buttons");
        goto err_buttons_lock;
    }

    DrawSignal = xSemaphoreCreateBinary(); // Screen buffer locking
    if (!DrawSignal) {
        PRINT_ERROR("Failed to create draw signal");
        goto err_draw_signal;
    }

    ScreenLock = xSemaphoreCreateMutex();
    if (!ScreenLock) {
        PRINT_ERROR("Failed to create screen lock");
        goto err_screen_lock;
    }

prints("###\n");
    if (xTaskCreate(vStateMachineTask, "StateMachine",
                    512, NULL,
                    configMAX_PRIORITIES - 1, &StateMachine) != pdPASS) {
        PRINT_TASK_ERROR("StateMachine");
        goto err_statemachinetask;
    }


    if (xTaskCreate(vTask1, "Task1", mainGENERIC_STACK_SIZE *2, NULL,
                        mainGENERIC_PRIORITY, &Task1) != pdPASS) {
            PRINT_TASK_ERROR("Task1");
            goto err_task1;
        }

    if (xTaskCreate(vTask2, "Task2", mainGENERIC_STACK_SIZE *2, NULL,
                        mainGENERIC_PRIORITY, &Task2) != pdPASS) {
            PRINT_TASK_ERROR("Task2");
            goto err_task2;
        }
    
    if (xTaskCreate(vSwapBuffers, "BufferSwapTask",
                    mainGENERIC_STACK_SIZE * 2, NULL, configMAX_PRIORITIES,
                    &BufferSwap) != pdPASS) {
        PRINT_TASK_ERROR("BufferSwapTask");
        goto err_bufferswap;
    }

    if (xTaskCreate(vCheckInputTask, "CheckInputTask",
                    mainGENERIC_STACK_SIZE * 2, NULL, mainGENERIC_PRIORITY,
                    &CheckInputTask) != pdPASS){
        PRINT_TASK_ERROR("CheckInputTask");
        goto err_check_input_task;
                    }



    //gfxFUtilPrintTaskStateList();
    
    vTaskStartScheduler();
   
    return EXIT_SUCCESS;

err_check_input_task:
    vTaskDelete(BufferSwap);
err_bufferswap:
    vTaskDelete(Task2);
err_task2:
    vTaskDelete(Task1);
err_task1:
    vTaskDelete(StateMachine);
err_statemachinetask:
    vSemaphoreDelete(ScreenLock);
err_screen_lock:
    vSemaphoreDelete(DrawSignal);
err_draw_signal:
    vButtonsExit();
err_buttons_lock:
    gfxSafePrintExit();
err_init_safe_print:
    gfxSoundExit();
err_init_audio:
    gfxEventExit();
err_init_events:
    gfxDrawExit();
err_init_drawing:
    return EXIT_FAILURE;
}

// cppcheck-suppress unusedFunction
__attribute__((unused)) void vMainQueueSendPassed(void)
{
    /* This is just an example implementation of the "queue send" trace hook. */
}

// cppcheck-suppress unusedFunction
__attribute__((unused)) void vApplicationIdleHook(void)
{
#ifdef __GCC_POSIX__
    struct timespec xTimeToSleep, xTimeSlept;
    /* Makes the process more agreeable when using the Posix simulator. */
    xTimeToSleep.tv_sec = 1;
    xTimeToSleep.tv_nsec = 0;
    nanosleep(&xTimeToSleep, &xTimeSlept);
#endif
}


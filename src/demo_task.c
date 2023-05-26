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


TaskHandle_t Task1 = NULL;

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
            if(xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
                pdTRUE){
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

                    prevWakeTime = xLastWakeTime;   
                }
    }
}


void vStateTwoEnter(void)
{
    //vTaskResume(Task2);
}

void vStateTwoExit(void)
{
    //vTaskSuspend(Task2);
}

void vTask2(void *pvParameters)
{
    while(1){
        if(DrawSignal)
            if(xSemaphoreTake(DrawSignal, portMAX_DELAY) ==
                pdTRUE){

                    gfxDrawClear(White);
                

                    writeStaticText();

                    writeMouseCoord();

            
                }
    }
}
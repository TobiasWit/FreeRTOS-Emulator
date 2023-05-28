#include <stdio.h>
#include <math.h>

#include "main.h"
#include "moving_object.h"
#include "gfx_font.h"
#include "gfx_draw.h"
#include "gfx_event.h"
#include "buttons.h"

#include "state_machine.h"
#include "buttons_count.h"




circle_moving_t circle_moving1 = {0, 0, 40, Red};
rect_moving_t rect_moving1 = {0, 0, 80, 80, Green};
coord_t triangle1[] = {{SCREEN_WIDTH/2, SCREEN_HEIGHT/2-40}, {SCREEN_WIDTH/2-45,
        SCREEN_HEIGHT/2+40}, {SCREEN_WIDTH/2+45, SCREEN_HEIGHT/2+40}};
text_moving_t text_moving1 = { 0, 0, {0}, 0, 0, Orange, 100, 0 };

typedef struct circular_path {
    signed short x;         /** X coordinate of circles center*/
    signed short y;         /** Y coordinate of circles center*/
    signed short radius;    /** Radius of the circle*/
    double angle_position;  /** Angle of the moving object to the center (rad)*/
    float speed_rotation;   /** Rotational speed of the moving objects on the circular path
                                (rad/sec)*/
} circular_path_t;
circular_path_t circular_path1 ={SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_HEIGHT/3, 0, 1};


void updateCirclePosition(circle_moving_t *circle_moving, unsigned milli_seconds)
{
    float update_intervall = milli_seconds / 1000.0;

    circular_path1.angle_position  +=  circular_path1.speed_rotation  *  update_intervall;
    if(circular_path1.angle_position >= 2 * 3.1416){
        circular_path1.angle_position -= 2 * 3.1416;
    }

    circle_moving->x =  circular_path1.x + 
                        circular_path1.radius * cos(circular_path1.angle_position);
    circle_moving->y =  circular_path1.y + 
                        circular_path1.radius * sin(circular_path1.angle_position);
}

void updateRectPosition(rect_moving_t *rect_moving, unsigned milli_seconds)
{
    float update_intervall = milli_seconds / 1000.0;

    circular_path1.angle_position  +=  circular_path1.speed_rotation  *  update_intervall;
    if(circular_path1.angle_position >= 2 * 3.1416){
        circular_path1.angle_position -= 2 * 3.1416;
    }

    rect_moving->x =    circular_path1.x - 
                        circular_path1.radius * cos(circular_path1.angle_position) -
                        rect_moving->w / 2;
    rect_moving->y =    circular_path1.y - 
                        circular_path1.radius * sin(circular_path1.angle_position) -
                        rect_moving->h / 2;
}

void writeStaticText(void)
{
    static char str[50] = { 0 };
    static int text_width;
    static int text_height;

    gfxFontSetSize((ssize_t)30);

    sprintf(str, "This is exercise 2.");

    if (!gfxGetTextSize((char *)str, &text_width, &text_height))
        gfxDrawText(str, (SCREEN_WIDTH - text_width)/2,
                                SCREEN_HEIGHT - text_height * 1.2 , Black);
}

void updateMovingTextPosition(text_moving_t *text_moving, unsigned milli_seconds)
{
    float update_intervall = milli_seconds / 1000.0;
    text_moving->y = text_moving->h * 0.5;
 
    if(text_moving->direction){
        text_moving->x += text_moving->speed * update_intervall;
        if(text_moving->x + text_moving->w >= SCREEN_WIDTH){
            text_moving->direction = 0;
            text_moving->x = SCREEN_WIDTH - text_moving->w;
        }
    }else{
        text_moving->x -= text_moving->speed * update_intervall;
        if(text_moving->x <= 0){
            text_moving->direction = 1;
            text_moving->x = 0;
        }
    }
}

void writeMovingText(text_moving_t *text_moving, TickType_t xLastWakeTime,
            TickType_t prevWakeTime)
{
    gfxFontSetSize((ssize_t)20);
    sprintf(text_moving->str, "Random moving Text!");

    if(!gfxGetTextSize((char *)text_moving->str, &text_moving->w, &text_moving->h)){
        updateMovingTextPosition(text_moving, xLastWakeTime - prevWakeTime);
        gfxDrawText(text_moving->str, text_moving->x, text_moving->y, text_moving->colour);
    }   
}

void writeMouseCoord()
{
    static char str[50] = { 0 };
    static int text_width;
    static int text_height;   

    
        gfxFontSetSize((ssize_t)20);
       
        sprintf(str, "X-coordinate: %hd", gfxEventGetMouseX());
        if (!gfxGetTextSize((char *)str, &text_width, &text_height))
            gfxDrawText(str, (SCREEN_WIDTH - text_width-10),
                                    SCREEN_HEIGHT/2 - text_height  , Black);

        sprintf(str, "Y-coordinate: %hd", gfxEventGetMouseY());
        if (!gfxGetTextSize((char *)str, &text_width, &text_height))
            gfxDrawText(str, (SCREEN_WIDTH - text_width-10),
                                    SCREEN_HEIGHT/2, Black);
    
}

void moveScreenInMouseDirection()
{
    gfxDrawSetGlobalXOffset((gfxEventGetMouseX() - SCREEN_WIDTH/2)/1.7);
    gfxDrawSetGlobalYOffset((gfxEventGetMouseY() - SCREEN_HEIGHT/2)/1.7);
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
#include <stdio.h>
#include "check_input.h"

#include "gfx_event.h"
#include "gfx_ball.h"
#include "gfx_sound.h"
#include "gfx_utils.h"
#include "gfx_print.h"
#include "gfx_draw.h"
#include "gfx_font.h"

#include "task.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "main.h"
#include "circle_blinking_display.h"
#include "buttons.h"
#include "state_machine.h"
#include "draw.h"
#include "moving_object.h"

#include <math.h>

typedef struct buttons_count{
    int a;
    int b;
    int c;
    int d;
} buttons_count_t;
buttons_count_t buttons_count1 = {0};


void writePressedButtonsCount(void)
{
    static char str[300] = { 0 };
    static int text_width;
    static int text_height;

    gfxFontSetSize((ssize_t)20);
    for(int i=0; i<=3; i++){
        char letter;
        int help_count;
        switch (i) {
            case 0:
                letter = 'A';
                help_count = buttons_count1.a;
                break;
            case 1:
                letter = 'B';
                help_count = buttons_count1.b;
                break;
            case 2:
                letter = 'C';
                help_count = buttons_count1.c;
                break;
            case 3:
                letter = 'D';
                help_count = buttons_count1.d;
                break;
        }   
        sprintf(str, "Button %c pressed %d times.", letter, help_count);
        if (!gfxGetTextSize((char *)str, &text_width, &text_height)){
            gfxDrawText(str, 10, SCREEN_HEIGHT/2 - text_height*(2-i) , Black);
        }
    }
}

void updatePressedButtonsCount(unsigned char keycode)
{
    if (keycode == KEYCODE(A)){
        buttons_count1.a ++;
    }
    else if (keycode == KEYCODE(B)){
        buttons_count1.b ++;
    }
    else if (keycode == KEYCODE(C)){
        buttons_count1.c ++;
    }
    else if (keycode == KEYCODE(D)){
        buttons_count1.d ++;
    }

}

void resetPressedButtonsCount(void)
{
    buttons_count1.a = 0;
    buttons_count1.b = 0;
    buttons_count1.c = 0;
    buttons_count1.d = 0;
}




int checkButtonPress(unsigned char keycode, TickType_t *last_pressed, TickType_t *debounce_delay)
{
    if (buttons.buttons[keycode]){
        if (xTaskGetTickCount() - *last_pressed >= *debounce_delay){
            *last_pressed = xTaskGetTickCount();
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        *last_pressed = 0;
        return 0;
    }
}


void vCheckInputTaskStateOne(void *pvParameters)
{
    static TickType_t last_pressed_A, last_pressed_B, last_pressed_C, last_pressed_D;
    static TickType_t debounce_delay = 300;

    
    while(1){

        vGetButtonInput();

        if (gfxEventGetMouseLeft()){
                    resetPressedButtonsCount();
        }

        if (xSemaphoreTake(buttons.lock, 0) == pdTRUE){

            if (checkButtonPress(KEYCODE(A), &last_pressed_A, &debounce_delay)){
                updatePressedButtonsCount(KEYCODE(A));
            }

            if (checkButtonPress(KEYCODE(B), &last_pressed_B, &debounce_delay)){
                updatePressedButtonsCount(KEYCODE(B));
            }

            if (checkButtonPress(KEYCODE(C), &last_pressed_C, &debounce_delay)){
                updatePressedButtonsCount(KEYCODE(C));
            }

            if (checkButtonPress(KEYCODE(D), &last_pressed_D, &debounce_delay)){
                updatePressedButtonsCount(KEYCODE(D));
            }

            xSemaphoreGive(buttons.lock);
    }
        vTaskDelay(10);
    }
        
}


void vCheckInputTaskStateTwo(void *pvParameters){
    
    
    static TickType_t last_pressed_T, last_pressed_R, last_pressed_S;
    static TickType_t debounce_delay = 300;

    while(1){
        vGetButtonInput();


        if (xSemaphoreTake(buttons.lock, 0) == pdTRUE){

            if (checkButtonPress(KEYCODE(T), &last_pressed_T, &debounce_delay)){     
                xTaskNotify(NotifyButtonPressTask, 0x01, eSetBits);
            }

            if (checkButtonPress(KEYCODE(R), &last_pressed_R, &debounce_delay)){
                xSemaphoreGive(ButtonPressR);
            }

            if (checkButtonPress(KEYCODE(S), &last_pressed_S, &debounce_delay)){
                if (eSuspended == eTaskGetState(SecondsCounterTask)){
                    vTaskResume(SecondsCounterTask);
                } else if(eReady == eTaskGetState(SecondsCounterTask)){
                    vTaskSuspend(SecondsCounterTask);
                }
            }

            xSemaphoreGive(buttons.lock);
        }
        vTaskDelay(10);

    
    }
}
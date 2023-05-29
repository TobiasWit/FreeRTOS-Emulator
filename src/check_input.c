#include <stdio.h>
#include "check_input.h"

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
#include "circle_blinking_display.h"
#include "buttons.h"
#include "state_machine.h"
#include "draw.h"
#include "moving_object.h"
#include "buttons_count.h"

#include <math.h>

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


void vCheckInputTask(void *pvParameters){
    
    
    static TickType_t last_pressed_T, last_pressed_Z;
    static TickType_t debounce_delay = 300;

    while(1){
        vGetButtonInput();


        if (xSemaphoreTake(buttons.lock, 0) == pdTRUE){

            if (checkButtonPress(KEYCODE(T), &last_pressed_T, &debounce_delay)){
                
                xTaskNotify(NotifyButtonPressTask, 0x01, eSetBits);
                
                //vTaskResume(NotifyButtonPressTask);
                // vTaskSuspend(NotifyButtonPressTask);
            }

            if (checkButtonPress(KEYCODE(Z), &last_pressed_Z, &debounce_delay)){
     
            }

            xSemaphoreGive(buttons.lock);
        }
        vTaskDelay(10);

    
    }
}
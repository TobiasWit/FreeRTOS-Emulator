#ifndef ___CIRCLE_BLINKING_DISPLAY_H__
#define ___CIRCLE_BLINKING_DISPLAY_H__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

// extern TaskHandle_t Task1;
extern TaskHandle_t CircleBlinkingDynamicTask;
extern TaskHandle_t CircleBlinkingStaticTask;
extern TaskHandle_t CircleBlinkingDisplay;
extern TaskHandle_t NotifyButtonPressTask;
extern TaskHandle_t SemaphoreButtonPressTask;
extern TaskHandle_t ResetButtonPressTRTask;
extern TaskHandle_t SecondsCounterTask;
extern TaskHandle_t CheckInputTaskStateTwo;

extern SemaphoreHandle_t ButtonPressR;

extern TimerHandle_t ResetButtonCountTRTimer;


typedef struct button_press_tr {
    unsigned char value[2];
    SemaphoreHandle_t lock;
} button_press_tr_t;

typedef struct seconds_counter {
    int value;
    SemaphoreHandle_t lock;
} seconds_counter_t;


/// @brief Structure to be send via UDP, important is that
/// that the structure is packed using __attribute__((__packed__))
struct __attribute__((__packed__)) common_struct {
    int first_int;
    int second_int;
};

/// @brief Creates the demo tasks found in demo_tasks.c
/// @return 0 on success
int xCreateCircleBlinkingDisplayTasks(void);

/// @brief Deletes the demo tasks found in demo_tasks.c
void vDeleteCircleBlinkingDisplayTasks(void);

void resetButtonCountTRCallback(TimerHandle_t timer);

#endif // __DEMO_TASKS_H__
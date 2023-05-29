#ifndef ___CIRCLE_BLINKING_DISPLAY_H__
#define ___CIRCLE_BLINKING_DISPLAY_H__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// extern TaskHandle_t Task1;
extern TaskHandle_t CircleBlinkingDynamicTask;
extern TaskHandle_t CircleBlinkingStaticTask;
extern TaskHandle_t CircleBlinkingDisplay;
extern TaskHandle_t NotifyButtonPressTask;


typedef struct button_press_tz {
    unsigned char value[2];
    SemaphoreHandle_t lock;
} button_press_tz_t;


/// @brief Structure to be send via UDP, important is that
/// that the structure is packed using __attribute__((__packed__))
struct __attribute__((__packed__)) common_struct {
    int first_int;
    int second_int;
};


/// @brief Creates the demo tasks found in demo_tasks.c
/// @return 0 on success
int xCreateDemoTask(void);

/// @brief Deletes the demo tasks found in demo_tasks.c
void vDeleteDemoTask(void);

// /// @brief Enter function for state one of the state machine
// void vStateOneEnter(void);

// /// @brief Exit function for state one of the state machine
// void vStateOneExit(void);

/// @brief Init function for state two of the state machine
//void vStateTwoInit(void);

/// @brief Enter function for state two of the state machine
void vStateTwoEnter(void);

/// @brief Exit function for state two of the state machine
void vStateTwoExit(void);

// /// @brief Enter function for state three of the state machine
// void vStateThreeEnter(void);

// /// @brief Exit function for state three of the state machine
// void vStateThreeExit(void);



#endif // __DEMO_TASKS_H__
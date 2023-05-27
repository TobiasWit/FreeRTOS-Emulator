#ifndef __DEMO_TASKS_H__
#define __DEMO_TASKS_H__

#include "FreeRTOS.h"
#include "task.h"



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

/// @brief Enter function for state one of the state machine
void vStateOneEnter(void);

/// @brief Exit function for state one of the state machine
void vStateOneExit(void);

/// @brief Init function for state two of the state machine
//void vStateTwoInit(void);

/// @brief Enter function for state two of the state machine
void vStateTwoEnter(void);

/// @brief Exit function for state two of the state machine
void vStateTwoExit(void);

void vTask1(void *pvParameters);

void vTask2(void *pvParameters);






#endif // __DEMO_TASKS_H__
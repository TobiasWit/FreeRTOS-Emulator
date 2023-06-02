#ifndef ___SCHEDULING_PRIORITIES_TEST_H__
#define ___SCHEDULING_PRIORITIES_TEST_H__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


// extern TaskHandle_t DrawScreenExercise4;
extern TaskHandle_t Task1;
extern TaskHandle_t Task2;
extern TaskHandle_t Task3;
extern TaskHandle_t Task4;
extern TaskHandle_t OutputTask;

extern SemaphoreHandle_t WakeTask3;

extern QueueHandle_t PrintNumbersQueue;

extern TickType_t StartingWakeTime;

/**
 * @brief struct, which has a value and the coorinates, where the value will be drawn on the screen
*/
typedef struct coord_number_tupel{
    coord_t coord;
    int value;
} coord_number_tupel_t;

/**
 * @brief array, each element counts how many outputs there are per tick 
*/
extern signed short count_array[15];
/**
 * @brief array of coord_number_tupel type
*/
extern coord_number_tupel_t coord_number[50];

/**
 * @brief struct that saves a value and a corresponding tick
*/
typedef struct number {
    int value;
    TickType_t tick;
} number_t;

/**
 * @brief creates all needed tasks for state three
 * @return 0 on succes
*/
int xCreateSchedulingPrioritiesTestTasks(void);

/**
 * @brief deletes all needed tasks of state three
*/
void vDeleteSchedulingPrioritiesTestTasks(void);


#endif
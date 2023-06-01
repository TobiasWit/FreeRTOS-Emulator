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
extern SemaphoreHandle_t StartingState3;

extern QueueHandle_t PrintNumbersQueue;

extern TickType_t StartingWakeTime;


typedef struct coord_number_tupel{
    coord_t coord;
    int value;
} coord_number_tupel_t;

extern signed short count_array[15];
extern coord_number_tupel_t coord_number[50];


typedef struct number {
    int value;
    TickType_t tick;
} number_t;



int xCreateSchedulingPrioritiesTestTasks(void);

void vDeleteSchedulingPrioritiesTestTasks(void);


#endif
#ifndef ___SCHEDULING_PRIORITIES_TEST_H__
#define ___SCHEDULING_PRIORITIES_TEST_H__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


extern TaskHandle_t DrawScreenExercise4;



int xCreateSchedulingPrioritiesTestTasks(void);

void vDeleteSchedulingPrioritiesTestTasks(void);


#endif
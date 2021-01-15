/*
 * task_wrapper.h
 *
 *  Created on: 11 Jan 2021
 *      Author: Jakub Mnich
 */

#ifndef INC_TASK_WRAPPER_H_
#define INC_TASK_WRAPPER_H_

#include "FreeRTOS.h"
#include "task.h"

class TaskClass {
protected:
	xTaskHandle taskHandle_;

public:
	TaskClass(char const * _name, unsigned portBASE_TYPE _priority, unsigned portSHORT _stackDepth = configMINIMAL_STACK_SIZE);
	~TaskClass();

	virtual void task() = 0;
	static void taskFunction(void* _param);
};



#endif /* INC_TASK_WRAPPER_H_ */

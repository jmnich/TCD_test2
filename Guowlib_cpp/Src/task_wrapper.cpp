/*
 * task_wrapper.cpp
 *
 *  Created on: 11 Jan 2021
 *      Author: Jakub Mnich
 */

#include "task_wrapper.h"

TaskClass::TaskClass(char const * _name, unsigned portBASE_TYPE _priority, unsigned portSHORT _stackDepth) {

	/*
	 * FreeRTOS task is created normally and receives a reference to this object
	 * as parameters in the argument. This ensures that all relevant information
	 * that would normally be passes this way is already contained in the wrapper
	 * object.
	 */
	xTaskCreate(taskFunction, _name, _stackDepth, this, _priority, &(this->taskHandle_));
}

TaskClass::~TaskClass() { }

void TaskClass::taskFunction(void* _param) {

	/*
	 * In order for the function pointer to be callable from FreeRTOS is must lose
	 * its 'this' pointer (become static). Therefore 'taskFunction' is needed to
	 * act as a trampolin for the virtual 'task' function implemented by a child
	 * class.
	 */
	TaskClass * p = static_cast<TaskClass *>(_param);
	p->task();
}



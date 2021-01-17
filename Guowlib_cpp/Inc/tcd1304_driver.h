/*
 * tcd1304_driver.h
 *
 *  Created on: 17 Jan 2021
 *      Author: Jakub Mnich
 */

#ifndef INC_TCD1304_DRIVER_H_
#define INC_TCD1304_DRIVER_H_

#include "stm32g4xx_hal.h"
#include "observable.h"
#include "task_wrapper.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "queue.h"
//#include "semphr.h"

class TCD1304Driver : public TaskClass, public Observable {

private:
	TIM_HandleTypeDef * masterClock_;
	uint32_t * outputBuffer_;
	volatile bool operationInProgress_;
	const int sensingElementsCount_ = 3648;

	void configureTimers();

public:
	TCD1304Driver(TIM_HandleTypeDef * _masterClock, uint32_t * _bufPtr, void(**_subscribersArray)(Observable*, void*),
			int _observersArraySize);
	~TCD1304Driver();

	void task() override;

	void startOperation();
	void stopOperation();
	bool isOperationInProgress();

};



#endif /* INC_TCD1304_DRIVER_H_ */

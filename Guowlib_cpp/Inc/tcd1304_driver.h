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
	TIM_HandleTypeDef * masterTimer_;
	TIM_HandleTypeDef * icgTimer_;
	TIM_HandleTypeDef * shTimer_;
	TIM_HandleTypeDef * adcTrigTimer_;
	ADC_HandleTypeDef * outputADC_;
	uint32_t * outputBuffer_;
	volatile bool operationInProgress_;
	const int sensingElementsCount_ = 3648;
	const int dummyElementsFront_ = 32;
	const int dummyElementsBack_ = 14;

	void configureTimers();

public:
	TCD1304Driver(TIM_HandleTypeDef * _masterTimer, TIM_HandleTypeDef * _icgTimer, TIM_HandleTypeDef * _shTimer,
			TIM_HandleTypeDef * _adcTrigTimer, uint32_t * _bufPtr, void(**_subscribersArray)(Observable*, void*),
			int _observersArraySize, ADC_HandleTypeDef * _outputADC);
	~TCD1304Driver();

	void initialize();
	void task() override;

	void startOperation();
	void stopOperation();
	bool isOperationInProgress();
	void ICGPulseCallback();

};



#endif /* INC_TCD1304_DRIVER_H_ */

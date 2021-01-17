/*
 * tcd1304_driver.cpp
 *
 *  Created on: 17 Jan 2021
 *      Author: Jakub Mnich
 */

#include "tcd1304_driver.h"

TCD1304Driver::TCD1304Driver(TIM_HandleTypeDef * _masterTimer, TIM_HandleTypeDef * _icgTimer, TIM_HandleTypeDef * _shTimer,
		uint32_t * _bufPtr, void(**_subscribersArray)(Observable*, void*), int _observersArraySize) :
		TaskClass("tcdDrv", 2), Observable(_subscribersArray, _observersArraySize)
{
	masterTimer_ = _masterTimer;
	icgTimer_ = _icgTimer;
	shTimer_ = _shTimer;

	outputBuffer_ = _bufPtr;
}

TCD1304Driver::~TCD1304Driver()
{
	// TODO
}

// ========== PUBLIC FUNCTIONS ==========

void TCD1304Driver::initialize()
{
	HAL_TIM_OC_Init(masterTimer_);
	HAL_TIM_OC_Start(masterTimer_, TIM_CHANNEL_1);

	startOperation();
}

void TCD1304Driver::task()
{
	while(1) {
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void TCD1304Driver::startOperation()
{
//	uint32_t icg_arr = icgTimer_->Instance->ARR;
//	uint32_t icg_pls = icgTimer_->Instance->CCR1;

	/*
	 * total read-out time for mclk = 2MHz is equal to 7,388ms
	 * so minimal ARR for icg timer in PWM mode 2 is = 14 776
	 *
	 * sh timer period is "integration time"
	 * also sh pulse must be shorter than the icg pulse (by at least 5 us)
	 * ALSO sh pulse must arrive between 100 and 1000ns after icg falling edge
	 */

	// ICG timer
	icgTimer_->Init.Prescaler = 79;
	icgTimer_->Init.Period = 14999;
	HAL_TIM_Base_Init(icgTimer_);

	TIM_OC_InitTypeDef sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = 100;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(icgTimer_, &sConfigOC, TIM_CHANNEL_1);

	// SH timer
	shTimer_->Init.Prescaler = 79;
	shTimer_->Init.Period = 1499;
	HAL_TIM_Base_Init(shTimer_);

	sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 30;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(shTimer_, &sConfigOC, TIM_CHANNEL_1);

//	icgTimer_->Instance->PSC = 79;
//	icgTimer_->Instance->ARR = 14999;
//	icgTimer_->Instance->CCR1 = 100;
//
//	shTimer_->Instance->PSC = 79;
//	shTimer_->Instance->ARR = 99;
//	shTimer_->Instance->CCR1 = 15;

	HAL_TIM_PWM_Init(icgTimer_);
	HAL_TIM_PWM_Init(shTimer_);

	HAL_TIM_PWM_Start(icgTimer_, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(shTimer_, TIM_CHANNEL_1);
}

void TCD1304Driver::stopOperation()
{
	// TODO
}

bool TCD1304Driver::isOperationInProgress()
{
	// TODO
	return false;
}

// ========== PRIVATE FUNCTIONS ==========

void TCD1304Driver::configureTimers()
{

}

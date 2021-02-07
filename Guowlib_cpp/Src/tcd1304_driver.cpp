/*
 * tcd1304_driver.cpp
 *
 *  Created on: 17 Jan 2021
 *      Author: Jakub Mnich
 */

#include "tcd1304_driver.h"
#include "main.h"

TCD1304Driver::TCD1304Driver(TIM_HandleTypeDef * _masterTimer, TIM_HandleTypeDef * _icgTimer, TIM_HandleTypeDef * _shTimer,
		TIM_HandleTypeDef * _adcTrigTimer, uint16_t * _bufPtr, void(**_subscribersArray)(Observable*, void*),
		int _observersArraySize, ADC_HandleTypeDef * _outputADC) :
		TaskClass("tcdDrv", 2), Observable(_subscribersArray, _observersArraySize)
{
	masterTimer_ = _masterTimer;
	icgTimer_ = _icgTimer;
	shTimer_ = _shTimer;
	adcTrigTimer_ = _adcTrigTimer;

	outputADC_ = _outputADC;
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

	HAL_TIM_OC_Init(adcTrigTimer_);
	HAL_TIM_OC_Start_IT(adcTrigTimer_, TIM_CHANNEL_1);

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
	icgTimer_->Init.Prescaler = 159;
	icgTimer_->Init.Period = 99999;
	HAL_TIM_Base_Init(icgTimer_);

	TIM_OC_InitTypeDef sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = 100;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(icgTimer_, &sConfigOC, TIM_CHANNEL_1);

	// SH timer
	shTimer_->Init.Prescaler = 159;
	shTimer_->Init.Period = 99;
	HAL_TIM_Base_Init(shTimer_);

	sConfigOC = {0};
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 60;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(shTimer_, &sConfigOC, TIM_CHANNEL_1);

	HAL_TIM_PWM_Init(icgTimer_);
	HAL_TIM_PWM_Init(shTimer_);

	HAL_TIM_PWM_Start_IT(icgTimer_, TIM_CHANNEL_1);
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

void TCD1304Driver::ICGPulseCallback()
{
	HAL_ADC_Init(outputADC_);
	HAL_ADC_Start_DMA(outputADC_, (uint32_t *)outputBuffer_, sensingElementsCount_ + dummyElementsFront_ + dummyElementsBack_);
	HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
}

// ========== PRIVATE FUNCTIONS ==========

void TCD1304Driver::configureTimers()
{

}

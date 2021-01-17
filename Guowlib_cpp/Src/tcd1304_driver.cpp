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
}

void TCD1304Driver::task()
{
	// TODO
}

void TCD1304Driver::startOperation()
{
	  HAL_TIM_OC_Init(icgTimer_);
	  HAL_TIM_OC_Start(icgTimer_, TIM_CHANNEL_1);
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

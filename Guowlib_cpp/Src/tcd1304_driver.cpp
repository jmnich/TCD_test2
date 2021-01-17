/*
 * tcd1304_driver.cpp
 *
 *  Created on: 17 Jan 2021
 *      Author: Jakub Mnich
 */

#include "tcd1304_driver.h"

TCD1304Driver::TCD1304Driver(TIM_HandleTypeDef * _masterClock, uint32_t * _bufPtr, void(**_subscribersArray)(Observable*, void*),
		int _observersArraySize) :
		TaskClass("tcdDrv", 2), Observable(_subscribersArray, _observersArraySize)
{
	// TODO
}

TCD1304Driver::~TCD1304Driver()
{
	// TODO
}

// ========== PUBLIC FUNCTIONS ==========

void TCD1304Driver::task()
{
	// TODO
}

void TCD1304Driver::startOperation()
{
	// TODO
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

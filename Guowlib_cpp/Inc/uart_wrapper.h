#ifndef _UART_WRAPPER_H
#define _UART_WRAPPER_H

#include "stm32g4xx_hal.h"
#include "observable.h"
#include "task_wrapper.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

class UART_Wrapper : public TaskClass, public Observable {

private:
	UART_HandleTypeDef * UART_Handle_;
	volatile uint8_t * rxBufPtr_;
	volatile uint32_t rxBufSize_;

	volatile uint8_t * rxBufHead_;
	volatile uint8_t * rxBufTail_;

	SemaphoreHandle_t messagesInRxBufferSemaphore_;

	uint8_t * getNextFreeRxPositionInBuf();
	static void UART_RxISR_8BIT(UART_HandleTypeDef *huart);

public:
	UART_Wrapper(UART_HandleTypeDef * _UART_Handle, uint8_t * _bufPtr, uint32_t _bufSize,
			void(**_subscribersArray)(Observable*, void*), int _observersArraySize);
	~UART_Wrapper();
	void task() override;
	void sendString(const char* _s);
	void sendBytes(const uint8_t * _b, uint32_t _size);
	void sendByte(uint8_t _b);
	void startRx();
	void stopRx();
	void rxCallback();
};

#endif // _UART_WRAPPER_H

#include "uart_wrapper.h"

UART_Wrapper::UART_Wrapper(UART_HandleTypeDef * _UART_Handle, uint8_t * _bufPtr, uint32_t _bufSize,
		void(**_subscribersArray)(Observable*, void*), int _observersArraySize) :
		TaskClass("uartRx", 2), Observable(_subscribersArray, _observersArraySize)
{

	this->UART_Handle_ = _UART_Handle;
	this->rxBufPtr_ = _bufPtr;
	this->rxBufSize_ = _bufSize;

	this->rxBufHead_ = rxBufPtr_;
	this->rxBufTail_ = rxBufPtr_;

	this->messagesInRxBufferSemaphore_ = xQueueCreateCountingSemaphore(16, 0);

	// zero the rx buffer
	for(uint32_t i = 0; i < rxBufSize_; i++) {
		*(_bufPtr++) = 0;
	}
}

UART_Wrapper::~UART_Wrapper() {

	// zero the rx buffer
	for(uint32_t i = 0; i < rxBufSize_; i++) {
		*(rxBufPtr_++) = 0;
	}
}

// ========== PUBLIC FUNCTIONS ==========

void UART_Wrapper::task() {

	while(1) {
		xSemaphoreTake(this->messagesInRxBufferSemaphore_, portMAX_DELAY);

		// handle a message in the rx buffer
		// 1. check message size
		uint32_t size = 0;
		uint8_t * dummyPtr = (uint8_t *)this->rxBufTail_;
		while(*dummyPtr) {
			size++;
			dummyPtr++;

			// loop the pointer when it hits the end of the buffer
			if(dummyPtr > rxBufPtr_ + rxBufSize_) {
				dummyPtr = (uint8_t *)rxBufPtr_;
			}
		}

		// 2. allocate memory to hold the whole message
		char * s = new char[size + 1];

		// 3. copy data from the ring buffer to the message buffer
		for(uint32_t i = 0; i < size; i++) {
			s[i] = *rxBufTail_;
			*rxBufTail_ = 0;
			rxBufTail_++;

			if(rxBufTail_ > rxBufPtr_ + rxBufSize_)
				rxBufTail_ = rxBufPtr_;
		}

		// 4. terminate the message buffer
		s[size] = 0;

		// 5. increment the tail - head points at the first free byte and tail must catch up now
		rxBufTail_++;

		this->dataForUpdate = (void*)s;
		notify();
	}
}

void UART_Wrapper::sendString(const char* _s) {

	while(*_s) {
		HAL_UART_Transmit(UART_Handle_, (uint8_t*)(_s)++, 1, 1);
	}
}

void UART_Wrapper::sendBytes(const uint8_t * _b, uint32_t _size) {
	HAL_UART_Transmit(UART_Handle_, (uint8_t *)_b, _size, 1000);
}

void UART_Wrapper::sendByte(uint8_t _b) {
	HAL_UART_Transmit(UART_Handle_, &_b, 1, 1);
}

void UART_Wrapper::sendLargeBufferDMA(uint8_t * _buf, uint32_t _size)
{
	HAL_UART_Transmit_DMA(UART_Handle_, _buf, _size);
}

void UART_Wrapper::startRx() {
	/* Check that a Rx process is not already ongoing */
	if (UART_Handle_->RxState == HAL_UART_STATE_READY)
	{
		UART_Handle_->pRxBuffPtr  = NULL;
		UART_Handle_->RxXferSize  = 0;
		UART_Handle_->RxXferCount = 0;
		UART_Handle_->RxISR       = UART_Wrapper::UART_RxISR_8BIT;

		/* Computation of UART mask to apply to RDR register */
		UART_MASK_COMPUTATION(UART_Handle_);

		UART_Handle_->ErrorCode = HAL_UART_ERROR_NONE;
		UART_Handle_->RxState = HAL_UART_STATE_BUSY_RX;

		/* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
		SET_BIT(UART_Handle_->Instance->CR3, USART_CR3_EIE);
		SET_BIT(UART_Handle_->Instance->CR1, USART_CR1_PEIE);
		SET_BIT(UART_Handle_->Instance->CR3, USART_CR3_RXFTIE);
	}
}

void UART_Wrapper::stopRx() {
	HAL_UART_AbortReceive_IT(UART_Handle_);
}

void UART_Wrapper::rxCallback() {

	uint16_t uhMask = UART_Handle_->Mask;
	uint16_t  uhdata;

	uint8_t newLineCounter = 0;

	/* Check that a Rx process is ongoing */
	if (UART_Handle_->RxState == HAL_UART_STATE_BUSY_RX)
	{

		// make sure all received bytes are extracted from FIFO
		while(UART_Handle_->Instance->ISR & UART_FLAG_RXFNE) {
			uhdata = (uint16_t) READ_REG(UART_Handle_->Instance->RDR);

			*(this->getNextFreeRxPositionInBuf()) = (uint8_t)(uhdata & (uint8_t)uhMask);
			UART_Handle_->pRxBuffPtr++;
			UART_Handle_->RxXferCount--;

			if((uint8_t)(uhdata & (uint8_t)uhMask) == '\n') {
				newLineCounter++;
				// terminate the full line in the buffer with '0'
				*(this->getNextFreeRxPositionInBuf()) = 0x0;
			}

		}

		if (UART_Handle_->RxXferCount == 0U)
		{
			/* Disable the UART Parity Error Interrupt and RXNE interrupts */
			CLEAR_BIT(UART_Handle_->Instance->CR1, (USART_CR1_RXNEIE_RXFNEIE | USART_CR1_PEIE));

			/* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
			CLEAR_BIT(UART_Handle_->Instance->CR3, USART_CR3_EIE);

			/* Rx process is completed, restore huart->RxState to Ready */
			UART_Handle_->RxState = HAL_UART_STATE_READY;

			/* Clear RxISR function pointer */
			UART_Handle_->RxISR = NULL;
		}
	}
	else
	{
		/* Clear RXNE interrupt flag */
		__HAL_UART_SEND_REQ(UART_Handle_, UART_RXDATA_FLUSH_REQUEST);
	}

	// start listening again
	startRx();

	// if new line signs were located in the message incerement the couting semaphore
	for(uint8_t i = 0; i < newLineCounter; i++) {
		xSemaphoreGiveFromISR(this->messagesInRxBufferSemaphore_, NULL);
	}
}

// ========== PRIVATE FUNCTIONS ==========

uint8_t * UART_Wrapper::getNextFreeRxPositionInBuf() {

	uint8_t * output = (uint8_t *)rxBufHead_;
	rxBufHead_++;

	// the buffer head jumps back to the start if needed
	if(rxBufHead_ > rxBufPtr_ + rxBufSize_) {
		rxBufHead_ = rxBufPtr_;
	}

	if(rxBufHead_ == rxBufTail_) {
		// TODO - buffer overflow
	}

	return output;
}

/*
 * custom ISR routine for handling incoming transmission while retaining some degree of
 * compatibility with the HAL
 */
void UART_Wrapper::UART_RxISR_8BIT(UART_HandleTypeDef *huart) {

	HAL_UART_RxCpltCallback(huart);
}

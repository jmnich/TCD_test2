/*
 * observer.cpp
 *
 *  Created on: Jan 10, 2021
 *      Author: Jakub Mnich
 */

#include <observable.h>

Observable::Observable(void(**_subscribersArray)(Observable*, void*), int _observersArraySize) {
	subscribers_ = _subscribersArray;
	maxSubscribers_ = maxSubscribers_;
	currentSubscribers_ = 0;
	dataForUpdate = 0x0;
}

void Observable::subscribe(void(*_updateFunction)(Observable*, void*)) {

	*(subscribers_ + currentSubscribers_) = _updateFunction;
	currentSubscribers_++;
}

void Observable::unsubscribe(void(*_updateFunction)(Observable*, void*)) {

	int removedSubIndex = -1;

	// locate the sub
	for(int i = 0; i < currentSubscribers_; i++) {

		if(*(subscribers_ + i) == _updateFunction) {
			*(subscribers_ + i) = 0x0;
			removedSubIndex = i;
		}
	}

	// if sub was found shift the rest to left and decrement current subs counter
	if(removedSubIndex != -1) {
		currentSubscribers_--;

		int subsRightFromTheDeleted = currentSubscribers_ - removedSubIndex;

		if(subsRightFromTheDeleted > 0) {

			for(int i = removedSubIndex; i < subsRightFromTheDeleted; i++) {
				*(subscribers_ + i) = *(subscribers_ + i + 1);
			}

			*(subscribers_ + removedSubIndex + subsRightFromTheDeleted) = 0x0;
		}
	}
}

int Observable::getCurrentSubscribersCount() {
	return currentSubscribers_;
}

int Observable::getMaxSubscribersCount() {
	return maxSubscribers_;
}

void Observable::notify() {

	for(int i = 0; i < currentSubscribers_; i++) {
		(*(subscribers_ + i))(this, dataForUpdate);
	}
}

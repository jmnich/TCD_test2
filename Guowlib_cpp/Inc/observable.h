/*
 * observer.h
 *
 *  Created on: Jan 10, 2021
 *      Author: Jakub Mnich
 */

#ifndef INC_OBSERVABLE_H_
#define INC_OBSERVABLE_H_

class Observable {

private:
	void(**subscribers_)(Observable*, void*);
	int maxSubscribers_;
	int currentSubscribers_;

protected:
	void notify();
	void * dataForUpdate;

public:
	Observable(void(**_subscribersArray)(Observable*, void*), int _observersArraySize);
	void subscribe(void(*_updateFunction)(Observable*, void*));
	void unsubscribe(void(*_updateFunction)(Observable*, void*));
	int getCurrentSubscribersCount();
	int getMaxSubscribersCount();
};


#endif /* INC_OBSERVABLE_H_ */

/*
 * c.h
 *
 *  Created on: Feb 2, 2024
 *      Author: MSI
 */

#ifndef TEST_C_C_H_
#define TEST_C_C_H_

#include <core/event.h>
#include <core/signal.h>

COMPONENT(test, C)

	M_EVENT(receiveSensorB, uint16_t)

public:
	void init();

COMPONENT_END

#endif /* TEST_C_C_H_ */

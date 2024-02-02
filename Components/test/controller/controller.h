/*
 * controller.h
 *
 *  Created on: Feb 2, 2024
 *      Author: MSI
 */

#ifndef TEST_CONTROLLER_CONTROLLER_H_
#define TEST_CONTROLLER_CONTROLLER_H_

#include <core/event.h>

COMPONENT(test, Controller)

	M_EVENT(receiveSensorDataA, uint32_t)
	M_EVENT(receiveSensorDataB, uint16_t)

public:
	void init();


COMPONENT_END

#endif /* TEST_CONTROLLER_CONTROLLER_H_ */

/*
 * controller.cpp
 *
 *  Created on: Feb 2, 2024
 *      Author: MSI
 */

#ifndef TEST_CONTROLLER_CONTROLLER_CPP_
#define TEST_CONTROLLER_CONTROLLER_CPP_

#include <test/sensor/sensor.h>
#include "controller.h"
#include <console/log.h>

void test::Controller::init()
{
	test::Sensor::instance().readSensorASignal.connect(&receiveSensorDataAEvent);
	test::Sensor::instance().readSensorBSignal.connect(&receiveSensorDataBEvent);
}

M_EVENT_HANDLER(test::Controller, receiveSensorDataA, uint32_t)
{
	LOG_PRINTF("CTRL: Receive Sensor A data %d", event);
}

M_EVENT_HANDLER(test::Controller, receiveSensorDataB, uint16_t)
{
	LOG_PRINTF("CTRL: Receive Sensor B data %d", event);
}


#endif /* TEST_CONTROLLER_CONTROLLER_CPP_ */

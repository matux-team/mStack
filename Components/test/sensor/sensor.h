/*
 * sensor.h
 *
 *  Created on: Feb 2, 2024
 *      Author: MSI
 */

#ifndef TEST_SENSOR_SENSOR_H_
#define TEST_SENSOR_SENSOR_H_

#include <core/event.h>
#include <core/signal.h>
#include <core/task.h>

COMPONENT(test, Sensor)

	struct Data
	{
		uint32_t dataA;
		uint16_t dataB;
	};

	M_TASK(readSensor)
	M_SIGNAL(readSensorA, uint32_t)
	M_SIGNAL_MANY(readSensorB, uint16_t)

public:
	void init();

private:
	struct Data data_;
COMPONENT_END

#endif /* TEST_SENSOR_SENSOR_H_ */

#include <test/sensor/sensor.h>
#include "c.h"
#include <console/log.h>

void test::C::init()
{
	test::Sensor::instance().readSensorBSignal.connect(&receiveSensorBEvent);
}

M_EVENT_HANDLER(test::C, receiveSensorB, uint16_t)
{
	LOG_PRINTF("C: Receive Sensor B data %d", event);
}

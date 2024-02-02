#include "sensor.h"

void test::Sensor::init()
{
	readSensorTask_.start(3000);
}

M_TASK_HANDLER(test::Sensor, readSensor)
{
	readSensorBSignal.emit(data_.dataB);
	readSensorASignal.emit(data_.dataA, true);

	data_.dataA += 10;
	data_.dataB += 5;
}

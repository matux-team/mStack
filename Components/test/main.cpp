#include <console/controller.h>
#include <test/controller/controller.h>
#include <test/sensor/sensor.h>
#include <test/C/c.h>

int main()
{
	COMPONENT_REG(console, Controller);
	COMPONENT_REG(test, Sensor);
	COMPONENT_REG(test, C);
	COMPONENT_REG(test, Controller);

	core::Engine::instance().run();

	return 0;
}

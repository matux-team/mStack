#include <console/controller.h>
#include <test/example/example.h>

int main()
{
	COMPONENT_REG(ex, Test);
	COMPONENT_REG(console, Controller);

	core::Engine::instance().run();
	return 0;
}



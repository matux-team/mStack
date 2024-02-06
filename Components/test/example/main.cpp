#include <console/controller.h>
#include <test/example/example.h>

int main()
{
	COMPONENT_REG(console, Controller);
	COMPONENT_REG(ex, Test);

	core::Engine::instance().run();
	return 0;
}



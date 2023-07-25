#include "mem-pool.h"
#include <console/controller.h>

int main()
{
	COMPONENT_REG(test, MemPool);
	COMPONENT_REG(console, Controller);

	core::Engine::instance().run();
	return 0;
}



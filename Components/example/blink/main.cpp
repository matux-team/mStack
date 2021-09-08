#include <example/blink/blink.h>
#include <console/controller.h>
#include "core/engine.h"

int main()
{
	COMPONENT_REG(console, Controller);
    COMPONENT_REG(ex, Blink);
    core::Engine::instance().run();
    return 0;
}

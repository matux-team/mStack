#include <console/controller.h>
#include "console.h"

int main()
{
    COMPONENT_REG(console, Controller);
    COMPONENT_REG(test, Console);
    core::Engine::instance().run();
    return 0;
}

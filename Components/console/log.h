#ifndef CONSOLE_LOG_H_
#define CONSOLE_LOG_H_
#include <console/controller.h>

#define LOG_PRINT 	console::Controller::instance().print
#define LOG_PRINTF 	console::Controller::instance().printf

#define DEBUG_PRINT 	console::Controller::instance().print
#define DEBUG_PRINTF 	console::Controller::instance().printf

#endif /* LOG_H_ */

/*
 * mem-pool.h
 *
 *  Created on: Jul 25, 2023
 *      Author: MSI
 */

#ifndef TEST_MEM_POOL_MEM_POOL_H_
#define TEST_MEM_POOL_MEM_POOL_H_

#include <core/engine.h>
#include <core/event.h>
#include <core/task.h>

COMPONENT(test, MemPool)

	M_TASK(timeout)
	M_EVENT(emptyEvent)
	M_EVENT(fixedEvent, uint32_t)

public:
	void init();

private:


COMPONENT_END

#endif /* TEST_MEM_POOL_MEM_POOL_H_ */

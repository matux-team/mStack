#include "mem-pool.h"

void test::MemPool::init()
{
	timeoutTask_.start(1000);
}

M_TASK_HANDLER(test::MemPool, timeout)
{

}

M_EVENT_HANDLER(test::MemPool, emptyEvent)
{

}

M_EVENT_HANDLER(test::MemPool, fixedEvent, uint32_t)
{

}

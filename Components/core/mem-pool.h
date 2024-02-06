/*
 * mem-pool.h
 *
 *  Created on: Jul 25, 2023
 *      Author: MSI
 */

#ifndef CORE_MEM_POOL_H_
#define CORE_MEM_POOL_H_

#include <stdlib.h>
#include <string.h>
#include "main.h"
#include <core/engine.h>

template <typename T>
class MemPool
{
private:
    // The purpose of the structure`s definition is that we can operate linkedlist conveniently
    struct _Unit //The type of the node of linkedlist.
    {
        struct _Unit *pNext = nullptr;
    };

    void* m_pMemBlock = nullptr;                //The address of memory pool.

    // Manage all unit with two linkedlist.
    struct _Unit*    m_pAllocatedMemBlock = nullptr; //Head pointer to Allocated linkedlist.
    struct _Unit*    m_pFreeMemBlock = nullptr;      //Head pointer to Free linkedlist.

public:

    MemPool(unsigned long ulUnitNum):
            m_pMemBlock(nullptr),
            m_pAllocatedMemBlock(nullptr),
            m_pFreeMemBlock(nullptr)//,
    {
    	uint32_t size = ulUnitNum * (sizeof(T) + sizeof(struct _Unit));
        m_pMemBlock = malloc(size); //Allocate a memory block.
        if(m_pMemBlock)
        {
            for(unsigned long i=0; i<ulUnitNum; i++)  //Link all mem unit . Create linked list.
            {
                struct _Unit *pCurUnit = (struct _Unit *)( (uint8_t*)m_pMemBlock + i*(sizeof(T)+sizeof(struct _Unit)) );

                pCurUnit->pNext = m_pFreeMemBlock;    //Insert the new unit at head.

                m_pFreeMemBlock = pCurUnit;
            }
            core::Engine::instance().addNumOfByteHeap(size);
        }
        else
        {
        	Error_Handler();
        }
    }

    ~MemPool()
    {
        free(m_pMemBlock);
    }

     // Allocate memory unit If memory pool can`t provide proper memory unit,
     // It will call system function.
    inline void* Alloc()
    {
        if(m_pFreeMemBlock == nullptr)	// Pool Full
        {
			return nullptr;
        }

        struct _Unit *pCurUnit = m_pFreeMemBlock;
        m_pFreeMemBlock = pCurUnit->pNext;  //Get a unit from free linkedlist.
        pCurUnit->pNext = m_pAllocatedMemBlock;
        m_pAllocatedMemBlock = pCurUnit;

        return (void *)((char *)pCurUnit + sizeof(struct _Unit) );
    }

    inline void Free(void* p)
    {
		struct _Unit *pCurUnit = (struct _Unit *)((char *)p - sizeof(struct _Unit) );
		struct _Unit* prev = nullptr;
		for(struct _Unit* it = m_pAllocatedMemBlock; it!= nullptr; it = it->pNext)
		{
			if(it == pCurUnit)
			{
				if(prev == nullptr) m_pAllocatedMemBlock = it->pNext;
				else prev->pNext = it->pNext;
				pCurUnit->pNext = m_pFreeMemBlock;
				m_pFreeMemBlock = pCurUnit;
				break;
			}
			prev = it;
		}
    }
};

#endif /* CORE_MEM_POOL_H_ */

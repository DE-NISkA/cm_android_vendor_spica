/*
*******************************************************************************
*
*                        SAMSUNG INDIA SOFTWARE OPERATIONS
*                               Copyright(C) 2006
*                              ALL RIGHTS RESERVED
*
* This program is proprietary  to Samsung India  Software Operations Pvt. Ltd.,
* and is protected under International Copyright Act as an unpublished work.Its
* use and disclosure is limited by the terms and conditions of a license agree-
* -ment. It may not be  copied or otherwise  reproduced or disclosed to persons
* outside the licensee's  organization except in accordance  with the terms and
* conditions of  such an agreement. All copies and  reproductions  shall be the
* property of  Samsung  India Software Operations Pvt. Ltd.  and must bear this
* notice in its entirety.
*
*******************************************************************************
*/

/*
***************************************************************************//*!
*
* \file         cpool.cpp
* \author       Prashant Singh (prashant@samsung.com)
* \brief        Chunk pool source (Simple segregated storage implementation)
*
*//*---------------------------------------------------------------------------
* NOTES:
*
*//*---------------------------------------------------------------------------
* HISTORY:
*
*       14.11.2006      Prashant Singh  Draft version
*
*******************************************************************************
*/

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

#include <stddef.h>
#include "cpool.h"

/*
*******************************************************************************
* Macro definitions and enumerations
*******************************************************************************
*/

/*
*******************************************************************************
* Type, Structure & Class Definitions
*******************************************************************************
*/

/*
*******************************************************************************
* Global Variables
*******************************************************************************
*/

/*
*******************************************************************************
* Local Function Declarations
*******************************************************************************
*/

/*
*******************************************************************************
* Function Definitions
*******************************************************************************
*/

/*-----------------------------------------------------------------------*//*!
* Brief.
*
* Detailed description
*
* \param [in] name(Type)                        Description
*
* \retval       void
*
* \sa
*//*------------------------------------------------------------------------*/

ChunkPool::ChunkPool( )
{
        head = NULL;
        num = 0;
        size = -1;
}

/*-----------------------------------------------------------------------*//*!
* Brief.
*
* Detailed description
*
* \param [in] name(Type)                        Description
*
* \retval       void
*
* \sa
*//*------------------------------------------------------------------------*/

ChunkPool::~ChunkPool( )
{
        /* Nothing to do. No use-case requires cleanup */
}


/*-----------------------------------------------------------------------*//*!
* Brief.
*
* Detailed description
*
* \param [in] name(Type)                        Description
*
* \retval       void
*
* \sa
*//*------------------------------------------------------------------------*/

bool
ChunkPool::AddChunks( void* paddr, void* vaddr, int num, int size )
{
        //-------------------------------------------------------------------------
        // Sanity checks
        //-------------------------------------------------------------------------

        if(paddr == NULL || vaddr == NULL || num <= 0 || size <= 0)
                return false;

        // Return error if pool size of newly added chunks is not same as previous
        //  AddChunk
        if(ChunkPool::num != 0 && ChunkPool::size != size)
                return false;

        //-------------------------------------------------------------------------
        // Create free chunk list
        //-------------------------------------------------------------------------

        ChunkNode *node;
        for(int i = 0; i < num; i++) {
                node = new(ChunkNode);
                node->next = head;
                node->paddr = paddr;
                node->vaddr = vaddr;

                paddr = (void*)(((unsigned int)paddr) + size);
                vaddr = (void*)(((unsigned int)vaddr) + size);
                head = node;
        }

        // Update member variables
        ChunkPool::num += num;
        ChunkPool::size = size;

        return true;
}


/*-----------------------------------------------------------------------*//*!
* Brief.
*
* Detailed description
*
* \param [in] name(Type)                        Description
*
* \retval       void
*
* \sa
*//*------------------------------------------------------------------------*/

Chunk
ChunkPool::New( )
{
        Chunk newChunk;

        if(head == NULL)                        // Free list is empty
                return NULL;

        newChunk = (Chunk) head;
        head = head->next;

        return newChunk;
}


/*-----------------------------------------------------------------------*//*!
* Brief.
*
* Detailed description
*
* \param [in] name(Type)                        Description
*
* \retval       void
*
* \sa
*//*------------------------------------------------------------------------*/

void
ChunkPool::Free( Chunk handle )
{
        ChunkNode* node = (ChunkNode*) handle;

        if(handle == NULL)                      // Sanity check
                return;

        node->next = head;
        head = node;
}


/*-----------------------------------------------------------------------*//*!
* Brief.
*
* Detailed description
*
* \param [in] name(Type)                        Description
*
* \retval       void
*
* \sa
*//*------------------------------------------------------------------------*/

void*
ChunkPool::GetPhyAddr( Chunk handle )
{
        ChunkNode* node = (ChunkNode*) handle;

        if(handle == NULL)                      // Sanity check
                return NULL;

        return node->paddr;
}


/*-----------------------------------------------------------------------*//*!
* Brief.
*
* Detailed description
*
* \param [in] name(Type)                        Description
*
* \retval       void
*
* \sa
*//*------------------------------------------------------------------------*/

void*
ChunkPool::GetVirtAddr( Chunk handle )
{
        ChunkNode* node = (ChunkNode*) handle;

        if(handle == NULL)                      // Sanity check
                return NULL;

        return node->vaddr;
}


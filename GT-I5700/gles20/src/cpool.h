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
* \file         cpool.h
* \author       Prashant Singh (prashant@samsung.com)
* \brief        Chunk pool header (Simple segregated storage implementation)
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

#ifndef __CPOOL_H__
#define __CPOOL_H__

/*
*******************************************************************************
* Includes
*******************************************************************************
*/

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

//! Handle to memory chunk
typedef void*   Chunk;

/*------------------------------------------------------------------------*//*!
* Memory chunk pool
*//*-------------------------------------------------------------------------*/

class ChunkPool {

        //! Node of free chunk list
        struct ChunkNode {
                void*           paddr;  //!< Physical address of memory chunk
                void*           vaddr;  //!< Virtual address of memory chunk
                ChunkNode*      next;   //!< Pointer to next chunk node
        };

private:

        ChunkNode*      head;           //!< Head to free chunk list
        int                     num;            //!< Number of chunks in this pool
        int                     size;           //!< Size of each chunk

public:

        // Interface functions ----------------------------------------------------

                        ChunkPool( );                                   //!< Constructor
                        ~ChunkPool( );                                  //!< Destructor

        bool    AddChunks( void* paddr, void* vaddr, int num, int size );
                                                                                        //!< Add chunks to pool

        Chunk   New();                                                  //!< Get new chunk from pool
        void    Free( Chunk handle );                   //!< Free a chunk and return to pool

        void*   GetPhyAddr( Chunk handle );             //!< Get physical pointer to memory chunk
        void*   GetVirtAddr( Chunk handle );    //!<  Get virtual pointer to memory chunk
};


/*
*******************************************************************************
* Global Variable Declarations
*******************************************************************************
*/

/*
*******************************************************************************
* Function Declarations
*******************************************************************************
*/

#endif /*__CPOOL_H__*/


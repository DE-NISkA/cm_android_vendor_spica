/**
 * Samsung Project
 * Copyright (c) 2007 Mobile XG, Samsung Electronics, Inc.
 * All right reserved.
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics Inc. ("Confidential Information"). You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung Electronics.
 */

/**
 * @file        interface.c
 * @brief       This is the host-interface-setting file.
 * @author      Hyunchang Kim
 * @version     2.0
 */

#include "fgl.h"
#include "register.h"

#ifndef FGHI_FIFO_SIZE
#define FGHI_FIFO_SIZE 32
#endif // FGHI_FIFO_SIZE


#if TARGET_FIMG_VERSION < _FIMG3DSE_VER_2_0
/***************************************************************************** 
 * FUNCTIONS: fglGetNumEmptyFIFOSlots
 * SYNOPSIS: This function obtains how many FIFO slots are empty in host interface
 * PARAMETERS: [OUT] pNumSlots: the number of empty slots
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 * ERRNO:
 *****************************************************************************/

FGL_Error fglGetNumEmptyFIFOSlots(unsigned int *pNumSlots)
{
        //*pNumSlots = READREG(FGHI_FIFO_EMPTY_SPACE);
        READREGP(FGHI_FIFO_EMPTY_SPACE, *pNumSlots);

        return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglDrawNonIndexArrays
 * SYNOPSIS: This function sends geometric data to rendering pipeline using non-index scheme.
 * PARAMETERS: [IN] numAttribs: number of input attributes
               [IN] pAttrib: array of input attributes
                           [IN] numVertices: number of vertices
                           [IN] ppData: array of pointers of input data
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if FGL_Attribute::numComp is greater than 4
 * ERRNO:
 *****************************************************************************/

FGL_Error fglDrawNonIndexArrays(int numAttribs, FGL_Attribute *pAttrib, int numVertices, void **ppData)
{
        int i, j;

        // write the property of input attributes
        
        for(i=0; i<numAttribs; i++)
        {
                fglSetAttribute(i, &(pAttrib[i]));
        }

        // write the number of vertices

        WRITEREGF(FGHI_FIFO_ENTRY, numVertices);
        WRITEREG(FGHI_FIFO_ENTRY, 0xFFFFFFFF);

        for(i=0; i<numVertices; i++)
        {
                for(j=0; j<numAttribs; j++)
                {
                        int n;

#ifdef _FGL_VALUE_CHECK
                        if(pAttrib[j].numComp > 4)
                                return FGL_ERR_INVALID_VALUE;
#endif

                        switch(pAttrib[j].type)
                        {
                        // 1bytes
                        case FGL_ATTRIB_DATA_BYTE:
                        case FGL_ATTRIB_DATA_UBYTE:
                        case FGL_ATTRIB_DATA_NBYTE:
                        case FGL_ATTRIB_DATA_NUBYTE:
                                {
                                        unsigned char bits[4];

                                        bits[0] = bits[1] = bits[2] = bits[3] = 0;
                                        for(n=0; n<pAttrib[j].numComp; n++)
                                        {
                                                bits[n] = ((unsigned char *)(ppData[j]))[pAttrib[j].numComp * i + n];
                                        }

                                        __fglSendToFIFO(4, bits,FGHI_FIFO_ENTRY);

                                        break;
                                }
                        // 2bytes
                        case FGL_ATTRIB_DATA_SHORT:
                        case FGL_ATTRIB_DATA_USHORT:
                        case FGL_ATTRIB_DATA_NSHORT:
                        case FGL_ATTRIB_DATA_NUSHORT:
                                {
                                        unsigned short bits[4];

                                        bits[0] = bits[1] = bits[2] = bits[3] = 0;
                                        for(n=0; n<pAttrib[j].numComp; n++)
                                        {
                                                bits[n] = ((unsigned short *)(ppData[j]))[pAttrib[j].numComp * i + n];
                                        }

                                        if(pAttrib[j].numComp > 2)
                                                __fglSendToFIFO(8, bits,FGHI_FIFO_ENTRY);
                                        else
                                                __fglSendToFIFO(4, bits,FGHI_FIFO_ENTRY);

                                        break;
                                }
                        // 4 bytes
                        case FGL_ATTRIB_DATA_FIXED:
                        case FGL_ATTRIB_DATA_NFIXED:
                        case FGL_ATTRIB_DATA_FLOAT:
                        case FGL_ATTRIB_DATA_INT:
                        case FGL_ATTRIB_DATA_UINT:
                        case FGL_ATTRIB_DATA_NINT:
                        case FGL_ATTRIB_DATA_NUINT:
                                {
                                        for(n=0; n<pAttrib[j].numComp; n++)
                                        {
                                                __fglSendToFIFO(4, &(((unsigned int *)(ppData[j]))[pAttrib[j].numComp * i + n]),FGHI_FIFO_ENTRY);
                                        }

                                        break;
                                }
                        }
                }
        }

        return FGL_ERR_NO_ERROR;
}



/***************************************************************************** 
 * FUNCTIONS: fglSendToFIFO
 * SYNOPSIS: This function sends data to the 3D rendering pipeline
 * PARAMETERS: [IN] buffer: pointer of input data
 *             [IN] bytes: the total bytes of input data
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_UNKNOWN - otherwise
 * ERRNO:
 *****************************************************************************/


/*inline FGL_Error fglSendToFIFO(unsigned int bytes, void *buffer)
{
	__fglSendToFIFO(bytes,buffer,FGHI_FIFO_ENTRY);
        unsigned int nEmptySpace = 0, nth = 0;
        unsigned char *ptr = buffer, bits[4];
        int i, diff;

        do {

#ifdef _FGL_VALUE_CHECK
                if(nEmptySpace > FGHI_FIFO_SIZE)
                        return FGL_ERR_UNKNOWN;
#endif

                if(nEmptySpace == 0)
                {
                        do {
                                //nEmptySpace = READREG(FGHI_FIFO_EMPTY_SPACE);
                                READREGP(FGHI_FIFO_EMPTY_SPACE, nEmptySpace);
                        } while(nEmptySpace < 1);
                }

                diff = bytes - nth;
                
                if(diff < 4)
                {
                        bits[0] = bits[1] = bits[2] = bits[3] = 0x00;

                        for(i=0; i<diff; i++)
                                bits[i] = ptr[nth++];

                        WRITEREG(FGHI_FIFO_ENTRY, *((unsigned int *)bits));
                        nEmptySpace --;
                }
                else
                {
                        WRITEREG(FGHI_FIFO_ENTRY, *((unsigned int *)&(ptr[nth])));
                        nEmptySpace --;
                        nth += 4;
                }

        } while(nth < bytes);
        
        return FGL_ERR_NO_ERROR;
}*/

#endif
/***************************************************************************** 
 * FUNCTIONS: fglSetHInterface
 * SYNOPSIS: This function defines whether vertex buffer, vertex cache and 
 *            auto increment index scheme is active or not.
 *           It also defines data type of transfered index and
 *            the number of output attributes of vertex shader.
 * PARAMETERS: [IN] pHI: pointer of FGL_HInterface
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - if FGL_HInterface::numVSOut is greater than 15
 * ERRNO:
 *****************************************************************************/


FGL_Error fglSetHInterface(FGL_HInterface *pHI)
{
        unsigned int bits = 0;

        FGL_Error error = FGL_ERR_NO_ERROR;

#if TARGET_FIMG_VERSION < _FIMG3DSE_VER_2_0     
        // enable vertex buffer
        // [31] == TRUE
        if(pHI->enableVtxBuffer == FGL_TRUE)
                bits |= 0x80000000;

        // index data type
        // [25:24]
        switch(pHI->idxType)
        {
        case FGL_INDEX_DATA_UINT: // 00
                break;
        case FGL_INDEX_DATA_USHORT: // 01
                bits |= 0x01000000;
                break;
        case FGL_INDEX_DATA_UBYTE:
                bits |= 0x03000000;
                break;
        }

        // auto increment
        // [16]

        if(pHI->enableAutoInc == FGL_TRUE)
                bits |= 0x00010000;
#endif

        // enable vertex cache
        // [4]

        if(pHI->enableVtxCache == FGL_TRUE)
                bits |= 0x00000010;

        // number of VS output
        // [3:0]

#ifdef _FGL_VALUE_CHECK
        if(pHI->numVSOut > 15)
                error = FGL_ERR_INVALID_VALUE;
        else
#endif
                bits |= pHI->numVSOut;

        // write

        WRITEREG(FGHI_HI_CTRL, bits);

        return error;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetIndexOffset
 * SYNOPSIS: This function defines index offset which is used in the auto increment mode
 * PARAMETERS: [IN] offset: index offset value
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetIndexOffset(unsigned int offset)
{
        WRITEREG(FGHI_IDX_OFFSET, offset);

        return FGL_ERR_NO_ERROR;
}

#if TARGET_FIMG_VERSION < _FIMG3DSE_VER_2_0     
/***************************************************************************** 
 * FUNCTIONS: fglSetVtxBufferAddr
 * SYNOPSIS: This function defines the starting address in vertex buffer,
 *            which are used to send data into vertex buffer
 * PARAMETERS: [IN] address: the starting address
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetVtxBufferAddr(unsigned int addr)
{
        WRITEREG(FGHI_VTXBUF_ADDR, addr);

        return FGL_ERR_NO_ERROR;
}

/***************************************************************************** 
 * FUNCTIONS: fglSendToVtxBuffer
 * SYNOPSIS: The function copies data into vertex buffer.
 * PARAMETERS: [IN] data: data issued into vertex buffer
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSendToVtxBuffer(unsigned int data)
{
        WRITEREG(FGHI_VTXBUF_ENTRY, data);

        return FGL_ERR_NO_ERROR;
}
#endif

/***************************************************************************** 
 * FUNCTIONS: fglSetAttribute
 * SYNOPSIS: This function specifies the property of attribute
 * PARAMETERS: [IN] attribIdx: the index of attribute, which is in [0-15]
 *             [IN] pAttribInfo: the pointer of FGL_Attribute
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetAttribute(unsigned char attribIdx, FGL_Attribute *pAttribInfo)
{
        unsigned int bits = 0;
        unsigned int t, s;

        FGL_Error error = FGL_ERR_NO_ERROR;

        // end
        // [31] == TRUE

        if(pAttribInfo->bEndFlag == FGL_TRUE)
                bits |= 0x80000000;

#if TARGET_FIMG_VERSION == _FIMG3DSE_VER_2_0    
        
        // address mode
        // [25:24]
        
        bits |= (pAttribInfo->addrMode << 0x18);

#endif

        // data type
        // [15:12]

        t = pAttribInfo->type;
        bits |= (t << 0xc);

        //  number of components (1~4)
        // [9:8]

#ifdef _FGL_VALUE_CHECK
        if(pAttribInfo->numComp < 1 || pAttribInfo->numComp > 4)
                error = FGL_ERR_INVALID_VALUE;
        else
#endif
        {
                unsigned int c = 0;

                c = pAttribInfo->numComp - 1;
                bits |= (c << 0x8);
        }

        // srcW
        // [7:6]

        s = pAttribInfo->srcW;
        bits |= (s << 0x6);

        // srcZ
        // [5:4]

        s = pAttribInfo->srcZ;
        bits |= (s << 0x4);

        // srcY
        // [3:2]

        s = pAttribInfo->srcY;
        bits |= (s << 0x2);

        // srcX
        // [1:0]

        s = pAttribInfo->srcX;
        bits |= s;

#ifdef _FGL_VALUE_CHECK
        if(attribIdx > 16)
                error = FGL_ERR_INVALID_VALUE;
        else
#endif
        {
                WRITEREG(FGHI_ATTR0 + 0x4 * attribIdx, bits);
        }

        return error;
}

/***************************************************************************** 
 * FUNCTIONS: fglSetVtxBufAttrib
 * SYNOPSIS: This function defines the property of attribute in vertex buffer
 * PARAMETERS: [IN] attribIdx: the index of attribute
 *             [IN] pAttribInfo: the pointer of FGL_VtxBufAttrib
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/

FGL_Error fglSetVtxBufAttrib(unsigned char attribIdx, FGL_VtxBufAttrib *pAttribInfo)
{
        FGL_Error error = FGL_ERR_NO_ERROR;

        unsigned int bits = 0;

        if(attribIdx > 16)
                error = FGL_ERR_INVALID_VALUE;
        else
        {
                // base address

                WRITEREG(FGHI_VTXBUF_BASE0 + 0x4 * attribIdx, pAttribInfo->addr);

                //stride, number of input attribute

#if TARGET_FIMG_VERSION < _FIMG3DSE_VER_2_0
                bits |= (pAttribInfo->num & 0x0000ffff);
#endif
                bits |= (pAttribInfo->stride << 0x18);

                WRITEREG(FGHI_VTXBUF_CTRL0 + 0x4 * attribIdx, bits);
        }

        return error;
}

#if TARGET_FIMG_VERSION == _FIMG3DSE_VER_2_0    
/***************************************************************************** 
 * FUNCTIONS: fglSetHIInstructions
 * SYNOPSIS: This function sets information of HI instructions
 * PARAMETERS: [IN] baseAddr: the base address of instructions
 *                         [IN] addrMode: the address mode of instructions
 *             [IN] addrCount: the number of instructions
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/

FGL_Error 
fglSetHIInstructions(unsigned int       baseAddr, FGL_AddrMode  addrMode, unsigned int  instCount)
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int bits = 0;
        bits = (baseAddr & 0x00000003);
        if(bits)
                error = FGL_ERR_INVALID_VALUE;
        else
        {
                bits = baseAddr;
                bits |= addrMode;
                WRITEREG(FGHI_INSTADDR, bits);  
                WRITEREG(FGHI_INSTCOUNT, instCount);            
        }       

        return error;
}

/***************************************************************************** 
 * FUNCTIONS: fglGetNextInstAddr
 * SYNOPSIS: This function gets information of next instruction address
 * PARAMETERS: [OUT] pNextAddr: address of next instruction                        
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/

FGL_Error 
fglGetNextInstAddr(unsigned int *pNextAddr)
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        *pNextAddr = READREG(FGHI_INSTADDRNEXT);
        
        return error;
}

/***************************************************************************** 
 * FUNCTIONS: fglGetRemainInstCount
 * SYNOPSIS: This function gets information of remain instruction count
 * PARAMETERS: [OUT] pInstCount: the number of remained instructions                       
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
                                                
FGL_Error 
fglGetRemainInstCount(unsigned int *pInstCount)
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        *pInstCount = READREG(FGHI_INSTCOUNTLEFT);
        
        return error;
}

/***************************************************************************** 
 * FUNCTIONS: fglInstSFRAD
 * SYNOPSIS: This function excute instruction for SFRAD mode
 * PARAMETERS: [IN] instOption: option of this instruction
 *                         [IN] numSFRPairs: the number of SFR pairs
 *             [IN] addrSrc: the base address of SFR pairs      
 *             [IN] addrMode: the address mode of base
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/

FGL_Error 
fglInstSFRAD(
                                FGL_InstOption instOption, unsigned int numSFRPairs, 
                                unsigned int addrSrc, FGL_AddrMode addrMode
                        )
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int bits = 0;
        bits = (addrSrc & 0x00000003);
        if(bits)
                error = FGL_ERR_INVALID_VALUE;
        else
        {
                unsigned int inst0 = 0, inst1 = 0, inst2 = 0, inst3 = 0;
                
                inst0 = (instOption.pipeMask & 0x00FFFFFF);
                
                inst1 = (instOption.wait << 0x1F);
                inst1 |= (instOption.interrupt << 0x1E);
                inst1 |= (addrMode << 0x1C);
                
                inst2 = numSFRPairs;
                
                inst3 = addrSrc;                        
                
                WRITEREG(FGHI_INST0, inst0);
                WRITEREG(FGHI_INST1, inst1);
                WRITEREG(FGHI_INST2, inst2);
                WRITEREG(FGHI_INST3, inst3);    
        }
                
        return error;   
}

/***************************************************************************** 
 * FUNCTIONS: fglInstSFRIM
 * SYNOPSIS: This function excute instruction for SFRID mode
 * PARAMETERS: [IN] instOption: option of this instruction
 *             [IN] addrDst: the destination address of SFR     
 *             [IN] SFRVal: the value of SFR
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
                                                
FGL_Error 
fglInstSFRIM(
                                FGL_InstOption instOption, unsigned int addrDst, unsigned int SFRVal
                        )
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int inst0 = 0, inst1 = 0, inst2 = 0, inst3 = 0;
        
        inst0 = (instOption.pipeMask & 0x00FFFFFF);
        inst0 |= 0x01000000; 
        
        inst1 = (instOption.wait << 0x1F);
        inst1 |= (instOption.interrupt << 0x1E);
        inst1 |= addrDst;
        
        inst2 = 0;
        
        inst3 = SFRVal;
                
        WRITEREG(FGHI_INST0, inst0);
        WRITEREG(FGHI_INST1, inst1);
        WRITEREG(FGHI_INST2, inst2);
        WRITEREG(FGHI_INST3, inst3);    

        return error;   
}
        
/***************************************************************************** 
 * FUNCTIONS: fglInstSFRAI
 * SYNOPSIS: This function excute instruction for SFRAI mode
 * PARAMETERS: [IN] instOption: option of this instruction
 *             [IN] addrDst: the destination address of SFR 
 *                         [IN] numSFR: the number of SFR 
 *             [IN] addrSrc: the source address of SFR 
 *             [IN] addrMode: the address mode of source
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
                                        
FGL_Error 
fglInstSFRAI(
                                FGL_InstOption instOption, unsigned int addrDst, unsigned int numSFR, 
                                unsigned int addrSrc, FGL_AddrMode addrMode
                        )
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int bits = 0;
        bits = (addrSrc & 0x00000003);
        if(bits)
                error = FGL_ERR_INVALID_VALUE;
        else
        {
                unsigned int inst0 = 0, inst1 = 0, inst2 = 0, inst3 = 0;
                
                inst0 = (instOption.pipeMask & 0x00FFFFFF);
                inst0 |= 0x02000000;
                
                inst1 = (instOption.wait << 0x1F);
                inst1 |= (instOption.interrupt << 0x1E);
                inst1 |= (addrMode << 0x1C);
                inst1 |= addrDst;
                
                inst2 = numSFR;
                
                inst3 = addrSrc;
                
                WRITEREG(FGHI_INST0, inst0);
                WRITEREG(FGHI_INST1, inst1);
                WRITEREG(FGHI_INST2, inst2);
                WRITEREG(FGHI_INST3, inst3);    
        }
                
        return error;   
}

/***************************************************************************** 
 * FUNCTIONS: fglInstVBWR
 * SYNOPSIS: This function excute instruction for VBWR mode
 * PARAMETERS: [IN] instOption: option of this instruction
 *             [IN] addrDst: the destination address of VB 
 *                         [IN] numWord: the number of source data
 *             [IN] addrSrc: the source address of source data
 *             [IN] addrMode: the address mode of source
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
                        
FGL_Error 
fglInstVBWR(
                                FGL_InstOption instOption, unsigned int addrDst, 
                                unsigned int numWord, unsigned int addrSrc, FGL_AddrMode addrMode
                        )
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int bitsDst = 0, bitsSrc = 0;
        bitsDst = (addrDst & 0x0000000F);
        bitsSrc = (addrSrc & 0x00000003);
        if(bitsDst | bitsSrc)
                error = FGL_ERR_INVALID_VALUE;
        else
        {
                unsigned int inst0 = 0, inst1 = 0, inst2 = 0, inst3 = 0;
                unsigned int numOf4Word = 0;
                
                inst0 = (instOption.pipeMask & 0x00FFFFFF);
                inst0 |= 0x10000000;
                
                inst1 = (instOption.wait << 0x1F);
                inst1 |= (instOption.interrupt << 0x1E);
                inst1 |= (addrMode << 0x1C);
                inst1 |= addrDst;
                
                numOf4Word = (numWord/4) + ((numWord%4) ? 1 : 0);
                inst2 = numOf4Word;
                
                inst3 = addrSrc;
                
                WRITEREG(FGHI_INST0, inst0);
                WRITEREG(FGHI_INST1, inst1);
                WRITEREG(FGHI_INST2, inst2);
                WRITEREG(FGHI_INST3, inst3);    
        }
                
        return error;   
}

/***************************************************************************** 
 * FUNCTIONS: fglInstNOP
 * SYNOPSIS: This function excute instruction for NOP mode
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
                                                
FGL_Error 
fglInstNOP(FGL_InstOption instOption)
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int inst0 = 0, inst1 = 0, inst2 = 0, inst3 = 0;
        
        inst0 = (instOption.pipeMask & 0x00FFFFFF);
        inst0 |= 0x20000000;
        
        inst1 = (instOption.wait << 0x1F);
        inst1 |= (instOption.interrupt << 0x1E);
                
        WRITEREG(FGHI_INST0, inst0);
        WRITEREG(FGHI_INST1, inst1);
        WRITEREG(FGHI_INST2, inst2);
        WRITEREG(FGHI_INST3, inst3);
                
        return error;   
}

/***************************************************************************** 
 * FUNCTIONS: fglInstINDEX
 * SYNOPSIS: This function excute instruction for INDEX mode
 * PARAMETERS: [IN] instOption: option of this instruction
 *             [IN] indexType: the type of index
 *                         [IN] numIndices: the number of indices
 *             [IN] addrIndex: the base address of indices
 *             [IN] addrMode: the address mode of source
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
 
FGL_Error 
fglInstINDEX(
                                FGL_InstOption instOption, FGL_IndexDataType indexType,
                                unsigned int numIndices, unsigned int addrIndex, FGL_AddrMode addrMode
                        )
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int bits = 0;
        bits = (addrIndex & 0x00000003);
        if(bits)
                error = FGL_ERR_INVALID_VALUE;
        else
        {
                unsigned int inst0 = 0, inst1 = 0, inst2 = 0, inst3 = 0;
                
                inst0 = (instOption.pipeMask & 0x00FFFFFF);
                inst0 |= (indexType << 0x18);
                
                inst1 = (instOption.wait << 0x1F);
                inst1 |= (instOption.interrupt << 0x1E);
                inst1 |= (addrMode << 0x1C);
                
                inst2 = numIndices;
                
                inst3 = addrIndex;
                
                WRITEREG(FGHI_INST0, inst0);
                WRITEREG(FGHI_INST1, inst1);
                WRITEREG(FGHI_INST2, inst2);
                WRITEREG(FGHI_INST3, inst3);    
        }
                
        return error;   
}

/***************************************************************************** 
 * FUNCTIONS: fglInstINDEXAI
 * SYNOPSIS: This function excute instruction for INDEXAI mode
 * PARAMETERS: [IN] instOption: option of this instruction
 *                         [IN] numIndices: the number of indices
 *             [IN] startIndex: the start index for auto increase mode
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
                                                
FGL_Error 
fglInstINDEXAI(
                                FGL_InstOption instOption, unsigned int numIndices, unsigned int startIndex
                          )
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int inst0 = 0, inst1 = 0, inst2 = 0, inst3 = 0;
        
        inst0 = (instOption.pipeMask & 0x00FFFFFF);
        inst0 |= 0x84000000; 
        
        inst1 = (instOption.wait << 0x1F);
        inst1 |= (instOption.interrupt << 0x1E);
        
        inst2 = numIndices;
        
        inst3 = startIndex;
                
        WRITEREG(FGHI_INST0, inst0);
        WRITEREG(FGHI_INST1, inst1);
        WRITEREG(FGHI_INST2, inst2);
        WRITEREG(FGHI_INST3, inst3);    
                
        return error;   
}

/***************************************************************************** 
 * FUNCTIONS: fglEXEControl
 * SYNOPSIS: This function control the behavior of inter-instruction
 * PARAMETERS: [IN] interrupt: invoke interrupt after instruction
 *                         [IN] wait: wait after instruction
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
                                                
FGL_Error 
fglEXEControl(
                                FG_BOOL interrupt, FG_BOOL wait
                         )
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int bits = 0;
        
        bits = (interrupt << 0x1);
        bits |= wait;
        
        WRITEREG(FGHI_EXECTRL, bits);
                
        return error;   
}

/***************************************************************************** 
 * FUNCTIONS: fglSetVBSize
 * SYNOPSIS: This function adjust the ratio between VB and PreVC
 * PARAMETERS: [IN] VBSizeMode: predefined enum variable about ratio
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/

FGL_Error 
fglSetVBSize(
                                FGL_VBSizeMode VBSizeMode
                        )
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int bits = 0;
        
        fglClearPreVC();
        
        bits = VBSizeMode;
        
        WRITEREG(FGHI_VBSIZE, bits);
                
        return error;   
}

/***************************************************************************** 
 * FUNCTIONS: fglClearPreVC
 * SYNOPSIS: This function clear preVC
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
 
FGL_Error 
fglClearPreVC(void)
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int bits = 0;
        
        WRITEREG(FGHI_PREVCCLR, bits);
                
        return error;   
}

/***************************************************************************** 
 * FUNCTIONS: fglClearPostVC
 * SYNOPSIS: This function clear postVC
 * RETURNS: FGL_ERR_NO_ERROR - if successful.
 *          FGL_ERR_INVALID_VALUE - otherwise
 * ERRNO:
 *****************************************************************************/
 
FGL_Error 
fglClearPostVC(void)
{
        FGL_Error error = FGL_ERR_NO_ERROR;
        
        unsigned int bits = 0;
        
        WRITEREG(FGHI_POSTVCCLR, bits);
                
        return error;   
}

#endif //TARGET_FIMG_VERSION == _FIMG3DSE_VER_2_0       

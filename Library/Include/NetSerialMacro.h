/******************************************************************************
* Copyright (c) 2012, Letion
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of California, Berkeley nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
///============================================================================
/// \file    : NetSerialMacro.h
/// \brief   : 网络序列化宏定义文件
/// \author  : letion
/// \version : 1.0
/// \date    : 2012-05-16
///============================================================================
#ifndef __NET_SERIAL_MACRO_H__
#define __NET_SERIAL_MACRO_H__

//=============================================================================
//缺省构造实现
#define IMPLEMENT_NET_CONSTRUCTOR(class_name) \
class_name::class_name( void ) \
{ \
	memset(this ,0, sizeof(class_name) ); \
} 

//=============================================================================
//缺省打包函数实现
#define IMPLEMENT_NET_SERIALIZE_PACK(class_name,package_serial_number) \
int \
class_name::Pack(char* apBuffer, long alLen ) \
{ \
    try \
    { \
        CNetSerialize loSerialize(apBuffer, alLen, CNetSerialize::STORE ); \
		WORD lwPackType=package_serial_number; \
        loSerialize.Serialize( lwPackType ); \
        if( Serialize(loSerialize) == FALSE ) \
        { \
            return 0; \
        } \
        return loSerialize.GetDataSize(); \
    } \
    catch( ... ) \
    { \
        return 0; \
    } \
}

//=============================================================================
//缺省解包函数
#define IMPLEMENT_NET_SERIALIZE_UNPACK(class_name) \
BOOL \
class_name::UnPack( char* apBuffer , long alLen ) \
{ \
    try \
    { \
        CNetSerialize loSerialize( apBuffer, alLen, CNetSerialize::LOAD ); \
        WORD lwPackType ; \
        loSerialize.Serialize(lwPackType); \
        if ( Serialize(loSerialize) == FALSE ) \
        { \
            return FALSE; \
        } \
        return TRUE; \
    } \
    catch( ... ) \
    { \
        return FALSE; \
    } \
}

//=============================================================================
//缺省构造，解包，打包函数定义
#define DECLEAR_NET_PACK_SERIALIZE() \
public:\
	int    Pack(char * apNetBuffer ,long alBuffLen);\
	BOOL   UnPack(char * apNetBuffer,long alBuffLen);\
	BOOL   Serialize(CStdSerialize & aoStdSerialize);

//=============================================================================
//缺省构造，解包，打包函数实现
#define IMPLEMENT_NET_PACK_SERIALIZE(class_name,package_serial_number) \
	IMPLEMENT_NET_SERIALIZE_PACK(class_name,package_serial_number) \
	IMPLEMENT_NET_SERIALIZE_UNPACK(class_name) \


#endif //__NET_SERIAL_MACRO_H__

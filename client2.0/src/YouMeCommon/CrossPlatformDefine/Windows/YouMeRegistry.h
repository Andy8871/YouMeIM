#ifndef _BLREGISTRY_H
#define _BLREGISTRY_H

#if _MSC_VER > 1000
#pragma once
#endif

//////////////////////////////////////////////////////////////////////////

// 下面是KEY_TYPE的种类，最后加了两项，用来判断KEY_TYPE是否可信
//

// #define REG_NONE                    ( 0 )   // No value type
// #define REG_SZ                      ( 1 )   // Unicode nul terminated string
// #define REG_EXPAND_SZ               ( 2 )   // Unicode nul terminated string
// #define REG_BINARY                  ( 3 )   // Free form binary
// #define REG_DWORD                   ( 4 )   // 32-bit number
// #define REG_DWORD_LITTLE_ENDIAN     ( 4 )   // 32-bit number (same as REG_DWORD)
// #define REG_DWORD_BIG_ENDIAN        ( 5 )   // 32-bit number
// #define REG_LINK                    ( 6 )   // Symbolic Link (unicode)
// #define REG_MULTI_SZ                ( 7 )   // Multiple Unicode strings
// #define REG_RESOURCE_LIST           ( 8 )   // Resource list in the resource map
// #define REG_FULL_RESOURCE_DESCRIPTOR ( 9 )  // Resource list in the hardware description
// #define REG_RESOURCE_REQUIREMENTS_LIST ( 10 )
// #define REG_QWORD                   ( 11 )  // 64-bit number
// #define REG_QWORD_LITTLE_ENDIAN     ( 11 )  // 64-bit number (same as REG_QWORD)


// Extended...
//
#define REG_NOT_VALID   -1
#define REG_NO_ACCESS   -2

typedef __success(return >= 0) LONG KEY_TYPE;

//////////////////////////////////////////////////////////////////////////

// 参数示例:
//
// [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control]
//    "CurrentUser"="USERNAME"
//
// RootKey : HKEY_LOCAL_MACHINE
// SubKey: SYSTEM\CurrentControlSet\Control
// ValueName: CurrentUser
// ValueData: USERNAME

//////////////////////////////////////////////////////////////////////////

// 返回RegHandle句柄，用这个句柄做接下来的查询、设置、删除操作
//
__checkReturn
HANDLE
BlRegOpenKey(
    __in HKEY RootKey, 
    __in LPCWSTR SubKey, 
    __in ACCESS_MASK Access,
    __in BOOLEAN Create
    );

void 
BlRegCloseKey(
    __in HANDLE RegHandle
    );

LSTATUS
BlRegQuerySubkey(
    __in HANDLE RegHandle, 
    __out_opt PULONG SubkeyCount,
    __out_opt PULONG MaxSubkeyLen
    );

// ERROR_NO_MORE_ITEMS 没有子键
// ERROR_MORE_DATA ValueName内存太小
//
LSTATUS
BlRegEnumSubkey(
    __in HANDLE RegHandle, 
    __in ULONG SubkeyIndex,
    __out LPTSTR ValueName,
    __inout PULONG ValueLen
    );

LSTATUS 
BlRegDeleteValue(
    __in HANDLE RegHandle, 
    __in LPCWSTR ValueName
    );

//////////////////////////////////////////////////////////////////////////

// 这里提供的函数一次只能操作一个键值 (打开-操作-关闭)
// 如果需要在一个子键下查询或者操作多个值，使用上面的方法
//

// 使用下面的方式来验证返回值是否可信，可能值并不存在
//
// if(RetValue != VALUE_ERROR)
//
#define VALUE_ERROR (RETVALUE)('t7LB')  // 特征值，不能使用0或者-1，因为返回值可能就是

typedef __success(return != VALUE_ERROR) ULONG RETVALUE;

RETVALUE
BlRegGetDword(
    __in HKEY RootKey,
    __in LPCWSTR SubKey,
    __in LPCWSTR ValueName
    );

// 返回字符串需要的长度
//
ULONG
BlRegGetString(
    __in HKEY RootKey,
    __in LPCWSTR SubKey,
    __in LPCWSTR ValueName,
    __inout_opt LPWSTR ValueData
    );

ULONG
BlRegGetBin(
    __in HKEY RootKey,
    __in LPCWSTR SubKey,
    __in LPCWSTR ValueName,
    __inout_opt LPBYTE ValueData
    );

LSTATUS
BlRegDelete(
    __in HKEY RootKey,
    __in LPCWSTR SubKey,
    __in LPCWSTR ValueName
    );

//////////////////////////////////////////////////////////////////////////

// 这里是内部统一的查询和设置方法
//

// 可以使用SUCCEEDED(KEY_TYPE)来判断函数是否执行成功
//
KEY_TYPE
BlRegReadValue(
    __in HANDLE RegHandle,
    __in LPCWSTR ValueName,
    __out LPBYTE ValueData,
    __inout PULONG cbValueLen
    );

//////////////////////////////////////////////////////////////////////////

#endif
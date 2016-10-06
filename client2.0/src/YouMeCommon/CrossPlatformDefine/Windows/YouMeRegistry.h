#ifndef _BLREGISTRY_H
#define _BLREGISTRY_H

#if _MSC_VER > 1000
#pragma once
#endif

//////////////////////////////////////////////////////////////////////////

// ������KEY_TYPE�����࣬��������������ж�KEY_TYPE�Ƿ����
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

// ����ʾ��:
//
// [HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control]
//    "CurrentUser"="USERNAME"
//
// RootKey : HKEY_LOCAL_MACHINE
// SubKey: SYSTEM\CurrentControlSet\Control
// ValueName: CurrentUser
// ValueData: USERNAME

//////////////////////////////////////////////////////////////////////////

// ����RegHandle����������������������Ĳ�ѯ�����á�ɾ������
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

// ERROR_NO_MORE_ITEMS û���Ӽ�
// ERROR_MORE_DATA ValueName�ڴ�̫С
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

// �����ṩ�ĺ���һ��ֻ�ܲ���һ����ֵ (��-����-�ر�)
// �����Ҫ��һ���Ӽ��²�ѯ���߲������ֵ��ʹ������ķ���
//

// ʹ������ķ�ʽ����֤����ֵ�Ƿ���ţ�����ֵ��������
//
// if(RetValue != VALUE_ERROR)
//
#define VALUE_ERROR (RETVALUE)('t7LB')  // ����ֵ������ʹ��0����-1����Ϊ����ֵ���ܾ���

typedef __success(return != VALUE_ERROR) ULONG RETVALUE;

RETVALUE
BlRegGetDword(
    __in HKEY RootKey,
    __in LPCWSTR SubKey,
    __in LPCWSTR ValueName
    );

// �����ַ�����Ҫ�ĳ���
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

// �������ڲ�ͳһ�Ĳ�ѯ�����÷���
//

// ����ʹ��SUCCEEDED(KEY_TYPE)���жϺ����Ƿ�ִ�гɹ�
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
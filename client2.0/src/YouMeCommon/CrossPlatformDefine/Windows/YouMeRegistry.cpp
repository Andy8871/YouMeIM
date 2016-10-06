
#include <windows.h>

//#include "BlString.h"
#include "YouMeRegistry.h"

//////////////////////////////////////////////////////////////////////////s

#ifndef BlMemMalloc
#define BlMemMalloc(_Size_) \
    HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, _Size_)
#endif

#ifndef BlMemFree
#define BlMemFree(_Addr_) \
    if (_Addr_){HeapFree(GetProcessHeap(), 0, _Addr_); _Addr_ = NULL;}
#endif
//////////////////////////////////////////////////////////////////////////

typedef struct _KEY_CONTEXT
{
    HKEY KeyHandle;
    REGSAM RegSam;

    KEY_TYPE CurType;

    ULONG ErrorCode; // FORMAT_MESSAGE_FROM_SYSTEM

}KEY_CONTEXT, *PKEY_CONTEXT;

//////////////////////////////////////////////////////////////////////////

HANDLE
BlRegOpenKey(
    __in HKEY RootKey, 
	__in LPCWSTR SubKey,
    __in ACCESS_MASK Access,
    __in BOOLEAN Create
    )
{
    HKEY KeyHandle = NULL;
    PKEY_CONTEXT KeyContext = NULL;
    LSTATUS lResult = ERROR_SUCCESS;

    lResult = RegOpenKeyExW(
        RootKey,
        SubKey,
        0, 
        Access,
        &KeyHandle
        );
    if (lResult == ERROR_FILE_NOT_FOUND && Create)
    {
        lResult = RegCreateKeyExW(
            RootKey,
            SubKey, 
            0, 
            NULL,
            REG_OPTION_NON_VOLATILE,
            Access,
            NULL, 
            &KeyHandle,
            NULL
            );
    }

    if (lResult == ERROR_SUCCESS)
    {
        KeyContext = (PKEY_CONTEXT)BlMemMalloc(sizeof(KEY_CONTEXT));

        if (!KeyContext)
        {
            return NULL;
        }

        KeyContext->RegSam = Access;
        KeyContext->KeyHandle = KeyHandle;
    }

    return (HANDLE)KeyContext;
}


void 
BlRegCloseKey(
    __in HANDLE RegHandle
    )
{
    PKEY_CONTEXT KeyContext = (PKEY_CONTEXT)RegHandle;

    if (!KeyContext)
    {
        return;
    }

    if (KeyContext->KeyHandle)
    {
        RegFlushKey(KeyContext->KeyHandle);
        RegCloseKey(KeyContext->KeyHandle);
    }

    BlMemFree(KeyContext);

    return;
}


KEY_TYPE
BlRegReadValue(
    __in HANDLE RegHandle,
	__in LPCWSTR ValueName,
    __out LPBYTE ValueData,
    __inout PULONG cbValueLen
    )
{
    PKEY_CONTEXT KeyContext = (PKEY_CONTEXT)RegHandle;
    KEY_TYPE RetKeyType = REG_NONE;
    LSTATUS lResult = ERROR_SUCCESS;

    if (!KeyContext || !cbValueLen)
    {
        return REG_NOT_VALID;
    }

    if((KeyContext->RegSam & KEY_QUERY_VALUE) != KEY_QUERY_VALUE)
    {
        return REG_NO_ACCESS;
    }

    lResult = RegQueryValueExW(
        KeyContext->KeyHandle,
        ValueName,
        0,
        (PULONG)&RetKeyType,
        ValueData,
        cbValueLen
        );
    if ((lResult != ERROR_SUCCESS) || !cbValueLen)
    {
        KeyContext->ErrorCode = lResult;

        RetKeyType = REG_NOT_VALID;
    }

    KeyContext->CurType = RetKeyType;

    return RetKeyType;
}

LSTATUS
BlRegQuerySubkey(
    __in HANDLE RegHandle, 
    __out_opt PULONG SubkeyCount,
    __out_opt PULONG MaxSubkeyLen
    )
{
    PKEY_CONTEXT KeyContext = (PKEY_CONTEXT)RegHandle;
    LSTATUS lResult = ERROR_SUCCESS;

    lResult = RegQueryInfoKey(
        KeyContext->KeyHandle, 
        NULL, NULL, NULL,
        SubkeyCount,
        MaxSubkeyLen, 
        NULL, NULL, NULL, 
        NULL, NULL, NULL
        );

    return lResult;
}

LSTATUS
BlRegEnumSubkey(
    __in HANDLE RegHandle, 
    __in ULONG SubkeyIndex,
    __out LPTSTR ValueName,
    __inout PULONG ValueLen
    )
{
    PKEY_CONTEXT KeyContext = (PKEY_CONTEXT)RegHandle;
    LSTATUS lResult = ERROR_SUCCESS;

    lResult = RegEnumKeyEx(
        KeyContext->KeyHandle, 
        SubkeyIndex, 
        ValueName, 
        ValueLen, 
        0, 
        NULL,
        NULL,
        NULL
        );

    return lResult;
}

LSTATUS 
BlRegDeleteValue(
    __in HANDLE RegHandle, 
	__in LPCWSTR ValueName
    )
{
    PKEY_CONTEXT KeyContext = (PKEY_CONTEXT)RegHandle;
    LSTATUS lResult = ERROR_SUCCESS;

    if (!KeyContext || !ValueName)
    {
        return ERROR_PATH_NOT_FOUND;
    }

    if((KeyContext->RegSam & KEY_SET_VALUE) != KEY_SET_VALUE)
    {
        return ERROR_ACCESS_DENIED;
    }

    lResult = RegDeleteValueW(
        KeyContext->KeyHandle,
        ValueName
        );

    KeyContext->ErrorCode = lResult;

    return lResult;
}

//////////////////////////////////////////////////////////////////////////

RETVALUE
BlRegGetDword(
    __in HKEY RootKey,
    __in LPCWSTR SubKey,
    __in LPCWSTR ValueName
    )
{
    HKEY KeyHandle = NULL;
    LSTATUS lResult = ERROR_SUCCESS;
    ULONG ValueData = VALUE_ERROR;
    ULONG ValueLen = sizeof(ULONG);

    do 
    {
        lResult = RegOpenKeyExW(
            RootKey,
            SubKey,
            0,
            KEY_READ,
            &KeyHandle
            );
        if (lResult != ERROR_SUCCESS)
        {
            break;
        }

        lResult = RegQueryValueExW(
            KeyHandle,
            ValueName,
            0,
            NULL,
            (LPBYTE)&ValueData,
            &ValueLen
            );
        if (lResult != ERROR_SUCCESS)
        {
            ValueData = VALUE_ERROR;

            break;
        }

    } while (FALSE);

    if (KeyHandle)
    {
        RegCloseKey(KeyHandle);
    }

    return ValueData;
}

ULONG
BlRegGetBin(
    __in HKEY RootKey,
	__in LPCWSTR SubKey,
	__in LPCWSTR ValueName,
    __out LPBYTE ValueData
    )
{
    HKEY KeyHandle = NULL;
    LSTATUS lResult = ERROR_SUCCESS;
    ULONG ValueLen = 0;
    ULONG ValueType = REG_NONE;

    do
    {
        lResult = RegOpenKeyExW(
            RootKey,
            SubKey,
            0,
            KEY_READ,
            &KeyHandle
            );
        if (lResult != ERROR_SUCCESS)
        {
            break;
        }

        lResult = RegQueryValueExW(
            KeyHandle,
            ValueName,
            0,
            NULL,
            NULL,
            &ValueLen
            );
        if (lResult != ERROR_SUCCESS)
        {
            break;
        }

        if (ValueData)
        {
            lResult = RegQueryValueExW(
                KeyHandle,
                ValueName,
                0,
                &ValueType,
                (LPBYTE)ValueData,
                &ValueLen
                );
            if (lResult != ERROR_SUCCESS)
            {
                break;
            }
        }

    } while (FALSE);

    if (KeyHandle)
    {
        RegCloseKey(KeyHandle);
    }

    return ValueLen;
}

ULONG
BlRegGetString(
    __in HKEY RootKey,
	__in LPCWSTR SubKey,
	__in LPCWSTR ValueName,
    __out LPWSTR ValueData
    )
{    
    return BlRegGetBin(
        RootKey,
        SubKey,
        ValueName,
        (LPBYTE)ValueData
        );
}

LSTATUS
BlRegDelete(
    __in HKEY RootKey,
	__in LPCWSTR SubKey,
	__in LPCWSTR ValueName
    )
{
    HANDLE KeyHandle = NULL;
    LSTATUS lResult = ERROR_SUCCESS;

    do 
    {
        KeyHandle = BlRegOpenKey(
            RootKey,
            SubKey,
            KEY_ALL_ACCESS,
            FALSE
            );
        if (KeyHandle == NULL)
        {
            break;
        }

        lResult = BlRegDeleteValue(
            KeyHandle, 
            ValueName
            );
        if (lResult != ERROR_SUCCESS)
        {
            break;
        }

    } while (FALSE);

    if (KeyHandle)
    {
        BlRegCloseKey(KeyHandle);
    }

    return lResult;
}
#include "stdafx.h"
#include "RegistryHelpers.h"

void PrintErrorDescription(DWORD error)
{
    LPWSTR pBuffer = NULL;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error,
        0,
        (LPWSTR)&pBuffer,
        0,
        NULL);
    wprintf(pBuffer);
    LocalFree(pBuffer);
}

int WriteKey(_In_z_ LPCWSTR subkeyName, _In_z_ LPCWSTR valueName, _In_z_ LPCWSTR valueData, _In_ int valueDataLen)
{
    LSTATUS status;
    HKEY regKey;
    DWORD disposition;
    status = RegOpenKeyExW(HKEY_CURRENT_USER, subkeyName, 0, KEY_SET_VALUE, &regKey);
    if (status == ERROR_FILE_NOT_FOUND)
    {
        status = RegCreateKeyExW(HKEY_CURRENT_USER, subkeyName, 0, NULL, 0, KEY_SET_VALUE, NULL, &regKey, &disposition);
        if (status != ERROR_SUCCESS)
        {
            PrintErrorDescription(status);
            return status;
        }
    }
    else if (status != ERROR_SUCCESS)
    {
        PrintErrorDescription(status);
        return status;
    }

    status = RegSetValueExW(
        regKey,
        valueName,
        NULL,
        REG_SZ,
        reinterpret_cast<CONST BYTE*>(valueData),
        (valueDataLen + 1) * sizeof(WCHAR));
    if (status != ERROR_SUCCESS)
    {
        PrintErrorDescription(status);
        return status;
    }

    status = RegCloseKey(regKey);
    if (status != ERROR_SUCCESS)
    {
        PrintErrorDescription(status);
        return status;
    }

    return 0;
}

int ReadKey(
    _In_z_ LPCWSTR subkeyName,
    _In_z_ LPCWSTR valueName,
    _Inout_ std::unique_ptr<WCHAR, decltype(&::CallHeapFree)>& str,
    _Out_ DWORD* strLen)
{
    LSTATUS status;
    HKEY regKey;
    status = RegOpenKeyExW(HKEY_CURRENT_USER, subkeyName, 0, KEY_QUERY_VALUE, &regKey);
    if (status != ERROR_SUCCESS)
    {
        PrintErrorDescription(status);
        return status;
    }

    // TODO: Query registry two times to get the exact buffer size needed and remember the last value returned.
    DWORD bufferLen = 1000;

    LPBYTE buffer2 = static_cast<LPBYTE>(HeapAlloc(GetProcessHeap(), 0, bufferLen));
    LPWSTR str2 = reinterpret_cast<LPWSTR>(buffer2);
    if (buffer2 == NULL)
    {
        return ERROR_OUTOFMEMORY;
    }

    std::swap(str, std::unique_ptr<WCHAR, decltype(&CallHeapFree)>(str2, CallHeapFree));

    DWORD valueType; // REG_SZ expected
    status = RegQueryValueExW(
        regKey,
        valueName,
        NULL,
        &valueType,
        buffer2,
        &bufferLen);
    if (status != ERROR_SUCCESS)
    {
        PrintErrorDescription(status);
        return status;
    }

    status = RegCloseKey(regKey);
    if (status != ERROR_SUCCESS)
    {
        PrintErrorDescription(status);
        return status;
    }

    *strLen = (bufferLen / sizeof(WCHAR)) - 1;

    return 0;
}

void CallHeapFree(_In_ LPVOID mem)
{
    HeapFree(GetProcessHeap(), 0, mem);
}

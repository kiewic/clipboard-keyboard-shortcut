#pragma once

void CallHeapFree(_In_ LPVOID mem);

int WriteKey(_In_z_ LPCWSTR subkeyName, _In_z_ LPCWSTR valueName, _In_z_ LPCWSTR valueData, _In_ int valueDataLen);

int ReadKey(
    _In_z_ LPCWSTR subkeyName,
    _In_z_ LPCWSTR valueName,
    _Inout_ std::unique_ptr<WCHAR, decltype(&CallHeapFree)>& str,
    _Out_ DWORD* strLen);

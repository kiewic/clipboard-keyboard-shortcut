// ClipboardShortcut.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Shlwapi.h>
#include <memory>

#pragma comment(lib, "Shlwapi.lib")

// See: https://msdn.microsoft.com/en-us/library/dd375731.aspx
const UINT KeyV = 0x56;
//const UINT ControlKey = 0x11;
const UINT ControlKey = 0xA2;

int MainCore();
void CopyToClipboard();
void Paste();

int main()
{
    return MainCore();
}

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow)
{
    return MainCore();
}

int CommandHelp()
{
    wprintf(L"Usage:\r\n");
    wprintf(L"  --help             prints this help message\r\n");
    wprintf(L"  --install <value>  run the application at windows startup and/or sets a new value to copy to the clipboard\r\n");
    wprintf(L"  --uninstall        removes application from windows startup\r\n");
    return 0;
}

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

int CommandInstall()
{
    DWORD exeNameLen;
    WCHAR exeName[MAX_PATH + 1];
    exeNameLen = GetModuleFileName(NULL, exeName, MAX_PATH);
    if (exeNameLen == 0)
    {
        return GetLastError();
    }

    //Microsoft.Win32.RegistryKey key = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true);
    //key.SetValue(curAssembly.GetName().Name, curAssembly.Location);

    LSTATUS status;
    HKEY regKey;
    status = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE | KEY_SET_VALUE, &regKey);
    if (status != ERROR_SUCCESS)
    {
        PrintErrorDescription(status);
        return status;
    }

    LPWSTR fileName = PathFindFileName(exeName);
    status = RegSetValueExW(regKey, fileName, NULL, REG_SZ, reinterpret_cast<BYTE*>(exeName), (exeNameLen + 1) * sizeof(WCHAR));
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

    wprintf(L"Successfully installed!");

    return 0;
}

int CommandUninstall()
{
    return 0;
}

int RunMessageLoop()
{
    BOOL result = RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, KeyV);
    printf("%d\n", result);

    if (result)
    {
        MSG msg = { 0 };
        while (GetMessage(&msg, NULL, 0, 0) != 0) 
        {
            if (msg.message == WM_HOTKEY)
            {
                printf("Hotkey!\n");
                CopyToClipboard();
                //Paste();
            }
        }
    }

    return 0;
}

int MainCore()
{
    LPWSTR commandLine = GetCommandLineW();
    int argc;
    std::unique_ptr<LPWSTR, decltype(&::LocalFree)> argv(
        CommandLineToArgvW(commandLine, &argc),
        ::LocalFree);

    for (int i = 0; i < argc; i++)
    {
        wprintf(L"%s\n", argv.get()[i]); // TODO: Remove

        if (_wcsicmp(argv.get()[i], L"--help") == 0)
        {
            return CommandHelp();
        }

        if (_wcsicmp(argv.get()[i], L"--install") == 0)
        {
            return CommandInstall();
        }

        if (_wcsicmp(argv.get()[i], L"--uninstall") == 0)
        {
            return CommandUninstall();
        }
    }

    return RunMessageLoop();
}

void CopyToClipboard()
{
    const char* output = "hello@kiewic.com";
    const size_t len = strlen(output) + 1;
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), output, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}

void DebugResult(UINT result)
{
    if (result == 0)
    {
        printf("%d\n", GetLastError());
    }
    else
    {
        printf("%d\n", result);
    }
}

void Paste()
{
    // Wait until no key is pressed.
    while ((GetKeyState(VK_CONTROL) & 0x80) != 0 ||
        (GetKeyState(VK_MENU) & 0x80) != 0 ||
        (GetKeyState('V') & 0x80) != 0)
    {
        if ((GetKeyState(VK_CONTROL) & 0x80) != 0)
            printf("Control pressed! %d\n", GetKeyState(VK_CONTROL) & 0x80);

        if ((GetKeyState(VK_MENU) & 0x80) != 0)
            printf("Alt pressed! %d\n", GetKeyState(VK_MENU) & 0x80);

        if ((GetKeyState('V') & 0x80) != 0)
            printf("V pressed! %d\n", GetKeyState('V') & 0x80);

        Sleep(0);
    }

    INPUT input[4];

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_CONTROL;
    input[0].ki.wScan = 0;
    input[0].ki.dwFlags = 0; // Key pressed
    input[0].ki.time = 0;
    input[0].ki.dwExtraInfo = 0;

    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = 'V';
    input[1].ki.wScan = 0;
    input[0].ki.dwFlags = 0; // Key pressed
    input[1].ki.time = 0;
    input[1].ki.dwExtraInfo = 0;

    input[2].type = INPUT_KEYBOARD;
    input[1].ki.wVk = 'V';
    input[2].ki.wScan = 0;
    input[2].ki.dwFlags = KEYEVENTF_KEYUP;
    input[2].ki.time = 0;
    input[2].ki.dwExtraInfo = 0;

    input[3].type = INPUT_KEYBOARD;
    input[3].ki.wVk = VK_CONTROL;
    input[3].ki.wScan = 0;
    input[3].ki.dwFlags = KEYEVENTF_KEYUP;
    input[3].ki.time = 0;
    input[3].ki.dwExtraInfo = 0;

    UINT result;
    result = SendInput(1, &input[0], sizeof(INPUT));
    DebugResult(result);

    result = SendInput(1, &input[1], sizeof(INPUT));
    DebugResult(result);

    result = SendInput(1, &input[2], sizeof(INPUT));
    DebugResult(result);

    result = SendInput(1, &input[3], sizeof(INPUT));
    DebugResult(result);
}


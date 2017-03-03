// ClipboardShortcut.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// See: https://msdn.microsoft.com/en-us/library/dd375731.aspx
const UINT KeyV = 0x56;
//const UINT ControlKey = 0x11;
const UINT ControlKey = 0xA2;

void CopyToClipboard();
void Paste();


int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow)
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


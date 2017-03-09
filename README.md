# clipboard-keyboard-shortcut

This application adds a global keyboard shortcut to Widnows so you can copy a frequently used value into the clipboard easily!

Here is how it works:


1. Build or download the binaries from the the *release* section.
2. Install using the following command, replace `hello@kiewic.com` with a value you copy-and-paste many times:
    ```
    ClipboardShortcut.exe --install hello@kiewic.com
    ```
    The installation will add the program to the list of applications that run every time Windows reboots.
3. Then execute the application one more time, but this time without any argument.
    ```
    ClipboardShortcut.exe
    ```
    Or restart your computer.

Ready! Use <kbd>CTRL</kbd> + <kbd>SHIFT</kbd> + <kbd>C</kbd> to copy your value into the clipboard, and then use <kbd>CTRL</kbd> + <kbd>V</kbd> to paste it to any Windows application.

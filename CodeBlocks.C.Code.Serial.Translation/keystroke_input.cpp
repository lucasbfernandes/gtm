//
// keystroke.c - Pauses, then simulates a key press
// and release of the "A" key.
//
// Written by Ted Burke - last updated 17-4-2012
//
// To compile with MinGW:
//
//      gcc -o keystroke.exe keystroke.c
//
// To run the program:
//
//      keystroke.exe
//
// ...then switch to e.g. a Notepad window and wait
// 5 seconds for the A key to be magically pressed.
//

// Because the SendInput function is only supported in
// Windows 2000 and later, WINVER needs to be set as
// follows so that SendInput gets defined when windows.h
// is included below.
#define WINVER 0x0500
#include <windows.h>

/**
    @parameters INPUT structure, WORD wVk (Key).
    @description Function that performs the action of pressing the key specified by WORD wVk.
**/
void press_key(INPUT* in, WORD wVk) {
    ///Press key specified by wvK;
    in->ki.wVk = wVk;
    in->ki.dwFlags = 0;
    SendInput(1, in, sizeof(INPUT));

    ///Release the key.
    in->ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, in, sizeof(INPUT));
}

/*
int main()
{
    // This structure will be used to create the keyboard
    // input event.
    INPUT in;

    // Set up a generic keyboard event.
    in.type = INPUT_KEYBOARD;
    in.ki.wScan = 0; // hardware scan code for key
    in.ki.time = 0;
    in.ki.dwExtraInfo = 0;

    ///Pressing UP Arrow key.
    int i;
    for (i = 0; i < 200; i++) {
        Sleep(30);
        press_key(&in, 0x26);
    }

    // Exit normally
    return 0;
}
*/

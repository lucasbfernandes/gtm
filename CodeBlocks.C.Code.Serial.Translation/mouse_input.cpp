// Because the SendInput function is only supported in
// Windows 2000 and later, WINVER needs to be set as
// follows so that SendInput gets defined when windows.h
// is included below.
#define WINVER 0x0500
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

/**
    @parameters INPUT structure, x position, y position.
    @description Function that moves the mouse to position (x,y).
**/
void mouse_move(INPUT* in, int x, int y) {
    ///Maximum size of the screen.
    double fScreenWidth = GetSystemMetrics( SM_CXSCREEN )-1;
    double fScreenHeight = GetSystemMetrics( SM_CYSCREEN )-1;

    ///(65535,65535) is the bottom right corner of the screen. Here I divided 65535 by the size in pixels of the screen, so that we can
    ///correctly move the mouse using pixel numbers as input.
    in->mi.dx = x*(65535.0f/fScreenWidth);
    in->mi.dy = y*(65535.0f/fScreenHeight);

    ///Values dx and dy are absolute and the mouse has moved.
    in->mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

    ///Perform action.
    SendInput(1, in, sizeof(INPUT));
}

/**
    @parameters INPUT structure.
    @description Function that performs a left click.
**/
void left_click(INPUT* in) {
    ///Left button down and perform action.
    in->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, in, sizeof(INPUT));

    ///Left button up and perform action.
    in->mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, in, sizeof(INPUT));
}

/**
    @parameters INPUT structure.
    @description Function that performs a right click.
**/
void right_click(INPUT* in) {
    ///Right button down and perform action.
    in->mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, in, sizeof(INPUT));

    ///Right button up and perform action.
    in->mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, in, sizeof(INPUT));
}

/*
int main() {
    ///Creating input structure.
    INPUT in;

    ///Initializing struture.
    in.type = INPUT_MOUSE;
    in.mi.time = 0;
    in.mi.dwExtraInfo = 0;
    in.mi.mouseData = 0;

    ///Testing mouse_move.
    int i;
    for (i = 0; i < 500; i++) {
        Sleep(10);
        mouse_move(&in, 10+i, 10+i);
    }

    ///Testing left_click
    printf("Left click in 5 seconds\n");
    Sleep(5000);
    left_click(&in);

    ///Testing right_click.
    printf("Right click in 5 seconds\n");
    Sleep(5000);
    right_click(&in);

    return 0;
}
*/

#define WINVER 0x0500

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

const unsigned int ARDUINO_WAIT_TIME = 2000;
const unsigned int jump_repetitions = 5;
const unsigned int right_repetitions = 5;
const unsigned int left_repetitions = 5;
const unsigned int keyboard_press_delay = 30;
/**
    @parameters INPUT structure, WORD wVk (Key).
    @description Function that performs the action of pressing the key specified by WORD wVk.
**/
void press_key(INPUT* in, WORD wVk) {
    ///Press key specified by wvK;
    in->ki.wVk = wVk;
    in->ki.dwFlags = 0;
    SendInput(1, in, sizeof(INPUT));

    Sleep(keyboard_press_delay);

    ///Release the key.
    in->ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, in, sizeof(INPUT));
}

void executeInput(char c) {
    INPUT in;
    // Set up a generic keyboard event.
    in.type = INPUT_KEYBOARD;
    in.ki.wScan = 0; // hardware scan code for key
    in.ki.time = 0;
    in.ki.dwExtraInfo = 0;

    //press_key(&in, 0x58);
    if (c == '1' || c == '2') {
        int i;
        for (i = 0; i < left_repetitions; i++) {
            press_key(&in, 0x25);
        }
    }
    else if (c == 'Y' || c == 'Z') {
        int i;
        for (i = 0; i < right_repetitions; i++) {
            press_key(&in, 0x27);
        }
    }
    else if (c == 'B' || c == 'C') {
        int i;
        for (i = 0; i < jump_repetitions; i++) {
            press_key(&in, 0x58);
        }
    }
}

int main() {
    HANDLE serialh;
    char* portName;

    portName = "\\\\.\\COM8";
    serialh = CreateFile(
                portName,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
              );

    if (serialh == INVALID_HANDLE_VALUE) {
        //If not success full display an Error
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            //Print Error if neccessary
            printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);
        }
        else {
            printf("ERROR.");
        }
    }
    else {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};

        //Try to get the current
        if (!GetCommState(serialh, &dcbSerialParams)) {
            //If impossible, show an error
            printf("Failed to get current serial parameters.");
        }
        else {
            //Define serialxxxxxxxxx connection parameters for the arduino board
            dcbSerialParams.BaudRate=CBR_2400;
            dcbSerialParams.ByteSize=8;
            dcbSerialParams.StopBits=ONESTOPBIT;
            dcbSerialParams.Parity=NOPARITY;
            //Setting the DTR to Control_Enable ensures that the Arduino is properly
            //reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            //Set the parameters and check for their proper application
            if (!SetCommState(serialh, &dcbSerialParams)) {
                printf("ALERT: Could not set Serial Port parameters");
            }
            else {
                //Flush any remaining characters in the buffers
                PurgeComm(serialh, PURGE_RXCLEAR | PURGE_TXCLEAR);
                //We wait 2s as the arduino board will be reseting
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }

    ///READING

    //Get various information about the connection
    COMSTAT status;
    //Keep track of last error
    DWORD errors;
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead = 1;
    //Length of data that we want to be read in bytes
    unsigned int dataLength = 1;
    //Character that will serve as a reading buffer for the string that we want to form
    char buffer;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(serialh, &errors, &status);

    //Reading forever
    while(1) {
        //Input - B1axE
        //String that will contain data read sent by the microcontroller
        char str[4] = "";
        //Integer that will keep track of the position of the string that we will insert the next character read
        unsigned int currpos = 0;
        //Flag that tells if the algorithm can start reading the meaningful data from the serial port or not
        unsigned short read = 0;
        //Reading one string
        while(1) {
            //If there is at least one byte to be read in the serial port
            if (status.cbInQue > 0) {
                ReadFile(serialh, &buffer, toRead, &bytesRead, NULL);
                //If we read the character that signals the end of the block and it was not garbage, end this loop
                if (buffer == 'E' && read) {
                    str[currpos] = '\0';
                    break;
                }
                //If we read the character that signals the beginning of the string block
                else if (buffer == 'S') {
                    read = 1;
                    continue;
                }
                //If we didn't reach the end of the block, keep reading
                else if (read) {
                    str[currpos] = buffer;
                    currpos++;
                    //printf("%c\n",buffer);
                    //Sleep(500);
                }
            }
        }

        int i;
        for (i = strlen(str) - 1; i >= 0; i--) {
            executeInput(str[i]);
        }
    }
    CloseHandle(serialh);
}

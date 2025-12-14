// CppBridge.cpp - Console bridge for Electron
// Based on VR1CallBackDemo pattern

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <windows.h>

// Include the USBBox protocol header
#include "../EN/64BIT/USBBoxProtocol.h"

// Link the library
#pragma comment(lib, "../EN/64BIT/USBBoxProtocol.lib")

// Global state
static bool g_bEnabled = false;
static int g_nDeviceType = USBBOX_TYPE_F2; // Default F2

// Callback function - outputs JSON to stdout
void CALLBACK USBEventCallback(WORD wEventCode, int nReference, DWORD dwParam, DWORD dwDeviceID)
{
    switch (wEventCode)
    {
    case EVENT_STATE:
        {
            int state = (int)dwParam;
            
            // Call comes - RINGING
            if (state == CH_STATE_RINGON)
            {
                char szCallerID[64] = {0};
                if (UsbBox_GetCallerNumber(nReference, szCallerID) > 0)
                {
                    // Send JSON: Call comes with phone number
                    std::cout << "{\"type\":\"event\",\"status\":\"Ring\",\"line\":" << nReference 
                              << ",\"callerId\":\"" << szCallerID << "\"}" << std::endl;
                }
                else
                {
                    // Send JSON: Call comes but no caller ID yet
                    std::cout << "{\"type\":\"event\",\"status\":\"Ring\",\"line\":" << nReference 
                              << ",\"callerId\":\"\"}" << std::endl;
                }
                std::cout.flush();
            }
            // Call end - FREE
            else if (state == CH_STATE_HOOKON)
            {
                // Send JSON: Call end
                std::cout << "{\"type\":\"event\",\"status\":\"Free\",\"line\":" << nReference << "}" << std::endl;
                std::cout.flush();
            }
        }
        break;
        
    case EVENT_CALLERID:
        {
            // If caller ID comes after ringing started, send update
            char szCallerID[64] = {0};
            if (UsbBox_GetCallerNumber(nReference, szCallerID) > 0)
            {
                std::cout << "{\"type\":\"event\",\"status\":\"Ring\",\"line\":" << nReference 
                          << ",\"callerId\":\"" << szCallerID << "\"}" << std::endl;
                std::cout.flush();
            }
        }
        break;
    }
}

int main(int argc, char* argv[])
{
    // Set stdout to unbuffered for real-time output
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    // Auto-enable device on startup (F2)
    g_nDeviceType = USBBOX_TYPE_F2;
    int result = UsbBox_EnableDevice();
    if (result == 0)
    {
        UsbBox_EventCallBack(USBEventCallback);
        UsbBox_SetDeviceType(g_nDeviceType);
        g_bEnabled = true;
        
        // Send ready status to stderr (not JSON output)
        std::cerr << "USB Bridge started. Device enabled (F2). Listening for calls..." << std::endl;
        std::cerr.flush();
    }
    else
    {
        std::cerr << "Failed to enable device. Error: " << result << std::endl;
        std::cerr.flush();
        return 1;
    }
    
    // Keep running and listening for events
    // Events are sent via callback to stdout as JSON
    while (true)
    {
        Sleep(1000); // Sleep to keep process alive
        // Check if stdin is closed (for pipe detection)
        if (std::cin.eof())
        {
            break;
        }
    }
    
    // Cleanup
    if (g_bEnabled)
    {
        UsbBox_DisableDevice();
    }
    
    return 0;
}


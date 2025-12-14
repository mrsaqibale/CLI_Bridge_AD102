// CppBridgeMenu.cpp - Interactive menu-driven console bridge
// Simple menu interface for testing on real systems

#include <iostream>
#include <string>
#include <cstring>
#include <windows.h>
#include <conio.h>

// Suppress warnings
#pragma warning(disable: 6031)  // Return value ignored for _getch
#pragma warning(push)
#pragma warning(disable: 4091)   // typedef ignored warning
// Include the USBBox protocol header
#include "../EN/64BIT/USBBoxProtocol.h"
#pragma warning(pop)

// Link the library
#pragma comment(lib, "../EN/64BIT/USBBoxProtocol.lib")

// Global state
static bool g_bEnabled = false;
static int g_nDeviceType = USBBOX_TYPE_F2; // Default F2
static int g_nCurrentLine = 0;

// Callback function - displays events to console
void CALLBACK USBEventCallback(WORD wEventCode, int nReference, DWORD dwParam, DWORD dwDeviceID)
{
    switch (wEventCode)
    {
    case EVENT_STATE:
        {
            int state = (int)dwParam;
            std::cout << " | State: ";
            switch (state)
            {
            case CH_STATE_HOOKON:
                // Don't show FREE status (too frequent)
                break;
            case CH_STATE_HOOKOFF:
                std::cout << "OFFHOOK";
                break;
            case CH_STATE_RINGON:
                std::cout << "RINGING";
                // Try to get caller ID when ringing
                {
                    char szCallerID[64] = {0};
                    if (UsbBox_GetCallerNumber(nReference, szCallerID) > 0)
                    {
                        std::cout << " | Caller ID: " << szCallerID;
                    }
                }
                break;
            case CH_STATE_INRECORD:
                std::cout << "INBOUND CALL";
                break;
            case CH_STATE_OUTRECORD:
                std::cout << "OUTBOUND CALL";
                break;
            default:
                std::cout << "UNKNOWN (" << state << ")";
                break;
            }
        }
        break;
        
    case EVENT_CALLERID:
        {
            char szCallerID[64] = {0};
            if (UsbBox_GetCallerNumber(nReference, szCallerID) > 0)
            {
                std::cout << "\n" << std::string(50, '=') << std::endl;
                std::cout << "  CALLER ID RECEIVED - LINE " << nReference << std::endl;
                std::cout << "  NUMBER: " << szCallerID << std::endl;
                std::cout << std::string(50, '=') << std::endl;
            }
        }
        break;
        
    case EVENT_DTMF:
        {
            char szDTMF[64] = {0};
            if (UsbBox_GetDtmfNumber(nReference, szDTMF) > 0)
            {
                std::cout << "\n[LINE " << nReference << "] DTMF Digits: " << szDTMF << std::endl;
            }
        }
        break;
        
    case EVENT_USBCONNECT:
        std::cout << "\n✓ USB Device CONNECTED" << std::endl;
        break;
        
    case EVENT_USBDISCONNECT:
        std::cout << "\n✗ USB Device DISCONNECTED" << std::endl;
        break;
        
    case EVENT_VOLTAGE:
        // Don't show voltage events (too frequent)
        break;
        
    case EVENT_RINGCOUNT:
        // Ring count shown with ringing state, don't duplicate
        break;
        
    case EVENT_MISSEDINBOUNDCALL:
        std::cout << "\n[LINE " << nReference << "] MISSED CALL" << std::endl;
        break;
        
    default:
        // Other events - minimal display
        break;
    }
}

// Clear screen
void ClearScreen()
{
    system("cls");
}

// Display main menu
void ShowMenu()
{
    ClearScreen();
    std::cout << "========================================" << std::endl;
    std::cout << "    USB Caller ID Bridge - Menu" << std::endl;
    std::cout << "    Developed by Saqib Ali (mrsaqibale)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    if (g_bEnabled)
    {
        std::cout << "Status: [ENABLED] | Device Type: F" << (g_nDeviceType + 1) << std::endl;
    }
    else
    {
        std::cout << "Status: [DISABLED]" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Select an option:" << std::endl;
    std::cout << "  1. Enable Device" << std::endl;
    std::cout << "  2. Disable Device" << std::endl;
    std::cout << "  3. Pickup Line" << std::endl;
    std::cout << "  4. Hang Up (Hook On)" << std::endl;
    std::cout << "  5. Dial Number" << std::endl;
    std::cout << "  6. Get Device Status" << std::endl;
    std::cout << "  0. Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter choice: ";
}

// Enable device
void EnableDevice()
{
    ClearScreen();
    std::cout << "========================================" << std::endl;
    std::cout << "         Enable Device" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Select device type
    std::cout << "Select Device Type:" << std::endl;
    std::cout << "  1. F1 (1 line)" << std::endl;
    std::cout << "  2. F2 (2 lines)" << std::endl;
    std::cout << "  3. F4 (4 lines)" << std::endl;
    std::cout << "  4. F8 (8 lines)" << std::endl;
    std::cout << "  0. Cancel" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter choice: ";
    
    int choice;
    std::cin >> choice;
    
    if (choice == 0)
    {
        return;
    }
    
    switch (choice)
    {
    case 1: g_nDeviceType = USBBOX_TYPE_F1; break;
    case 2: g_nDeviceType = USBBOX_TYPE_F2; break;
    case 3: g_nDeviceType = USBBOX_TYPE_F4; break;
    case 4: g_nDeviceType = USBBOX_TYPE_F8; break;
    default:
        std::cout << "Invalid choice. Using F2 (default)." << std::endl;
        g_nDeviceType = USBBOX_TYPE_F2;
        break;
    }
    
    std::cout << std::endl;
    std::cout << "Enabling device (Type: F" << (g_nDeviceType + 1) << ")..." << std::endl;
    
    // Enable device (same pattern as working demos)
    int result = UsbBox_EnableDevice();
    if (result == 0)
    {
        UsbBox_EventCallBack(USBEventCallback);
        UsbBox_SetDeviceType(g_nDeviceType);
        g_bEnabled = true;
        
        // Get device info
        int deviceCount = UsbBox_GetDeviceCount();
        int channelCount = UsbBox_GetChannelCount();
        
        std::cout << "✓ Device enabled successfully!" << std::endl;
        std::cout << "  Device Count: " << deviceCount << std::endl;
        std::cout << "  Channel Count: " << channelCount << std::endl;
        std::cout << std::endl;
        std::cout << "Waiting for events... (Press any key to continue)" << std::endl;
    }
    else
    {
        std::cout << "✗ Failed to enable device. Error code: " << result << std::endl;
        std::cout << "  Make sure:" << std::endl;
        std::cout << "  1. USB device is connected" << std::endl;
        std::cout << "  2. Device drivers are installed" << std::endl;
        std::cout << "  3. Device is not in use by another application" << std::endl;
        std::cout << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
    }
    
    _getch();
}

// Disable device
void DisableDevice()
{
    ClearScreen();
    std::cout << "========================================" << std::endl;
    std::cout << "         Disable Device" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    if (!g_bEnabled)
    {
        std::cout << "Device is already disabled." << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
        (void)_getch();
        return;
    }
    
    std::cout << "Disabling device..." << std::endl;
    UsbBox_DisableDevice();
    g_bEnabled = false;
    std::cout << "✓ Device disabled." << std::endl;
    std::cout << "Press any key to continue..." << std::endl;
    (void)_getch();
}

// Pickup line
void PickupLine()
{
    ClearScreen();
    std::cout << "========================================" << std::endl;
    std::cout << "         Pickup Line" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    if (!g_bEnabled)
    {
        std::cout << "Error: Device is not enabled!" << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
        (void)_getch();
        return;
    }
    
    std::cout << "Enter line number (0-based): ";
    std::cin >> g_nCurrentLine;
    
    std::cout << std::endl;
    std::cout << "Picking up line " << g_nCurrentLine << "..." << std::endl;
    
    int result = UsbBox_Pickup(g_nCurrentLine);
    if (result == 0)
    {
        std::cout << "✓ Line picked up successfully." << std::endl;
    }
    else
    {
        std::cout << "✗ Failed to pick up line. Error code: " << result << std::endl;
    }
    
    std::cout << "Press any key to continue..." << std::endl;
    (void)_getch();
}

// Hang up (Hook On)
void HangUp()
{
    ClearScreen();
    std::cout << "========================================" << std::endl;
    std::cout << "         Hang Up (Hook On)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    if (!g_bEnabled)
    {
        std::cout << "Error: Device is not enabled!" << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
        (void)_getch();
        return;
    }
    
    std::cout << "Enter line number (0-based): ";
    std::cin >> g_nCurrentLine;
    
    std::cout << std::endl;
    std::cout << "Hanging up line " << g_nCurrentLine << "..." << std::endl;
    
    int result = UsbBox_HookOn(g_nCurrentLine);
    if (result == 0)
    {
        std::cout << "✓ Line hung up successfully." << std::endl;
    }
    else
    {
        std::cout << "✗ Failed to hang up line. Error code: " << result << std::endl;
    }
    
    std::cout << "Press any key to continue..." << std::endl;
    (void)_getch();
}

// Dial number
void DialNumber()
{
    ClearScreen();
    std::cout << "========================================" << std::endl;
    std::cout << "         Dial Number" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    if (!g_bEnabled)
    {
        std::cout << "Error: Device is not enabled!" << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
        (void)_getch();
        return;
    }
    
    std::cout << "Enter line number (0-based): ";
    std::cin >> g_nCurrentLine;
    
    std::cout << "Enter phone number to dial: ";
    std::string number;
    std::cin >> number;
    
    std::cout << std::endl;
    std::cout << "Dialing " << number << " on line " << g_nCurrentLine << "..." << std::endl;
    
    char dialBuffer[256] = {0};
    strncpy_s(dialBuffer, number.c_str(), sizeof(dialBuffer) - 1);
    
    int result = UsbBox_Diall(g_nCurrentLine, dialBuffer);
    if (result == 0)
    {
        std::cout << "✓ Dialing started." << std::endl;
    }
    else
    {
        std::cout << "✗ Failed to dial. Error code: " << result << std::endl;
    }
    
    std::cout << "Press any key to continue..." << std::endl;
    (void)_getch();
}

// Get device status
void GetDeviceStatus()
{
    ClearScreen();
    std::cout << "========================================" << std::endl;
    std::cout << "         Device Status" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    if (!g_bEnabled)
    {
        std::cout << "Device is not enabled." << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
        (void)_getch();
        return;
    }
    
    int deviceCount = UsbBox_GetDeviceCount();
    int channelCount = UsbBox_GetChannelCount();
    
    std::cout << "Device Type: F" << (g_nDeviceType + 1) << std::endl;
    std::cout << "Device Count: " << deviceCount << std::endl;
    std::cout << "Channel Count: " << channelCount << std::endl;
    std::cout << std::endl;
    
    if (channelCount > 0)
    {
        std::cout << "Line Status:" << std::endl;
        for (int i = 0; i < channelCount; i++)
        {
            int state = UsbBox_GetState(i);
            int voltage = UsbBox_GetVoltage(i);
            int ringCount = UsbBox_GetRingCount(i);
            
            std::cout << "  Line " << i << ": ";
            switch (state)
            {
            case CH_STATE_HOOKON:
                std::cout << "FREE";
                break;
            case CH_STATE_HOOKOFF:
                std::cout << "OFFHOOK";
                break;
            case CH_STATE_RINGON:
                std::cout << "RINGING";
                break;
            case CH_STATE_INRECORD:
                std::cout << "INBOUND CALL";
                break;
            case CH_STATE_OUTRECORD:
                std::cout << "OUTBOUND CALL";
                break;
            default:
                std::cout << "UNKNOWN (" << state << ")";
                break;
            }
            
            std::cout << " | Voltage: " << voltage << "V | Rings: " << ringCount << std::endl;
            
            // Try to get caller ID
            char szCallerID[64] = {0};
            if (UsbBox_GetCallerNumber(i, szCallerID) > 0)
            {
                std::cout << "    Caller ID: " << szCallerID << std::endl;
            }
        }
    }
    
    std::cout << std::endl;
    std::cout << "Press any key to continue..." << std::endl;
    (void)_getch();
}

int main(int argc, char* argv[])
{
    // Set console title
    SetConsoleTitle(L"USB Caller ID Bridge - Menu");
    
    // Auto-enable device on startup (F2)
    std::cout << "========================================" << std::endl;
    std::cout << "    USB Caller ID Bridge - Menu" << std::endl;
    std::cout << "    Developed by Saqib Ali (mrsaqibale)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Auto-enabling device (F2)..." << std::endl;
    
    g_nDeviceType = USBBOX_TYPE_F2;
    int result = UsbBox_EnableDevice();
    if (result == 0)
    {
        UsbBox_EventCallBack(USBEventCallback);
        UsbBox_SetDeviceType(g_nDeviceType);
        g_bEnabled = true;
        
        int deviceCount = UsbBox_GetDeviceCount();
        int channelCount = UsbBox_GetChannelCount();
        
        std::cout << "✓ Device enabled successfully!" << std::endl;
        std::cout << "  Device Count: " << deviceCount << std::endl;
        std::cout << "  Channel Count: " << channelCount << std::endl;
        std::cout << std::endl;
        std::cout << "Waiting for incoming calls..." << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
    }
    else
    {
        std::cout << "✗ Failed to enable device. Error code: " << result << std::endl;
        std::cout << "  Make sure USB device is connected." << std::endl;
        std::cout << std::endl;
    }
    
    int choice;
    bool running = true;
    
    while (running)
    {
        ShowMenu();
        std::cin >> choice;
        
        switch (choice)
        {
        case 1:
            EnableDevice();
            break;
        case 2:
            DisableDevice();
            break;
        case 3:
            PickupLine();
            break;
        case 4:
            HangUp();
            break;
        case 5:
            DialNumber();
            break;
        case 6:
            GetDeviceStatus();
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "Invalid choice. Press any key to continue..." << std::endl;
            _getch();
            break;
        }
    }
    
    // Cleanup
    if (g_bEnabled)
    {
        UsbBox_DisableDevice();
    }
    
    std::cout << std::endl;
    std::cout << "Exiting..." << std::endl;
    
    return 0;
}


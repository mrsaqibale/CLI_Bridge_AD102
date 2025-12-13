// CppBridge.cpp - Console bridge for Electron
// Based on VR1CallBackDemo pattern

#include <iostream>
#include <string>
#include <sstream>
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
    // Output JSON event to stdout
    std::cout << "{\"type\":\"event\",\"eventCode\":" << wEventCode 
              << ",\"line\":" << nReference 
              << ",\"param\":" << dwParam
              << ",\"deviceID\":" << dwDeviceID;
    
    // Add event-specific data
    switch (wEventCode)
    {
    case EVENT_STATE:
        {
            int state = (int)dwParam;
            std::cout << ",\"state\":" << state;
            
            // Try to get caller ID when ringing
            if (state == CH_STATE_RINGON)
            {
                char szCallerID[64] = {0};
                if (UsbBox_GetCallerNumber(nReference, szCallerID, 64) > 0)
                {
                    std::cout << ",\"callerId\":\"" << szCallerID << "\"";
                }
                std::cout << ",\"status\":\"Ringing\"";
            }
            else if (state == CH_STATE_HOOKON)
            {
                std::cout << ",\"status\":\"Free\"";
            }
        }
        break;
        
    case EVENT_CALLERID:
        {
            char szCallerID[64] = {0};
            if (UsbBox_GetCallerNumber(nReference, szCallerID, 64) > 0)
            {
                std::cout << ",\"callerId\":\"" << szCallerID << "\"";
            }
            std::cout << ",\"status\":\"CallerID\"";
        }
        break;
        
    case EVENT_DTMF:
        {
            char szDTMF[64] = {0};
            if (UsbBox_GetDtmfNumber(nReference, szDTMF, 64) > 0)
            {
                std::cout << ",\"dtmf\":\"" << szDTMF << "\"";
            }
        }
        break;
        
    case USBBOX_PLUG_IN:
        std::cout << ",\"status\":\"Connected\"";
        break;
        
    case USBBOX_PLUG_OUT:
        std::cout << ",\"status\":\"Disconnected\"";
        break;
    }
    
    std::cout << "}" << std::endl;
    std::cout.flush();
}

// Process JSON command from stdin
void ProcessCommand(const std::string& jsonLine)
{
    // Simple JSON parsing (for production, use a proper JSON library)
    // For now, just check for basic commands
    
    if (jsonLine.find("\"cmd\":\"enable\"") != std::string::npos)
    {
        // Extract deviceType if present
        size_t pos = jsonLine.find("\"deviceType\":\"F");
        if (pos != std::string::npos)
        {
            char type = jsonLine[pos + 13]; // "F" + digit
            g_nDeviceType = (type == '1') ? USBBOX_TYPE_F1 :
                          (type == '2') ? USBBOX_TYPE_F2 :
                          (type == '4') ? USBBOX_TYPE_F4 :
                          (type == '8') ? USBBOX_TYPE_F8 : USBBOX_TYPE_F2;
        }
        
        // Enable device (same pattern as working demos)
        int result = UsbBox_EnableDevice();
        if (result == 0)
        {
            UsbBox_EventCallBack(USBEventCallback);
            UsbBox_SetDeviceType(g_nDeviceType);
            g_bEnabled = true;
            
            std::cout << "{\"type\":\"result\",\"ok\":true,\"cmd\":\"enable\"}" << std::endl;
        }
        else
        {
            std::cout << "{\"type\":\"result\",\"ok\":false,\"cmd\":\"enable\",\"error\":" << result << "}" << std::endl;
        }
    }
    else if (jsonLine.find("\"cmd\":\"disable\"") != std::string::npos)
    {
        UsbBox_DisableDevice();
        g_bEnabled = false;
        std::cout << "{\"type\":\"result\",\"ok\":true,\"cmd\":\"disable\"}" << std::endl;
    }
    else if (jsonLine.find("\"cmd\":\"pickup\"") != std::string::npos)
    {
        // Extract line number
        int line = 0;
        size_t pos = jsonLine.find("\"line\":");
        if (pos != std::string::npos)
        {
            line = std::stoi(jsonLine.substr(pos + 7));
        }
        
        int result = UsbBox_Pickup(line);
        std::cout << "{\"type\":\"result\",\"ok\":" << (result == 0 ? "true" : "false") 
                  << ",\"cmd\":\"pickup\"}" << std::endl;
    }
    else if (jsonLine.find("\"cmd\":\"hookon\"") != std::string::npos)
    {
        int line = 0;
        size_t pos = jsonLine.find("\"line\":");
        if (pos != std::string::npos)
        {
            line = std::stoi(jsonLine.substr(pos + 7));
        }
        
        int result = UsbBox_HookOn(line);
        std::cout << "{\"type\":\"result\",\"ok\":" << (result == 0 ? "true" : "false") 
                  << ",\"cmd\":\"hookon\"}" << std::endl;
    }
    else if (jsonLine.find("\"cmd\":\"dial\"") != std::string::npos)
    {
        int line = 0;
        std::string number;
        
        size_t linePos = jsonLine.find("\"line\":");
        if (linePos != std::string::npos)
        {
            line = std::stoi(jsonLine.substr(linePos + 7));
        }
        
        size_t numPos = jsonLine.find("\"number\":\"");
        if (numPos != std::string::npos)
        {
            size_t numEnd = jsonLine.find("\"", numPos + 10);
            if (numEnd != std::string::npos)
            {
                number = jsonLine.substr(numPos + 10, numEnd - numPos - 10);
            }
        }
        
        if (!number.empty())
        {
            int result = UsbBox_Diall(line, number.c_str());
            std::cout << "{\"type\":\"result\",\"ok\":" << (result == 0 ? "true" : "false") 
                      << ",\"cmd\":\"dial\"}" << std::endl;
        }
    }
    
    std::cout.flush();
}

int main(int argc, char* argv[])
{
    // Set stdout to unbuffered for real-time output
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    
    std::cerr << "USB Bridge Console (C++) started." << std::endl;
    std::cerr << "Waiting for JSON commands on stdin..." << std::endl;
    std::cerr.flush();
    
    // Read commands from stdin line by line
    std::string line;
    while (std::getline(std::cin, line))
    {
        if (!line.empty() && line[0] == '{')
        {
            ProcessCommand(line);
        }
    }
    
    // Cleanup
    if (g_bEnabled)
    {
        UsbBox_DisableDevice();
    }
    
    return 0;
}


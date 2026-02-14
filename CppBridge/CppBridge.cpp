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

// Track call state per line to determine call type when call ends
static int g_lineCallState[8] = {0}; // Track state for up to 8 lines

// Helper function to get current timestamp in ISO 8601 format
std::string GetCurrentTimestamp()
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    char buffer[32];
    sprintf_s(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return std::string(buffer);
}

// Escape JSON string
std::string EscapeJsonString(const char* str)
{
    std::string result;
    for (const char* p = str; *p; ++p)
    {
        switch (*p)
        {
        case '"': result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '\b': result += "\\b"; break;
        case '\f': result += "\\f"; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        default: result += *p; break;
        }
    }
    return result;
}

// Callback function - outputs JSON to stdout
void CALLBACK USBEventCallback(WORD wEventCode, int nReference, DWORD dwParam, DWORD dwDeviceID)
{
    std::string timestamp = GetCurrentTimestamp();
    
    switch (wEventCode)
    {
    case EVENT_STATE:
        {
            int state = (int)dwParam;
            
            // Call comes - RINGING
            if (state == CH_STATE_RINGON)
            {
                char szCallerID[64] = {0};
                std::string callerIdStr = "";
                if (UsbBox_GetCallerNumber(nReference, szCallerID) > 0)
                {
                    callerIdStr = EscapeJsonString(szCallerID);
                }
                
                std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":0,\"line\":" << nReference 
                          << ",\"param\":" << state << ",\"deviceId\":" << dwDeviceID
                          << ",\"status\":\"Ringing\",\"callerId\":\"" << callerIdStr 
                          << "\",\"callType\":\"Inbound\",\"ts\":\"" << timestamp << "\"}" << std::endl;
                std::cout.flush();
            }
            // Call active - INBOUND RECORD
            else if (state == CH_STATE_INRECORD)
            {
                // Track that this line has an active inbound call
                if (nReference >= 0 && nReference < 8) {
                    g_lineCallState[nReference] = CH_STATE_INRECORD;
                }
                
                std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":0,\"line\":" << nReference 
                          << ",\"param\":" << state << ",\"deviceId\":" << dwDeviceID
                          << ",\"status\":\"InCall\",\"callType\":\"Inbound\",\"ts\":\"" << timestamp << "\"}" << std::endl;
                std::cout.flush();
                
                // Also check duration immediately when call becomes active
                // (in case EVENT_RECORDTIME doesn't fire automatically)
                int recordTime = UsbBox_GetRecordTime(nReference);
                if (recordTime > 0)
                {
                    std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":5,\"line\":" << nReference 
                              << ",\"param\":0,\"deviceId\":" << dwDeviceID
                              << ",\"duration\":" << recordTime << ",\"callType\":\"Inbound\""
                              << ",\"meaning\":\"Call duration: " << recordTime << " ms\",\"ts\":\"" << timestamp << "\"}" << std::endl;
                    std::cout.flush();
                }
            }
            // Call active - OUTBOUND RECORD
            else if (state == CH_STATE_OUTRECORD)
            {
                // Track that this line has an active outbound call
                if (nReference >= 0 && nReference < 8) {
                    g_lineCallState[nReference] = CH_STATE_OUTRECORD;
                }
                
                std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":0,\"line\":" << nReference 
                          << ",\"param\":" << state << ",\"deviceId\":" << dwDeviceID
                          << ",\"status\":\"InCall\",\"callType\":\"Outbound\",\"ts\":\"" << timestamp << "\"}" << std::endl;
                std::cout.flush();
                
                // Also check duration immediately when call becomes active
                // (in case EVENT_RECORDTIME doesn't fire automatically)
                int recordTime = UsbBox_GetRecordTime(nReference);
                if (recordTime > 0)
                {
                    std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":5,\"line\":" << nReference 
                              << ",\"param\":0,\"deviceId\":" << dwDeviceID
                              << ",\"duration\":" << recordTime << ",\"callType\":\"Outbound\""
                              << ",\"meaning\":\"Call duration: " << recordTime << " ms\",\"ts\":\"" << timestamp << "\"}" << std::endl;
                    std::cout.flush();
                }
            }
            // Call end - FREE
            else if (state == CH_STATE_HOOKON)
            {
                // Determine call type from tracked state
                std::string callType = "Unknown";
                if (nReference >= 0 && nReference < 8) {
                    if (g_lineCallState[nReference] == CH_STATE_INRECORD) {
                        callType = "Inbound";
                    } else if (g_lineCallState[nReference] == CH_STATE_OUTRECORD) {
                        callType = "Outbound";
                    }
                }
                
                // Check duration BEFORE outputting Free status (duration might be cleared after state change)
                int recordTime = UsbBox_GetRecordTime(nReference);
                
                // If we don't know call type and have duration, try to determine from previous state
                if (callType == "Unknown" && recordTime > 0) {
                    // Check if there was a tracked state before resetting
                    if (nReference >= 0 && nReference < 8) {
                        if (g_lineCallState[nReference] == CH_STATE_INRECORD) {
                            callType = "Inbound";
                        } else if (g_lineCallState[nReference] == CH_STATE_OUTRECORD) {
                            callType = "Outbound";
                        } else {
                            // Default to Inbound if we had a caller ID earlier
                            callType = "Inbound";
                        }
                    }
                }
                
                // Output duration if available (before Free status)
                if (recordTime > 0)
                {
                    std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":5,\"line\":" << nReference 
                              << ",\"param\":0,\"deviceId\":" << dwDeviceID
                              << ",\"duration\":" << recordTime << ",\"callType\":\"" << callType 
                              << "\",\"meaning\":\"Call duration: " << recordTime << " ms\",\"ts\":\"" << timestamp << "\"}" << std::endl;
                    std::cout.flush();
                }
                
                // Reset the tracked state
                if (nReference >= 0 && nReference < 8) {
                    g_lineCallState[nReference] = 0;
                }
                
                // Now output the Free status
                std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":0,\"line\":" << nReference 
                          << ",\"param\":" << state << ",\"deviceId\":" << dwDeviceID
                          << ",\"status\":\"Free\",\"ts\":\"" << timestamp << "\"}" << std::endl;
                std::cout.flush();
            }
        }
        break;
        
    case EVENT_CALLERID:
        {
            char szCallerID[64] = {0};
            std::string callerIdStr = "";
            if (UsbBox_GetCallerNumber(nReference, szCallerID) > 0)
            {
                callerIdStr = EscapeJsonString(szCallerID);
            }
            
            std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":2,\"line\":" << nReference 
                      << ",\"param\":0,\"deviceId\":" << dwDeviceID
                      << ",\"callerId\":\"" << callerIdStr << "\",\"callType\":\"Inbound\",\"ts\":\"" << timestamp << "\"}" << std::endl;
            std::cout.flush();
        }
        break;
        
    case EVENT_RECORDTIME:
        {
            int recordTime = UsbBox_GetRecordTime(nReference);
            if (recordTime >= 0)
            {
                int currentState = UsbBox_GetState(nReference);
                std::string callType = "Unknown";
                if (currentState == CH_STATE_INRECORD)
                {
                    callType = "Inbound";
                }
                else if (currentState == CH_STATE_OUTRECORD)
                {
                    callType = "Outbound";
                }
                
                std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":5,\"line\":" << nReference 
                          << ",\"param\":0,\"deviceId\":" << dwDeviceID
                          << ",\"duration\":" << recordTime << ",\"callType\":\"" << callType 
                          << "\",\"meaning\":\"Call duration: " << recordTime << " ms\",\"ts\":\"" << timestamp << "\"}" << std::endl;
                std::cout.flush();
            }
        }
        break;
        
    case EVENT_MISSEDINBOUNDCALL:
        {
            char szCallerID[64] = {0};
            std::string callerIdStr = "";
            if (UsbBox_GetCallerNumber(nReference, szCallerID) > 0)
            {
                callerIdStr = EscapeJsonString(szCallerID);
            }
            
            std::string meaning = "Missed inbound call";
            if (!callerIdStr.empty())
            {
                meaning += " from " + callerIdStr;
            }
            
            std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":11,\"line\":" << nReference 
                      << ",\"param\":0,\"deviceId\":" << dwDeviceID
                      << ",\"status\":\"Missed\",\"callerId\":\"" << callerIdStr 
                      << "\",\"callType\":\"Inbound\",\"meaning\":\"" << EscapeJsonString(meaning.c_str()) 
                      << "\",\"ts\":\"" << timestamp << "\"}" << std::endl;
            std::cout.flush();
            
            // Check if there's actually a duration (call might have been answered despite MISSED event)
            int recordTime = UsbBox_GetRecordTime(nReference);
            if (recordTime > 0)
            {
                // Call was actually answered and had duration
                std::cout << "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":5,\"line\":" << nReference 
                          << ",\"param\":0,\"deviceId\":" << dwDeviceID
                          << ",\"duration\":" << recordTime << ",\"callType\":\"Inbound\""
                          << ",\"meaning\":\"Call duration: " << recordTime << " ms\",\"ts\":\"" << timestamp << "\"}" << std::endl;
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
    
    // Print version to stderr first (so it shows when running exe directly)
    std::cerr << "v2" << std::endl;
    std::cerr.flush();
    
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


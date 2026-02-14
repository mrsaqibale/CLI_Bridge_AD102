// CppBridge.cpp - Console bridge for Electron
// Based on VR1CallBackDemo pattern

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <windows.h>
#include <fstream>
#include <ctime>

// Include the USBBox protocol header
#include "../EN/64BIT/USBBoxProtocol.h"

// Link the library
#pragma comment(lib, "../EN/64BIT/USBBoxProtocol.lib")

// Global state
static bool g_bEnabled = false;
static int g_nDeviceType = USBBOX_TYPE_F2; // Default F2

// Track call state per line to determine call type when call ends
static int g_lineCallState[8] = {0}; // Track state for up to 8 lines

// Log file
static std::ofstream g_logFile;
static bool g_logFileOpen = false;

// Helper to write to log file
void WriteToLog(const std::string& message) {
    if (!g_logFileOpen) {
        // Open log file in append mode
        char logPath[MAX_PATH];
        GetModuleFileNameA(NULL, logPath, MAX_PATH);
        std::string exePath = logPath;
        size_t lastSlash = exePath.find_last_of("\\/");
        std::string logFilePath = (lastSlash != std::string::npos) 
            ? exePath.substr(0, lastSlash + 1) + "CppBridge.log"
            : "CppBridge.log";
        
        g_logFile.open(logFilePath, std::ios::app);
        g_logFileOpen = g_logFile.is_open();
    }
    
    if (g_logFileOpen) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        char timeStr[32];
        sprintf_s(timeStr, sizeof(timeStr), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        
        g_logFile << "[" << timeStr << "] " << message << std::endl;
        g_logFile.flush();
    }
    
    // Also write to stderr for console output
    std::cerr << message << std::endl;
    std::cerr.flush();
}

// Helper to output JSON and log it
void OutputJson(const std::string& json) {
    std::cout << json << std::endl;
    std::cout.flush();
    WriteToLog("JSON: " + json);
}

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
    
    // Log callback received
    char logMsg[256];
    sprintf_s(logMsg, sizeof(logMsg), "CALLBACK: eventCode=%d, line=%d, param=%lu, deviceId=%lu", 
        wEventCode, nReference, dwParam, dwDeviceID);
    WriteToLog(logMsg);
    
    switch (wEventCode)
    {
    case EVENT_STATE:
        {
            int state = (int)dwParam;
            
            // Call answered - OFFHOOK (phone picked up)
            if (state == CH_STATE_HOOKOFF)
            {
                // Track that call was answered
                if (nReference >= 0 && nReference < 8) {
                    // Check if we had a ringing call (inbound) or if it's outbound
                    // For now, assume inbound if we had a caller ID
                    g_lineCallState[nReference] = CH_STATE_INRECORD; // Assume inbound for now
                }
                
                std::string json = "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":0,\"line\":" + std::to_string(nReference) 
                          + ",\"param\":" + std::to_string(state) + ",\"deviceId\":" + std::to_string(dwDeviceID)
                          + ",\"status\":\"Offhook\",\"callType\":\"Inbound\",\"ts\":\"" + timestamp + "\"}";
                OutputJson(json);
                
                // Check duration immediately when call is answered
                int recordTime = UsbBox_GetRecordTime(nReference);
                if (recordTime > 0)
                {
                    std::string durationJson = "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":5,\"line\":" + std::to_string(nReference) 
                              + ",\"param\":0,\"deviceId\":" + std::to_string(dwDeviceID)
                              + ",\"duration\":" + std::to_string(recordTime) + ",\"callType\":\"Inbound\""
                              + ",\"meaning\":\"Call duration: " + std::to_string(recordTime) + " ms\",\"ts\":\"" + timestamp + "\"}";
                    OutputJson(durationJson);
                }
            }
            // Call comes - RINGING
            else if (state == CH_STATE_RINGON)
            {
                char szCallerID[64] = {0};
                std::string callerIdStr = "";
                if (UsbBox_GetCallerNumber(nReference, szCallerID) > 0)
                {
                    callerIdStr = EscapeJsonString(szCallerID);
                }
                
                std::string json = "{\"type\":\"event\",\"api\":\"usbbox\",\"eventCode\":0,\"line\":" + std::to_string(nReference) 
                          + ",\"param\":" + std::to_string(state) + ",\"deviceId\":" + std::to_string(dwDeviceID)
                          + ",\"status\":\"Ringing\",\"callerId\":\"" + callerIdStr 
                          + "\",\"callType\":\"Inbound\",\"ts\":\"" + timestamp + "\"}";
                OutputJson(json);
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
        
    case EVENT_RINGCOUNT:
        {
            // Ring count update - param is the ring count
            // This happens during ringing, we can use it to track that call is still ringing
            // Don't output JSON for every ring count, but log it for debugging
            char logMsg[128];
            sprintf_s(logMsg, sizeof(logMsg), "Ring count: %lu (line %d)", dwParam, nReference);
            WriteToLog(logMsg);
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
    WriteToLog("v2");
    WriteToLog("CppBridge starting...");
    
    // Auto-enable device on startup (F2)
    g_nDeviceType = USBBOX_TYPE_F2;
    WriteToLog("Calling UsbBox_EnableDevice()...");
    int result = UsbBox_EnableDevice();
    WriteToLog("UsbBox_EnableDevice() returned: " + std::to_string(result));
    
    if (result == 0)
    {
        WriteToLog("Setting event callback...");
        UsbBox_EventCallBack(USBEventCallback);
        WriteToLog("Setting device type to F2...");
        UsbBox_SetDeviceType(g_nDeviceType);
        g_bEnabled = true;
        
        // Send ready status
        WriteToLog("USB Bridge started. Device enabled (F2). Listening for calls...");
    }
    else
    {
        WriteToLog("Failed to enable device. Error: " + std::to_string(result));
        if (g_logFileOpen) {
            g_logFile.close();
        }
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
        WriteToLog("Disabling device...");
        UsbBox_DisableDevice();
    }
    
    WriteToLog("CppBridge exiting...");
    if (g_logFileOpen) {
        g_logFile.close();
    }
    
    return 0;
}


#include "BarcodeScanner.h"
#include <windows.h>
#include <chrono>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "ole32.lib")

using namespace barcode;

BarcodeScanner::BarcodeScanner()
    : portHandle(nullptr), currentBaudRate(9600), 
      isListening(false), isConnected(false), 
      eventHandler(nullptr), eventId(0)
{
}

BarcodeScanner::~BarcodeScanner() {
    StopListening();
    Disconnect();
}

bool BarcodeScanner::Connect(const std::string& portName, int baudRate) {
    if (isConnected) {
        Disconnect();
    }
    
    // Append \\.\ prefix for port names on Windows
    std::string fullPortName = portName;
    if (portName.substr(0, 4) != "\\\\.\\") {
        fullPortName = "\\\\.\\" + portName;
    }
    
    // Open serial port
    HANDLE hPort = CreateFileA(
        fullPortName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    
    if (hPort == INVALID_HANDLE_VALUE) {
        lastError = "Failed to open port: " + portName;
        return false;
    }
    
    // Configure port settings
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    
    if (!GetCommState(hPort, &dcbSerialParams)) {
        lastError = "Failed to get comm state";
        CloseHandle(hPort);
        return false;
    }
    
    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.fDsrSensitivity = FALSE;
    dcbSerialParams.fOutxCtsFlow = FALSE;
    dcbSerialParams.fOutxDsrFlow = FALSE;
    dcbSerialParams.fOutX = FALSE;
    dcbSerialParams.fInX = FALSE;
    
    if (!SetCommState(hPort, &dcbSerialParams)) {
        lastError = "Failed to set comm state";
        CloseHandle(hPort);
        return false;
    }
    
    // Set timeouts
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 100;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    
    if (!SetCommTimeouts(hPort, &timeouts)) {
        lastError = "Failed to set timeouts";
        CloseHandle(hPort);
        return false;
    }
    
    portHandle = hPort;
    currentPort = portName;
    currentBaudRate = baudRate;
    isConnected = true;
    
    return true;
}

void BarcodeScanner::Disconnect() {
    StopListening();
    
    if (portHandle != nullptr && portHandle != INVALID_HANDLE_VALUE) {
        CloseHandle((HANDLE)portHandle);
        portHandle = nullptr;
    }
    
    isConnected = false;
    currentPort.clear();
}

std::vector<std::string> BarcodeScanner::GetAvailableCOMPorts() {
    std::vector<std::string> ports;
    
    // Check COM1 through COM9
    for (int i = 1; i <= 9; ++i) {
        std::string portName = "COM" + std::to_string(i);
        
        HANDLE hPort = CreateFileA(
            ("\\\\.\\" + portName).c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );
        
        if (hPort != INVALID_HANDLE_VALUE) {
            ports.push_back(portName);
            CloseHandle(hPort);
        }
    }
    
    return ports;
}

std::vector<std::string> BarcodeScanner::GetAvailableUSBPorts() {
    std::vector<std::string> ports;
    
    // Try COM ports 10-20 as potential USB to serial adapters
    // (COM1-9 are checked by GetAvailableCOMPorts)
    for (int i = 10; i <= 20; ++i) {
        std::string portName = "COM" + std::to_string(i);
        std::string fullPath = "\\\\.\\" + portName;
        
        HANDLE hPort = CreateFileA(fullPath.c_str(), GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                   OPEN_EXISTING, 0, nullptr);
        
        if (hPort != INVALID_HANDLE_VALUE) {
            ports.push_back(portName);
            CloseHandle(hPort);
        }
    }
    
    return ports;
}

bool BarcodeScanner::AutoDetect(int baudRate) {
    // Try COM ports first (legacy barcode readers)
    std::vector<std::string> comPorts = GetAvailableCOMPorts();
    for (const auto& port : comPorts) {
        if (Connect(port, baudRate)) {
            return true;
        }
    }
    
    // Try USB ports
    std::vector<std::string> usbPorts = GetAvailableUSBPorts();
    for (const auto& port : usbPorts) {
        if (Connect(port, baudRate)) {
            return true;
        }
    }
    
    lastError = "No barcode scanner found on available ports";
    return false;
}

bool BarcodeScanner::IsConnected() const {
    return isConnected;
}

void BarcodeScanner::StartListening(wxEvtHandler* handler, int id) {
    if (isListening || !isConnected) {
        return;
    }
    
    eventHandler = handler;
    eventId = id;
    isListening = true;
    
    // Start background reading thread
    listenerThread = std::thread(&BarcodeScanner::ReadingThread, this);
}

void BarcodeScanner::StopListening() {
    if (isListening) {
        isListening = false;
        if (listenerThread.joinable()) {
            listenerThread.join();
        }
    }
}

void BarcodeScanner::ReadingThread() {
    char buffer[1024];
    DWORD bytesRead;
    std::string currentBuffer;
    
    while (isListening && isConnected) {
        if (!ReadFile((HANDLE)portHandle, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
            lastError = "Failed to read from port";
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            currentBuffer += std::string(buffer);
            
            // Check for complete barcode (ends with CR, LF, or both)
            size_t pos;
            while ((pos = currentBuffer.find_first_of("\r\n")) != std::string::npos) {
                std::string barcodeLine = currentBuffer.substr(0, pos);
                currentBuffer.erase(0, pos + 1);
                
                // Parse and trim the barcode
                std::string barcode = ParseBarcodeData(barcodeLine);
                
                if (!barcode.empty()) {
                    BarcodeData data;
                    data.barcode = barcode;
                    data.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();
                    data.portName = currentPort;
                    
                    // Add to queue
                    barcodeQueue.push(data);
                    
                    // Send event to handler if provided
                    if (eventHandler && eventId) {
                        wxCommandEvent evt(eventId);
                        evt.SetString(barcode);
                        eventHandler->ProcessEvent(evt);
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

std::string BarcodeScanner::ParseBarcodeData(const std::string& rawData) {
    // Trim whitespace
    std::string trimmed = rawData;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
    
    // Return if non-empty
    if (!trimmed.empty() && trimmed.length() >= 3) {  // Minimum barcode length
        return trimmed;
    }
    
    return "";
}

bool BarcodeScanner::GetLatestBarcode(BarcodeData& data) {
    return GetBarcodeFromQueue(data);
}

bool BarcodeScanner::GetBarcodeFromQueue(BarcodeData& data) {
    if (!barcodeQueue.empty()) {
        data = barcodeQueue.front();
        barcodeQueue.pop();
        return true;
    }
    return false;
}

bool BarcodeScanner::HasPendingBarcode() const {
    return !barcodeQueue.empty();
}

int BarcodeScanner::GetPendingBarcodeCount() const {
    return static_cast<int>(barcodeQueue.size());
}

std::string BarcodeScanner::GetLastError() const {
    return lastError;
}

std::string BarcodeScanner::GetCurrentPort() const {
    return currentPort;
}

int BarcodeScanner::GetBaudRate() const {
    return currentBaudRate;
}

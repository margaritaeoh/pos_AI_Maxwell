#ifndef BARCODE_SCANNER_H
#define BARCODE_SCANNER_H

#include <wx/wx.h>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>

namespace barcode {

// Port type enumeration for barcode scanners
enum class PortType {
    COM1, COM2, COM3, COM4, COM5,     // Classic serial COM ports
    USB,                               // USB devices (all USB ports)
    AUTO_DETECT                        // Auto-detect available ports
};

// Barcode data structure
struct BarcodeData {
    std::string barcode;
    long timestamp;
    std::string portName;
};

// Barcode Scanner class for reading from laser/USB barcode readers
class BarcodeScanner {
public:
    BarcodeScanner();
    ~BarcodeScanner();
    
    // Connect to a specific port
    bool Connect(const std::string& portName, int baudRate = 9600);
    
    // Disconnect from current port
    void Disconnect();
    
    // Get list of available COM ports
    static std::vector<std::string> GetAvailableCOMPorts();
    
    // Get list of available USB ports
    static std::vector<std::string> GetAvailableUSBPorts();
    
    // Auto-detect and connect to first available barcode reader
    bool AutoDetect(int baudRate = 9600);
    
    // Check if scanner is connected
    bool IsConnected() const;
    
    // Start listening for barcodes in background thread
    void StartListening(wxEvtHandler* eventHandler, int eventId);
    
    // Stop listening
    void StopListening();
    
    // Get latest barcode (non-blocking)
    bool GetLatestBarcode(BarcodeData& data);
    
    // Get barcode from queue (for thread-safe access)
    bool GetBarcodeFromQueue(BarcodeData& data);
    
    // Check if there are pending barcodes
    bool HasPendingBarcode() const;
    
    // Get number of pending barcodes
    int GetPendingBarcodeCount() const;
    
    // Get last error message
    std::string GetLastError() const;
    
    // Get current port name
    std::string GetCurrentPort() const;
    
    // Get baud rate
    int GetBaudRate() const;

private:
    void* portHandle;                     // Native port handle (HANDLE on Windows)
    std::string currentPort;
    int currentBaudRate;
    std::atomic<bool> isListening;
    std::atomic<bool> isConnected;
    std::thread listenerThread;
    std::queue<BarcodeData> barcodeQueue;
    std::string lastError;
    wxEvtHandler* eventHandler;
    int eventId;
    
    // Reading thread function
    void ReadingThread();
    
    // Parse barcode data (handle different formats)
    std::string ParseBarcodeData(const std::string& rawData);
};

}  // namespace barcode

#endif // BARCODE_SCANNER_H

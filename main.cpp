#include <wx/wx.h>
#include "ui/LoginWindow.h"
#include <iostream>
#include <fstream>

class POSApp : public wxApp {
public:
    virtual bool OnInit() override {
        try {
            // Write to a log file to track initialization
            std::ofstream log("pos_startup.log", std::ios::app);
            log << "POS Application starting...\n";
            log.flush();
            
            LoginWindow* login = new LoginWindow(nullptr);
            log << "LoginWindow created successfully\n";
            log.flush();
            
            login->Show();
            log << "LoginWindow shown successfully\n";
            log.flush();
            
            return true;
        } catch (const std::exception& e) {
            std::ofstream log("pos_startup.log", std::ios::app);
            log << "EXCEPTION in OnInit: " << e.what() << "\n";
            log.flush();
            wxMessageBox(wxString::FromUTF8(e.what()), "Error", wxOK | wxICON_ERROR);
            return false;
        } catch (...) {
            std::ofstream log("pos_startup.log", std::ios::app);
            log << "UNKNOWN EXCEPTION in OnInit\n";
            log.flush();
            wxMessageBox("Unknown error during startup", "Error", wxOK | wxICON_ERROR);
            return false;
        }
    }
};

wxIMPLEMENT_APP(POSApp);
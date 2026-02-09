#include <wx/wx.h>
#include <iostream>
#include <fstream>

class TestFrame : public wxFrame {
public:
    TestFrame(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300))
    {
        std::ofstream log("test_ui.log", std::ios::app);
        log << "TestFrame constructor called\n";
        log.flush();

        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        sizer->Add(new wxStaticText(panel, wxID_ANY, "Test UI Application"), 0, wxALL, 10);
        sizer->Add(new wxStaticText(panel, wxID_ANY, "If you see this, wxWidgets is working!"), 0, wxALL, 10);

        wxButton* btn = new wxButton(panel, wxID_ANY, "Click Me");
        sizer->Add(btn, 0, wxALIGN_CENTER | wxALL, 10);

        btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
            std::ofstream log("test_ui.log", std::ios::app);
            log << "Button clicked\n";
            log.flush();
            wxMessageBox("Button works!");
        });

        panel->SetSizer(sizer);

        log << "TestFrame constructor completed\n";
        log.flush();
    }
};

class TestApp : public wxApp {
public:
    virtual bool OnInit() override {
        try {
            std::ofstream log("test_ui.log", std::ios::app);
            log << "=== TEST_UI STARTUP ===\n";
            log << "TestApp::OnInit() called\n";
            log.flush();

            TestFrame* frame = new TestFrame("Simple Test");
            log << "TestFrame created\n";
            log.flush();

            frame->Show();
            log << "TestFrame shown\n";
            log.flush();

            return true;
        } catch (const std::exception& e) {
            std::ofstream log("test_ui.log", std::ios::app);
            log << "EXCEPTION: " << e.what() << "\n";
            log.flush();
            return false;
        } catch (...) {
            std::ofstream log("test_ui.log", std::ios::app);
            log << "UNKNOWN EXCEPTION\n";
            log.flush();
            return false;
        }
    }
};

wxIMPLEMENT_APP(TestApp);

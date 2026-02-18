#include "LoginWindow.h"
#include "MainWindow.h"
#include "SignUpWindow.h"
#include "ThemeHelper.h"
#include <iostream>

LoginWindow::LoginWindow(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, wxString::FromUTF8("Inicio de Sesión"), 
              wxDefaultPosition, wxSize(450, 400)),
      archivoUsuarios("data/usuarios.csv"),
      auth(archivoUsuarios)
{
    try {
        auth.cargar();
    } catch (const std::exception& e) {
        std::cerr << "Error loading users: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error loading users" << std::endl;
    }

    wxPanel* panel = new wxPanel(this);
    theme::Styling::ApplyDarkTheme(panel);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->AddStretchSpacer(1);

    // Title
    wxStaticText* lblTitle = new wxStaticText(panel, wxID_ANY, 
        wxString::FromUTF8("Sistema POS"));
    theme::Styling::StyleAsTitle(lblTitle);
    mainSizer->Add(lblTitle, 0, wxALIGN_CENTER | wxALL, 15);

    // Subtitle
    wxStaticText* lblSubtitle = new wxStaticText(panel, wxID_ANY, 
        wxString::FromUTF8("Iniciar Sesión"));
    theme::Styling::StyleAsHeader(lblSubtitle);
    mainSizer->Add(lblSubtitle, 0, wxALIGN_CENTER | wxALL, 10);

    // Username label and input
    wxStaticText* lblUsuario = new wxStaticText(panel, wxID_ANY, 
        wxString::FromUTF8("Usuario:"));
    theme::Styling::StyleAsLabel(lblUsuario);
    mainSizer->Add(lblUsuario, 0, wxALL, 8);
    
    txtUsuario = new wxTextCtrl(panel, wxID_ANY);
    theme::Styling::StyleTextControl(txtUsuario);
    mainSizer->Add(txtUsuario, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    // Password label and input
    wxStaticText* lblPassword = new wxStaticText(panel, wxID_ANY, 
        wxString::FromUTF8("Contraseña:"));
    theme::Styling::StyleAsLabel(lblPassword);
    mainSizer->Add(lblPassword, 0, wxALL, 8);
    
    txtPassword = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    theme::Styling::StyleTextControl(txtPassword);
    mainSizer->Add(txtPassword, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

    // Checkbox for users that don't require password
    chkSinPassword = new wxCheckBox(panel, wxID_ANY, 
        wxString::FromUTF8("Entrar sin contraseña"));
    chkSinPassword->SetForegroundColour(theme::Colors::TEXT_SECONDARY);
    mainSizer->Add(chkSinPassword, 0, wxALL, 8);
    chkSinPassword->Bind(wxEVT_CHECKBOX, &LoginWindow::OnPasswordCheckboxToggle, this);

    // Button sizer
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxButton* btnLogin = new wxButton(panel, wxID_ANY, 
        wxString::FromUTF8("Ingresar"));
    theme::Styling::StyleButtonAccent(btnLogin);
    
    wxButton* btnSignUp = new wxButton(panel, wxID_ANY, 
        wxString::FromUTF8("Registrarse"));
    theme::Styling::StyleButtonSecondary(btnSignUp);
    
    buttonSizer->Add(btnLogin, 1, wxEXPAND | wxALL, 8);
    buttonSizer->Add(btnSignUp, 1, wxEXPAND | wxALL, 8);
    mainSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 5);

    // Error label
    lblError = new wxStaticText(panel, wxID_ANY, "");
    lblError->SetForegroundColour(theme::Colors::COLOR_ERROR);
    lblError->SetFont(theme::Fonts::GetLabelFont());
    mainSizer->Add(lblError, 0, wxALIGN_CENTER | wxALL, 10);

    mainSizer->AddStretchSpacer(1);

    btnLogin->Bind(wxEVT_BUTTON, &LoginWindow::OnLogin, this);
    btnSignUp->Bind(wxEVT_BUTTON, &LoginWindow::OnSignUp, this);

    panel->SetSizer(mainSizer);
}

void LoginWindow::OnPasswordCheckboxToggle(wxCommandEvent& evt) {
    if (chkSinPassword->GetValue()) {
        txtPassword->Enable(false);
        txtPassword->SetValue("");
    } else {
        txtPassword->Enable(true);
    }
}

void LoginWindow::OnLogin(wxCommandEvent& evt) {
    std::string user = txtUsuario->GetValue().ToStdString();
    std::string pass = txtPassword->GetValue().ToStdString();

    auto resultado = auth.login(user, pass);

    if (!resultado.has_value()) {
        lblError->SetLabel(wxString::FromUTF8("Usuario o contraseña incorrectos"));
        return;
    }

    MainWindow* main = new MainWindow(nullptr, resultado.value());
    main->Show();
    Close();
}

void LoginWindow::OnSignUp(wxCommandEvent& evt) {
    SignUpWindow* signup = new SignUpWindow(this, auth);
    signup->ShowModal();
    
    // Reload users after signup
    try {
        auth.cargar();
    } catch (const std::exception& e) {
        std::cerr << "Error reloading users: " << e.what() << std::endl;
    }
}
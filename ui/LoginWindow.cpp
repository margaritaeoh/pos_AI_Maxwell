#include "LoginWindow.h"
#include "MainWindow.h"
#include <iostream>

wxString titulo_utf8 = wxString::FromUTF8("Inicio de Sesión");
wxString password_utf8 = wxString::FromUTF8("Contraseña:");
wxString warning_utf8 = wxString::FromUTF8("Usuario o contraseña incorrectos");

LoginWindow::LoginWindow(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, titulo_utf8, wxDefaultPosition, wxSize(350, 250)),
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

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Usuario:"), 0, wxALL, 5);
    txtUsuario = new wxTextCtrl(panel, wxID_ANY);
    sizer->Add(txtUsuario, 0, wxEXPAND | wxALL, 5);

    sizer->Add(new wxStaticText(panel, wxID_ANY, password_utf8), 0, wxALL, 5);
	txtPassword = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    sizer->Add(txtPassword, 0, wxEXPAND | wxALL, 5);

    wxButton* btnLogin = new wxButton(panel, wxID_ANY, "Ingresar");
    sizer->Add(btnLogin, 0, wxALIGN_CENTER | wxALL, 10);

    lblError = new wxStaticText(panel, wxID_ANY, "");
    lblError->SetForegroundColour(*wxRED);
    sizer->Add(lblError, 0, wxALIGN_CENTER | wxALL, 5);

    btnLogin->Bind(wxEVT_BUTTON, &LoginWindow::OnLogin, this);

    panel->SetSizer(sizer);
}

void LoginWindow::OnLogin(wxCommandEvent& evt) {
    std::string user = txtUsuario->GetValue().ToStdString();
    std::string pass = txtPassword->GetValue().ToStdString();

    auto resultado = auth.login(user, pass);

    if (!resultado.has_value()) {
        lblError->SetLabel(warning_utf8);
        return;
    }

    MainWindow* main = new MainWindow(nullptr, resultado.value());
    main->Show();
    Close();
}
#include "SignUpWindow.h"
#include <iostream>

wxString titulo_signup = wxString::FromUTF8("Registrar Usuario");
wxString lbl_username = wxString::FromUTF8("Usuario:");
wxString lbl_password = wxString::FromUTF8("Contraseña:");
wxString lbl_confirm = wxString::FromUTF8("Confirmar Contraseña:");
wxString lbl_rol = wxString::FromUTF8("Rol:");
wxString lbl_require_pass = wxString::FromUTF8("Requiere Contraseña:");

SignUpWindow::SignUpWindow(wxWindow* parent, pos::Autenticacion& auth)
    : wxDialog(parent, wxID_ANY, titulo_signup, wxDefaultPosition, wxSize(450, 400)),
      auth(auth)
{
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Username
    sizer->Add(new wxStaticText(panel, wxID_ANY, lbl_username), 0, wxALL, 5);
    txtUsername = new wxTextCtrl(panel, wxID_ANY);
    sizer->Add(txtUsername, 0, wxEXPAND | wxALL, 5);

    // Password
    sizer->Add(new wxStaticText(panel, wxID_ANY, lbl_password), 0, wxALL, 5);
    txtPassword = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    sizer->Add(txtPassword, 0, wxEXPAND | wxALL, 5);

    // Confirm Password
    sizer->Add(new wxStaticText(panel, wxID_ANY, lbl_confirm), 0, wxALL, 5);
    txtPasswordConfirm = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    sizer->Add(txtPasswordConfirm, 0, wxEXPAND | wxALL, 5);

    // Role Selection
    sizer->Add(new wxStaticText(panel, wxID_ANY, lbl_rol), 0, wxALL, 5);
    chcRol = new wxChoice(panel, wxID_ANY);
    chcRol->Append(wxString::FromUTF8("Usuario Estándar"));
    chcRol->Append("Administrador");
    chcRol->SetSelection(0);
    sizer->Add(chcRol, 0, wxEXPAND | wxALL, 5);
    chcRol->Bind(wxEVT_CHOICE, &SignUpWindow::OnRoleChange, this);

    // Require Password Checkbox
    chkRequirePassword = new wxCheckBox(panel, wxID_ANY, lbl_require_pass);
    chkRequirePassword->SetValue(false);  // Default: no password required
    sizer->Add(chkRequirePassword, 0, wxALL, 5);

    // Error message
    lblError = new wxStaticText(panel, wxID_ANY, "");
    lblError->SetForegroundColour(*wxRED);
    sizer->Add(lblError, 0, wxALIGN_CENTER | wxALL, 5);

    // Success message
    lblSuccess = new wxStaticText(panel, wxID_ANY, "");
    lblSuccess->SetForegroundColour(*wxGREEN);
    sizer->Add(lblSuccess, 0, wxALIGN_CENTER | wxALL, 5);

    // Buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnRegister = new wxButton(panel, wxID_ANY, 
        wxString::FromUTF8("Registrarse"));
    wxButton* btnCancel = new wxButton(panel, wxID_CANCEL, 
        wxString::FromUTF8("Cancelar"));

    buttonSizer->Add(btnRegister, 0, wxALL, 5);
    buttonSizer->Add(btnCancel, 0, wxALL, 5);
    sizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 5);

    btnRegister->Bind(wxEVT_BUTTON, &SignUpWindow::OnRegister, this);
    btnCancel->Bind(wxEVT_BUTTON, &SignUpWindow::OnCancel, this);

    panel->SetSizer(sizer);
}

void SignUpWindow::OnRoleChange(wxCommandEvent& evt) {
    int selection = chcRol->GetSelection();
    
    // If Admin is selected, force password requirement
    if (selection == 1) {  // Admin
        chkRequirePassword->SetValue(true);
        chkRequirePassword->Enable(false);
    } else {  // Standard user
        chkRequirePassword->Enable(true);
    }
}

void SignUpWindow::OnRegister(wxCommandEvent& evt) {
    std::string username = txtUsername->GetValue().ToStdString();
    std::string password = txtPassword->GetValue().ToStdString();
    std::string confirmPassword = txtPasswordConfirm->GetValue().ToStdString();
    
    // Validation
    if (username.empty()) {
        lblError->SetLabel(wxString::FromUTF8("El usuario no puede estar vacío"));
        return;
    }
    
    if (password.empty()) {
        lblError->SetLabel(wxString::FromUTF8("La contraseña no puede estar vacía"));
        return;
    }
    
    if (password != confirmPassword) {
        lblError->SetLabel(wxString::FromUTF8("Las contraseñas no coinciden"));
        return;
    }
    
    // Check if user already exists
    if (auth.verificarUsuarioExiste(username)) {
        lblError->SetLabel(wxString::FromUTF8("El usuario ya existe"));
        return;
    }
    
    // Get role
    int roleSelection = chcRol->GetSelection();
    pos::RolUsuario rol = (roleSelection == 1) ? pos::RolUsuario::Administrador : pos::RolUsuario::Usuario;
    
    // Determine if password is required
    bool requierePassword = chkRequirePassword->GetValue();
    
    // Register user
    if (auth.registrarUsuario(username, password, rol, requierePassword)) {
        if (auth.guardar()) {
            lblSuccess->SetLabel(wxString::FromUTF8("Usuario registrado exitosamente"));
            lblError->SetLabel("");
            
            // Clear fields
            txtUsername->SetValue("");
            txtPassword->SetValue("");
            txtPasswordConfirm->SetValue("");
            
            // Close after 2 seconds
            wxMilliSleep(1500);
            EndModal(wxID_OK);
        } else {
            lblError->SetLabel(wxString::FromUTF8("Error al guardar el usuario"));
        }
    } else {
        lblError->SetLabel(wxString::FromUTF8("Error al registrar el usuario"));
    }
}

void SignUpWindow::OnCancel(wxCommandEvent& evt) {
    EndModal(wxID_CANCEL);
}

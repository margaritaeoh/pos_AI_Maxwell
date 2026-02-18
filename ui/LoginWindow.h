#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <wx/wx.h>
#include "../negocio/Autenticacion.h"
#include "../datos/ArchivoUsuarios.h"

class LoginWindow : public wxFrame {
public:
    LoginWindow(wxWindow* parent);

private:
    wxTextCtrl* txtUsuario;
    wxTextCtrl* txtPassword;
    wxStaticText* lblError;
    wxCheckBox* chkSinPassword;

    pos::ArchivoUsuarios archivoUsuarios;
    pos::Autenticacion auth;

    void OnLogin(wxCommandEvent& evt);
    void OnSignUp(wxCommandEvent& evt);
    void OnPasswordCheckboxToggle(wxCommandEvent& evt);
};

#endif
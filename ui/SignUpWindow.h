#ifndef SIGNUP_WINDOW_H
#define SIGNUP_WINDOW_H

#include <wx/wx.h>
#include "../negocio/Autenticacion.h"

class SignUpWindow : public wxDialog {
public:
    SignUpWindow(wxWindow* parent, pos::Autenticacion& auth);

private:
    wxTextCtrl* txtUsername;
    wxTextCtrl* txtPassword;
    wxTextCtrl* txtPasswordConfirm;
    wxChoice* chcRol;
    wxCheckBox* chkRequirePassword;
    wxStaticText* lblError;
    wxStaticText* lblSuccess;

    pos::Autenticacion& auth;

    void OnRegister(wxCommandEvent& evt);
    void OnCancel(wxCommandEvent& evt);
    void OnRoleChange(wxCommandEvent& evt);
};

#endif

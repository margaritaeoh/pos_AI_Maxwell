#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include "../modelos/Usuario.h"

class MainWindow : public wxFrame {
public:
    MainWindow(wxWindow* parent, const pos::Usuario& usuario);

private:
    pos::Usuario usuarioActual;
    wxNotebook* notebook;

    bool darkMode = false;

    void OnCerrarSesion(wxCommandEvent& evt);
    void OnToggleTheme(wxCommandEvent& evt);
};

#endif
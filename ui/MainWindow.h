#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include "../modelos/Usuario.h"

// Forward declarations
class MenuVentas;
class MenuInventario;
class MenuReportes;

class MainWindow : public wxFrame {
public:
    MainWindow(wxWindow* parent, const pos::Usuario& usuario);

private:
    pos::Usuario usuarioActual;
    wxNotebook* notebook;
    wxPanel* mainPanel;
    MenuVentas* ventasPanel;
    MenuInventario* inventarioPanel;
    MenuReportes* reportesPanel;

    // Event handlers
    void OnCerrarSesion(wxCommandEvent& evt);
    void OnExit(wxCommandEvent& evt);
    void OnExportData(wxCommandEvent& evt);
    void OnViewHelp(wxCommandEvent& evt);
    void OnManageUsers(wxCommandEvent& evt);
    void OnAbout(wxCommandEvent& evt);
    void OnToggleTheme(wxCommandEvent& evt);
    void OnCustomizeTheme(wxCommandEvent& evt);
    
    // Helper methods
    void CreateMenuBar();
    void OpenHelpFile();
    void RefreshAllThemes();
};

#endif
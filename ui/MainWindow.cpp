#include "MainWindow.h"
#include "LoginWindow.h"
#include "MenuVentas.h"
#include "MenuInventario.h"
#include "MenuReportes.h"
#include "ThemeHelper.h"
#include "UICustomizationDialog.h"
#include <wx/textfile.h>
#include <wx/process.h>
#include <fstream>

enum MenuIds {
    ID_EXIT = wxID_HIGHEST + 1,
    ID_EXPORT_DATA,
    ID_TOGGLE_THEME,
    ID_CUSTOMIZE_THEME,
    ID_MANAGE_USERS,
    ID_VIEW_HELP,
    ID_ABOUT
};

MainWindow::MainWindow(wxWindow* parent, const pos::Usuario& usuario)
    : wxFrame(parent, wxID_ANY, 
              wxString::FromUTF8("Punto de Venta"),
              wxDefaultPosition, wxSize(1280, 800),
              wxDEFAULT_FRAME_STYLE | wxMAXIMIZE | wxMINIMIZE),
      usuarioActual(usuario)
{
    Maximize(true);

    // Create menu bar
    CreateMenuBar();

    mainPanel = new wxPanel(this);
    theme::Styling::ApplyDarkTheme(mainPanel);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ---------------------------
    // Top bar with user info and buttons
    // ---------------------------
    wxBoxSizer* topBar = new wxBoxSizer(wxHORIZONTAL);
    topBar->SetMinSize(wxSize(-1, 50));

    std::string rolStr = (usuarioActual.rol == pos::RolUsuario::Administrador) ? 
        "Administrador" : "Usuario Estándar";
    
    wxStaticText* lblUsuario = new wxStaticText(mainPanel, wxID_ANY,
        wxString::FromUTF8("Usuario: ") + usuarioActual.username + 
        wxString::FromUTF8(" (") + rolStr + ")");
    lblUsuario->SetFont(theme::Fonts::GetHeaderFont());
    lblUsuario->SetForegroundColour(theme::Colors::TEXT_ACCENT);

    wxButton* btnCerrarSesion = new wxButton(mainPanel, wxID_ANY, 
        wxString::FromUTF8("Cerrar Sesión"));
    theme::Styling::StyleButtonSecondary(btnCerrarSesion);

    topBar->Add(lblUsuario, 1, wxALIGN_CENTER_VERTICAL | wxALL, 10);
    topBar->AddStretchSpacer();
    topBar->Add(btnCerrarSesion, 0, wxALIGN_RIGHT | wxALL, 5);

    mainSizer->Add(topBar, 0, wxEXPAND | wxBOTTOM, 5);

    // ---------------------------
    // Notebook (tabs) with dynamic sizing
    // ---------------------------
    notebook = new wxNotebook(mainPanel, wxID_ANY);
    theme::Styling::ApplyDarkTheme(notebook);

    ventasPanel = new MenuVentas(notebook, usuarioActual);
    inventarioPanel = new MenuInventario(notebook, usuarioActual);
    reportesPanel = nullptr;

    notebook->AddPage(ventasPanel, wxString::FromUTF8("Ventas"));
    notebook->AddPage(inventarioPanel, wxString::FromUTF8("Inventario"));
    
    // Only admin can access Reports
    if (usuarioActual.rol == pos::RolUsuario::Administrador) {
        reportesPanel = new MenuReportes(notebook);
        notebook->AddPage(reportesPanel, wxString::FromUTF8("Reportes"));
    }

    mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

    // ---------------------------
    // Event Bindings
    // ---------------------------
    btnCerrarSesion->Bind(wxEVT_BUTTON, &MainWindow::OnCerrarSesion, this);

    mainPanel->SetSizer(mainSizer);
    SetMinSize(wxSize(900, 650));  // Minimum window size for usability
}

void MainWindow::CreateMenuBar() {
    wxMenuBar* menuBar = new wxMenuBar();

    // File Menu
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ID_EXPORT_DATA, 
        wxString::FromUTF8("&Exportar datos\tCtrl+E"),
        wxString::FromUTF8("Exportar datos del sistema"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, 
        wxString::FromUTF8("&Salir\tCtrl+Q"),
        wxString::FromUTF8("Salir de la aplicación"));

    // View Menu
    wxMenu* viewMenu = new wxMenu();
    viewMenu->Append(ID_TOGGLE_THEME,
        wxString::FromUTF8("&Cambiar Tema\tCtrl+T"),
        wxString::FromUTF8("Cambiar entre tema oscuro y claro"));
    viewMenu->Append(ID_CUSTOMIZE_THEME,
        wxString::FromUTF8("&Personalizar Vistas\tCtrl+P"),
        wxString::FromUTF8("Personalizar colores, fuentes y estilos"));

    // Tools Menu
    wxMenu* toolsMenu = new wxMenu();
    if (usuarioActual.rol == pos::RolUsuario::Administrador) {
        toolsMenu->Append(ID_MANAGE_USERS, 
            wxString::FromUTF8("&Administrar Usuarios\tCtrl+U"),
            wxString::FromUTF8("Gestionar usuarios del sistema"));
    }

    // Help Menu
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(ID_VIEW_HELP, 
        wxString::FromUTF8("&Ver Ayuda\tF1"),
        wxString::FromUTF8("Abrir archivo de ayuda"));
    helpMenu->AppendSeparator();
    helpMenu->Append(ID_ABOUT, 
        wxString::FromUTF8("&Acerca de..."),
        wxString::FromUTF8("Información sobre el POS"));

    // Add menus to menu bar
    menuBar->Append(fileMenu, wxString::FromUTF8("&Archivo"));
    menuBar->Append(viewMenu, wxString::FromUTF8("&Vista"));
    if (usuarioActual.rol == pos::RolUsuario::Administrador) {
        menuBar->Append(toolsMenu, wxString::FromUTF8("&Herramientas"));
    }
    menuBar->Append(helpMenu, wxString::FromUTF8("A&yuda"));

    SetMenuBar(menuBar);

    // Bind menu events
    Bind(wxEVT_MENU, &MainWindow::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainWindow::OnExportData, this, ID_EXPORT_DATA);
    Bind(wxEVT_MENU, &MainWindow::OnToggleTheme, this, ID_TOGGLE_THEME);
    Bind(wxEVT_MENU, &MainWindow::OnCustomizeTheme, this, ID_CUSTOMIZE_THEME);
    Bind(wxEVT_MENU, &MainWindow::OnManageUsers, this, ID_MANAGE_USERS);
    Bind(wxEVT_MENU, &MainWindow::OnViewHelp, this, ID_VIEW_HELP);
    Bind(wxEVT_MENU, &MainWindow::OnAbout, this, ID_ABOUT);
    
    // Bind F1 key for help
    Bind(wxEVT_CHAR_HOOK, [this](wxKeyEvent& event) {
        if (event.GetKeyCode() == WXK_F1) {
            OpenHelpFile();
        } else {
            event.Skip();
        }
    });
}

void MainWindow::OnCerrarSesion(wxCommandEvent& evt) {
    Close();
    LoginWindow* login = new LoginWindow(nullptr);
    login->Show();
}

void MainWindow::OnExit(wxCommandEvent& evt) {
    Close(true);
}

void MainWindow::OnExportData(wxCommandEvent& evt) {
    wxMessageBox(
        wxString::FromUTF8("Función de exportación disponible en el módulo de Reportes.\n"
        "Seleccione la pestaña 'Reportes' para exportar datos."),
        wxString::FromUTF8("Exportar Datos"),
        wxOK | wxICON_INFORMATION);
}

void MainWindow::OnManageUsers(wxCommandEvent& evt) {
    wxMessageBox(
        wxString::FromUTF8("Funcionalidad de administración de usuarios disponible.\n"
        "Use la ventana de Registro para crear nuevos usuarios.\n"
        "Para eliminar usuarios, modifique el archivo de usuarios."),
        wxString::FromUTF8("Administrar Usuarios"),
        wxOK | wxICON_INFORMATION);
}

void MainWindow::OpenHelpFile() {
    wxString helpPath = "data/AYUDA.txt";
    
    // Try to open the help file
    if (wxFileExists(helpPath)) {
        // Try to open with default text editor
        #ifdef __WXMSW__
            wxExecute("notepad.exe " + helpPath);
        #elif __WXMAC__
            wxExecute("open " + helpPath);
        #else
            wxExecute("xdg-open " + helpPath);
        #endif
    } else {
        wxMessageBox(
            wxString::FromUTF8("No se pudo encontrar el archivo de ayuda.\n"
            "Ubicación esperada: ") + helpPath,
            wxString::FromUTF8("Error"),
            wxOK | wxICON_ERROR);
    }
}

void MainWindow::OnViewHelp(wxCommandEvent& evt) {
    OpenHelpFile();
}

void MainWindow::OnAbout(wxCommandEvent& evt) {
    wxMessageBox(
        wxString::FromUTF8("Sistema de Punto de Venta (POS)\n"
        "Versión 1.0\n\n"
        "Un sistema completo para gestionar ventas, inventario y reportes.\n\n"
        "© 2026 - Todos los derechos reservados\n\n"
        "Para ayuda, presione F1 o vaya a Ayuda > Ver Ayuda"),
        wxString::FromUTF8("Acerca de POS"),
        wxOK | wxICON_INFORMATION);
}

void MainWindow::OnToggleTheme(wxCommandEvent& evt) {
    // Toggle theme
    theme::ThemeType currentTheme = theme::ThemeManager::GetCurrentTheme();
    theme::ThemeType newTheme = (currentTheme == theme::ThemeType::DARK) ? 
                                theme::ThemeType::LIGHT : 
                                theme::ThemeType::DARK;
    
    theme::ThemeManager::SetCurrentTheme(newTheme);
    RefreshAllThemes();
    
    wxMessageBox(
        newTheme == theme::ThemeType::DARK ? 
            wxString::FromUTF8("Tema oscuro activado") :
            wxString::FromUTF8("Tema claro activado"),
        wxString::FromUTF8("Cambio de Tema"),
        wxOK | wxICON_INFORMATION);
}

void MainWindow::RefreshAllThemes() {
    // Refresh main panel
    theme::Styling::ApplyActiveTheme(mainPanel);
    theme::Styling::ApplyActiveTheme(notebook);
    
    // Refresh all panels
    if (ventasPanel) {
        theme::Styling::ApplyActiveTheme(ventasPanel);
    }
    if (inventarioPanel) {
        theme::Styling::ApplyActiveTheme(inventarioPanel);
    }
    if (reportesPanel) {
        theme::Styling::ApplyActiveTheme(reportesPanel);
    }
    
    // Refresh the entire window
    mainPanel->Refresh();
    notebook->Refresh();
}

void MainWindow::OnCustomizeTheme(wxCommandEvent& evt) {
    UICustomizationDialog dlg(this);
    if (dlg.ShowModal() == wxID_OK) {
        // Theme has been updated and saved in the dialog
        RefreshAllThemes();
        
        wxMessageBox(
            wxString::FromUTF8("Tema personalizado aplicado.\n"
            "Los cambios se guardarán para futuras sesiones."),
            wxString::FromUTF8("Éxito"),
            wxOK | wxICON_INFORMATION);
    }
}
#include "MainWindow.h"
#include "LoginWindow.h"
#include "MenuVentas.h"
#include "MenuInventario.h"
#include "MenuReportes.h"

wxString end_session_utf8 = wxString::FromUTF8("Cerrar sesión");

MainWindow::MainWindow(wxWindow* parent, const pos::Usuario& usuario)
    : wxFrame(parent, wxID_ANY, "Punto de Venta",
              wxDefaultPosition, wxDefaultSize,
              wxDEFAULT_FRAME_STYLE | wxMAXIMIZE | wxMINIMIZE),
      usuarioActual(usuario)
{
    Maximize(true);

    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ---------------------------
    // Barra superior
    // ---------------------------
    wxBoxSizer* topBar = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* lblUsuario = new wxStaticText(panel, wxID_ANY,
        "Usuario: " + usuarioActual.username);

    wxButton* btnCerrarSesion = new wxButton(panel, wxID_ANY, end_session_utf8);
    wxButton* btnTema = new wxButton(panel, wxID_ANY, "Tema");

    topBar->Add(lblUsuario, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    topBar->AddStretchSpacer();
    topBar->Add(btnTema, 0, wxALIGN_RIGHT | wxALL, 5);
    topBar->Add(btnCerrarSesion, 0, wxALIGN_RIGHT | wxALL, 5);

    mainSizer->Add(topBar, 0, wxEXPAND | wxALL, 5);

    // ---------------------------
    // Notebook (tabs) 
    // ---------------------------
    notebook = new wxNotebook(panel, wxID_ANY);

    MenuVentas* ventasPanel = new MenuVentas(notebook, usuarioActual);
    MenuInventario* inventarioPanel = new MenuInventario(notebook);
    MenuReportes* reportesPanel = new MenuReportes(notebook);

    notebook->AddPage(ventasPanel, "Ventas");
    notebook->AddPage(inventarioPanel, "Inventario");
    notebook->AddPage(reportesPanel, "Reportes");

    mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

    btnCerrarSesion->Bind(wxEVT_BUTTON, &MainWindow::OnCerrarSesion, this);
    btnTema->Bind(wxEVT_BUTTON, &MainWindow::OnToggleTheme, this);

    panel->SetSizer(mainSizer);
}

void MainWindow::OnCerrarSesion(wxCommandEvent& evt) {
    Close();
    LoginWindow* login = new LoginWindow(nullptr);
    login->Show();
}

void MainWindow::OnToggleTheme(wxCommandEvent& evt) {
    darkMode = !darkMode;

    wxColour bgDark(30, 30, 30);
    wxColour bgPanel(40, 40, 40);
    wxColour fgLight(255, 255, 255);

    if (darkMode) {
        // Frame background
        SetBackgroundColour(bgDark);
        SetForegroundColour(fgLight);

        // Notebook background
        notebook->SetBackgroundColour(bgPanel);
        notebook->SetForegroundColour(fgLight);

        // Apply to all pages
        for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
            wxWindow* page = notebook->GetPage(i);
            page->SetBackgroundColour(bgPanel);
            page->SetForegroundColour(fgLight);

            // Apply recursively to all child controls
            wxWindowList& children = page->GetChildren();
            for (wxWindow* child : children) {
                child->SetBackgroundColour(bgPanel);
                child->SetForegroundColour(fgLight);
            }
        }
    }
    else {
        // Reset to system defaults
        SetBackgroundColour(wxNullColour);
        SetForegroundColour(wxNullColour);
        notebook->SetBackgroundColour(wxNullColour);
        notebook->SetForegroundColour(wxNullColour);

        for (size_t i = 0; i < notebook->GetPageCount(); ++i) {
            wxWindow* page = notebook->GetPage(i);
            page->SetBackgroundColour(wxNullColour);
            page->SetForegroundColour(wxNullColour);

            wxWindowList& children = page->GetChildren();
            for (wxWindow* child : children) {
                child->SetBackgroundColour(wxNullColour);
                child->SetForegroundColour(wxNullColour);
            }
        }
    }

    Refresh();
    Update();
}
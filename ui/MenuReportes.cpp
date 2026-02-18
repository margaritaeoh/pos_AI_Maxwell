#include "MenuReportes.h"
#include <wx/filedlg.h>
#include <wx/statline.h>
#include <wx/scrolwin.h>
#include "ThemeHelper.h"

MenuReportes::MenuReportes(wxWindow* parent)
    : wxPanel(parent),
      archivoVentas("data/ventas.txt"),
      archivoProductos("data/productos.csv"),
      reportes(archivoVentas, archivoProductos)
{
    theme::Styling::ApplyActiveTheme(this);
    
    // Use a scrolled window to allow resizing with content fit
    wxScrolledWindow* scrollWin = new wxScrolledWindow(this);
    scrollWin->SetScrollRate(5, 5);
    theme::Styling::ApplyActiveTheme(scrollWin);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Title
    wxStaticText* lblTitle = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Generador de Reportes"));
    theme::Styling::StyleAsTitle(lblTitle);
    mainSizer->Add(lblTitle, 0, wxALL, 15);

    // Separator
    mainSizer->Add(new wxStaticLine(scrollWin), 0, wxEXPAND | wxALL, 5);

    // ---------------------------
    // Sales Report Section
    // ---------------------------
    wxStaticText* lblSalesSection = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Reporte de Ventas:"));
    theme::Styling::StyleAsHeader(lblSalesSection);
    mainSizer->Add(lblSalesSection, 0, wxALL, 10);

    // File path box for sales report
    wxBoxSizer* salesPathSizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxStaticText* lblSalesFile = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Ubicación del archivo:"));
    theme::Styling::StyleAsLabel(lblSalesFile);
    salesPathSizer->Add(lblSalesFile, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    
    txtSalesReport = new wxTextCtrl(scrollWin, wxID_ANY, "reporte_ventas.csv", 
                                     wxDefaultPosition, wxSize(300, -1));
    txtSalesReport->SetEditable(false);
    theme::Styling::StyleTextControl(txtSalesReport);
    salesPathSizer->Add(txtSalesReport, 1, wxEXPAND | wxALL, 5);
    
    wxButton* btnBrowseSales = new wxButton(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Examinar..."));
    theme::Styling::StyleButtonSecondary(btnBrowseSales);
    salesPathSizer->Add(btnBrowseSales, 0, wxALL, 5);
    mainSizer->Add(salesPathSizer, 0, wxEXPAND | wxALL, 5);

    // Generate button
    wxButton* btnVentas = new wxButton(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Generar Reporte de Ventas"));
    theme::Styling::StyleButtonAccent(btnVentas);
    mainSizer->Add(btnVentas, 0, wxEXPAND | wxALL, 5);

    // Status label
    lblSalesStatus = new wxStaticText(scrollWin, wxID_ANY, "");
    lblSalesStatus->SetForegroundColour(theme::Colors::COLOR_SUCCESS);
    mainSizer->Add(lblSalesStatus, 0, wxALL, 5);

    // Separator
    mainSizer->Add(new wxStaticLine(scrollWin), 0, wxEXPAND | wxALL, 15);

    // ---------------------------
    // Inventory Report Section
    // ---------------------------
    wxStaticText* lblInventorySection = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Reporte de Inventario:"));
    theme::Styling::StyleAsHeader(lblInventorySection);
    mainSizer->Add(lblInventorySection, 0, wxALL, 10);

    // File path box for inventory report
    wxBoxSizer* inventoryPathSizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxStaticText* lblInventoryFile = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Ubicación del archivo:"));
    theme::Styling::StyleAsLabel(lblInventoryFile);
    inventoryPathSizer->Add(lblInventoryFile, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    
    txtInventoryReport = new wxTextCtrl(scrollWin, wxID_ANY, "reporte_inventario.csv", 
                                         wxDefaultPosition, wxSize(300, -1));
    txtInventoryReport->SetEditable(false);
    theme::Styling::StyleTextControl(txtInventoryReport);
    inventoryPathSizer->Add(txtInventoryReport, 1, wxEXPAND | wxALL, 5);
    
    wxButton* btnBrowseInventory = new wxButton(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Examinar..."));
    theme::Styling::StyleButtonSecondary(btnBrowseInventory);
    inventoryPathSizer->Add(btnBrowseInventory, 0, wxALL, 5);
    mainSizer->Add(inventoryPathSizer, 0, wxEXPAND | wxALL, 5);

    // Generate button
    wxButton* btnInventario = new wxButton(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Generar Reporte de Inventario"));
    theme::Styling::StyleButtonAccent(btnInventario);
    mainSizer->Add(btnInventario, 0, wxEXPAND | wxALL, 5);

    // Status label
    lblInventoryStatus = new wxStaticText(scrollWin, wxID_ANY, "");
    lblInventoryStatus->SetForegroundColour(theme::Colors::COLOR_SUCCESS);
    mainSizer->Add(lblInventoryStatus, 0, wxALL, 5);

    mainSizer->AddStretchSpacer(1);

    // Event bindings
    btnBrowseSales->Bind(wxEVT_BUTTON, &MenuReportes::OnBrowseSalesReport, this);
    btnBrowseInventory->Bind(wxEVT_BUTTON, &MenuReportes::OnBrowseInventoryReport, this);
    btnVentas->Bind(wxEVT_BUTTON, &MenuReportes::OnReporteVentas, this);
    btnInventario->Bind(wxEVT_BUTTON, &MenuReportes::OnReporteInventario, this);

    scrollWin->SetSizer(mainSizer);
    
    // Main panel sizer
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(scrollWin, 1, wxEXPAND);
    SetSizer(panelSizer);
}

void MenuReportes::OnBrowseSalesReport(wxCommandEvent& evt) {
    wxFileDialog fileDialog(this, 
                           wxString::FromUTF8("Guardar reporte de ventas"),
                           "", "reporte_ventas.csv",
                           "CSV files (*.csv)|*.csv|All files (*.*)|*.*",
                           wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (fileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    txtSalesReport->SetValue(fileDialog.GetPath());
}

void MenuReportes::OnBrowseInventoryReport(wxCommandEvent& evt) {
    wxFileDialog fileDialog(this,
                           wxString::FromUTF8("Guardar reporte de inventario"),
                           "", "reporte_inventario.csv",
                           "CSV files (*.csv)|*.csv|All files (*.*)|*.*",
                           wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (fileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    txtInventoryReport->SetValue(fileDialog.GetPath());
}

void MenuReportes::OnReporteVentas(wxCommandEvent& evt) {
    wxString filePath = txtSalesReport->GetValue();
    
    if (filePath.IsEmpty()) {
        wxMessageBox(wxString::FromUTF8("Por favor selecciona una ubicación para guardar el archivo."),
                    wxString::FromUTF8("Archivo no especificado"),
                    wxOK | wxICON_WARNING);
        return;
    }

    if (reportes.generarReporteVentasCSV(filePath.ToStdString())) {
        lblSalesStatus->SetLabel(wxString::FromUTF8("✓ Reporte de ventas generado exitosamente"));
        lblSalesStatus->SetForegroundColour(theme::Colors::COLOR_SUCCESS);
        wxMessageBox(wxString::Format(wxString::FromUTF8("Reporte guardado en:\n%s"), filePath),
                    wxString::FromUTF8("Éxito"),
                    wxOK | wxICON_INFORMATION);
    } else {
        lblSalesStatus->SetLabel(wxString::FromUTF8("✗ Error al generar el reporte"));
        lblSalesStatus->SetForegroundColour(theme::Colors::COLOR_ERROR);
        wxMessageBox(wxString::FromUTF8("Error al generar el reporte de ventas."),
                    wxString::FromUTF8("Error"),
                    wxOK | wxICON_ERROR);
    }
    Refresh();
}

void MenuReportes::OnReporteInventario(wxCommandEvent& evt) {
    wxString filePath = txtInventoryReport->GetValue();
    
    if (filePath.IsEmpty()) {
        wxMessageBox(wxString::FromUTF8("Por favor selecciona una ubicación para guardar el archivo."),
                    wxString::FromUTF8("Archivo no especificado"),
                    wxOK | wxICON_WARNING);
        return;
    }

    if (reportes.generarReporteInventarioCSV(filePath.ToStdString())) {
        lblInventoryStatus->SetLabel(wxString::FromUTF8("✓ Reporte de inventario generado exitosamente"));
        lblInventoryStatus->SetForegroundColour(theme::Colors::COLOR_SUCCESS);
        wxMessageBox(wxString::Format(wxString::FromUTF8("Reporte guardado en:\n%s"), filePath),
                    wxString::FromUTF8("Éxito"),
                    wxOK | wxICON_INFORMATION);
    } else {
        lblInventoryStatus->SetLabel(wxString::FromUTF8("✗ Error al generar el reporte"));
        lblInventoryStatus->SetForegroundColour(theme::Colors::COLOR_ERROR);
        wxMessageBox(wxString::FromUTF8("Error al generar el reporte de inventario."),
                    wxString::FromUTF8("Error"),
                    wxOK | wxICON_ERROR);
    }
    Refresh();
}
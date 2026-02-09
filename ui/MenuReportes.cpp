#include "MenuReportes.h"

MenuReportes::MenuReportes(wxWindow* parent)
    : wxPanel(parent),
      archivoVentas("data/ventas.txt"),
      archivoProductos("data/productos.csv"),
      reportes(archivoVentas, archivoProductos)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxButton* btnVentas = new wxButton(this, wxID_ANY, "Generar reporte de ventas");
    wxButton* btnInventario = new wxButton(this, wxID_ANY, "Generar reporte de inventario");

    mainSizer->Add(btnVentas, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(btnInventario, 0, wxEXPAND | wxALL, 10);

    btnVentas->Bind(wxEVT_BUTTON, &MenuReportes::OnReporteVentas, this);
    btnInventario->Bind(wxEVT_BUTTON, &MenuReportes::OnReporteInventario, this);

    SetSizer(mainSizer);
}

void MenuReportes::OnReporteVentas(wxCommandEvent& evt) {
    if (reportes.generarReporteVentasCSV("data/reporte_ventas.csv")) {
        wxMessageBox("Reporte de ventas generado.", "Éxito",
                     wxOK | wxICON_INFORMATION);
    } else {
        wxMessageBox("Error al generar reporte.", "Error",
                     wxOK | wxICON_ERROR);
    }
}

void MenuReportes::OnReporteInventario(wxCommandEvent& evt) {
    if (reportes.generarReporteInventarioCSV("data/reporte_inventario.csv")) {
        wxMessageBox("Reporte de inventario generado.", "Éxito",
                     wxOK | wxICON_INFORMATION);
    } else {
        wxMessageBox("Error al generar reporte.", "Error",
                     wxOK | wxICON_ERROR);
    }
}
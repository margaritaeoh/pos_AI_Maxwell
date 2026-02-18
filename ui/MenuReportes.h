#ifndef MENU_REPORTES_H
#define MENU_REPORTES_H

#include <wx/wx.h>
#include "../negocio/Reportes.h"
#include "../datos/ArchivoVentas.h"
#include "../datos/ArchivoProductos.h"

class MenuReportes : public wxPanel {
public:
    MenuReportes(wxWindow* parent);

private:
    pos::ArchivoVentas archivoVentas;
    pos::ArchivoProductos archivoProductos;
    pos::Reportes reportes;
    
    wxTextCtrl* txtSalesReport;
    wxTextCtrl* txtInventoryReport;
    wxStaticText* lblSalesStatus;
    wxStaticText* lblInventoryStatus;

    void OnReporteVentas(wxCommandEvent& evt);
    void OnReporteInventario(wxCommandEvent& evt);
    void OnBrowseSalesReport(wxCommandEvent& evt);
    void OnBrowseInventoryReport(wxCommandEvent& evt);
};

#endif
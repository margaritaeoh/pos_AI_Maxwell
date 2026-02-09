#ifndef MENU_INVENTARIO_H
#define MENU_INVENTARIO_H

#include <wx/wx.h>
#include <wx/grid.h>
#include "../negocio/Inventario.h"
#include "../datos/ArchivoProductos.h"
#include "ProductoForm.h"

class MenuInventario : public wxPanel {
public:
    MenuInventario(wxWindow* parent);

private:
    wxGrid* grid;
    wxStaticText* lblPageInfo;
    
    pos::ArchivoProductos archivoProductos;
    pos::Inventario inventario;
    
    size_t currentPage = 1;
    static constexpr size_t PAGE_SIZE = 50;

    void CargarProductosEnGrid();
    void ActualizarPaginacion();

    void OnAgregar(wxCommandEvent& evt);
    void OnEditar(wxCommandEvent& evt);
    void OnPaginaAnterior(wxCommandEvent& evt);
    void OnPaginaSiguiente(wxCommandEvent& evt);
};

#endif
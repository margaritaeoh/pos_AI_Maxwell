#ifndef MENU_VENTAS_H
#define MENU_VENTAS_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/listbox.h>
#include "../negocio/ProcesadorVentas.h"
#include "../negocio/Inventario.h"
#include "../datos/ArchivoProductos.h"
#include "../datos/ArchivoVentas.h"
#include "../modelos/Usuario.h"

class MenuVentas : public wxPanel {
public:
    MenuVentas(wxWindow* parent, const pos::Usuario& usuario);

private:
    const pos::Usuario& usuarioActual;

    wxTextCtrl* txtCodigo;
    wxTextCtrl* txtCantidad;
    wxStaticText* lblProducto;
    wxStaticText* lblTotal;

    wxListBox* listaSugerencias;
    wxGrid* grid;

    pos::ArchivoProductos archivoProductos;
    pos::Inventario inventario;
    pos::ArchivoVentas archivoVentas;
    pos::ProcesadorVentas procesador;

    std::vector<pos::LineaVenta> lineas;
    
    // Pagination for search results
    std::vector<const pos::Producto*> searchResults;
    size_t currentSearchPage = 1;
    static constexpr size_t SEARCH_PAGE_SIZE = 20;

    void OnTextoCambio(wxCommandEvent& evt);
    void OnSeleccionSugerencia(wxCommandEvent& evt);

    void OnBuscar(wxCommandEvent& evt);
    void OnAgregarLinea(wxCommandEvent& evt);
    void OnFinalizar(wxCommandEvent& evt);
    void OnCancelar(wxCommandEvent& evt);

    void RefrescarGrid();
    void RecalcularTotal();
    void ActualizarSugerencias();
    
    // Helper methods for search
    double CalcularRelevancia(const std::string& busqueda, const pos::Producto* producto) const;
    int CalcularDistanciaLevenshtein(const std::string& s1, const std::string& s2) const;
};

#endif
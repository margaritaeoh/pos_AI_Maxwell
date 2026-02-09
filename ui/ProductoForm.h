#ifndef PRODUCTO_FORM_H
#define PRODUCTO_FORM_H

#include <wx/wx.h>
#include "../modelos/Producto.h"

class ProductoForm : public wxDialog {
public:
    ProductoForm(wxWindow* parent, const pos::Producto* productoExistente);


    pos::Producto ObtenerProducto() const;

private:
    wxTextCtrl* txtCodigoBarras;
    wxTextCtrl* txtCodigoLocal;
    wxTextCtrl* txtNombre;
    wxTextCtrl* txtDescripcion;
    wxTextCtrl* txtPrecioPublico;
    wxTextCtrl* txtPrecioMayorista;
    wxChoice*   chUnidad;
    wxTextCtrl* txtCantidad;
    wxTextCtrl* txtAnio;
    wxTextCtrl* txtMes;
    wxTextCtrl* txtDia;
    wxChoice*   chPresentacion;
    wxTextCtrl* txtPiezasPresentacion;

    pos::Producto producto;

    void OnAceptar(wxCommandEvent& evt);
	void OnUnidadCambio(wxCommandEvent& evt);
};

#endif
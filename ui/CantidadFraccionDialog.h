#include <wx/wx.h>

class CantidadFraccionDialog : public wxDialog {
public:
    CantidadFraccionDialog(wxWindow* parent, const wxString& nombreProducto);

    double GetCantidad() const { return cantidad; }

private:
    wxTextCtrl* txtCantidad;
    double cantidad = 0.0;

    void OnAceptar(wxCommandEvent& evt);
};
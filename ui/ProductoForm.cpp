#include "ProductoForm.h"
#include <wx/wx.h>

// Helper: convert system/UTF-8 encoding to wxString
static wxString toWxString(const std::string& str) {
    wxString result = wxString::FromUTF8(str.c_str());
    if (result.IsEmpty() && !str.empty()) {
        result = wxString(str.c_str(), wxConvISO8859_1);
    }
    return result;
}

enum {
    ID_BTN_ACEPTAR = wxID_HIGHEST + 100
};

ProductoForm::ProductoForm(wxWindow* parent, const pos::Producto* productoExistente)
    : wxDialog(parent, wxID_ANY,
               productoExistente ? "Editar producto" : "Agregar producto",
               wxDefaultPosition, wxSize(500, 500))
{
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    auto addLabeledCtrl = [&](const wxString& label, wxTextCtrl*& ctrl) {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(panel, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        ctrl = new wxTextCtrl(panel, wxID_ANY);
        row->Add(ctrl, 1, wxEXPAND | wxALL, 5);
        mainSizer->Add(row, 0, wxEXPAND);
    };

    addLabeledCtrl("Código de barras:", txtCodigoBarras);
    addLabeledCtrl("Código local:", txtCodigoLocal);
    addLabeledCtrl("Nombre:", txtNombre);
    addLabeledCtrl("Descripción:", txtDescripcion);
    addLabeledCtrl("Precio público:", txtPrecioPublico);
    addLabeledCtrl("Precio mayorista:", txtPrecioMayorista);
    addLabeledCtrl("Cantidad inventario:", txtCantidad);

    // Unidad
    {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(panel, wxID_ANY, "Unidad:"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        chUnidad = new wxChoice(panel, wxID_ANY);
        chUnidad->Append("Pieza");
        chUnidad->Append("Granel");
        chUnidad->SetSelection(0);
        row->Add(chUnidad, 1, wxEXPAND | wxALL, 5);
        mainSizer->Add(row, 0, wxEXPAND);
    }

    // Caducidad
    {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(panel, wxID_ANY, "Caducidad (AAAA-MM-DD):"),
                 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        txtAnio = new wxTextCtrl(panel, wxID_ANY);
        txtMes  = new wxTextCtrl(panel, wxID_ANY);
        txtDia  = new wxTextCtrl(panel, wxID_ANY);
        row->Add(txtAnio, 1, wxEXPAND | wxALL, 5);
        row->Add(txtMes,  1, wxEXPAND | wxALL, 5);
        row->Add(txtDia,  1, wxEXPAND | wxALL, 5);
        mainSizer->Add(row, 0, wxEXPAND);
    }

    // Presentación
    {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(panel, wxID_ANY, "Presentación:"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        chPresentacion = new wxChoice(panel, wxID_ANY);
        chPresentacion->Append("Ninguna");
        chPresentacion->Append("Caja");
        chPresentacion->Append("Bolsa");
        chPresentacion->SetSelection(0);
        row->Add(chPresentacion, 1, wxEXPAND | wxALL, 5);

        row->Add(new wxStaticText(panel, wxID_ANY, "Piezas:"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        txtPiezasPresentacion = new wxTextCtrl(panel, wxID_ANY);
        row->Add(txtPiezasPresentacion, 1, wxEXPAND | wxALL, 5);

        mainSizer->Add(row, 0, wxEXPAND);
    }

    // Botones
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnAceptar = new wxButton(panel, ID_BTN_ACEPTAR, "Aceptar");
    wxButton* btnCancelar = new wxButton(panel, wxID_CANCEL, "Cancelar");
    btnSizer->Add(btnAceptar, 0, wxALL, 5);
    btnSizer->Add(btnCancelar, 0, wxALL, 5);
    mainSizer->Add(btnSizer, 0, wxALIGN_CENTER | wxALL, 5);
chUnidad->Bind(wxEVT_CHOICE, &ProductoForm::OnUnidadCambio, this);
    btnAceptar->Bind(wxEVT_BUTTON, &ProductoForm::OnAceptar, this);

    panel->SetSizer(mainSizer);

    if (productoExistente) {
        producto = *productoExistente;
        txtCodigoBarras->SetValue(toWxString(producto.codigoBarras));
        txtCodigoLocal->SetValue(toWxString(producto.codigoLocal));
        txtNombre->SetValue(toWxString(producto.nombre));
        txtDescripcion->SetValue(toWxString(producto.descripcion));
        txtPrecioPublico->SetValue(wxString::Format("%.2f", producto.precioPublico));
        txtPrecioMayorista->SetValue(wxString::Format("%.2f", producto.precioMayorista));
        txtCantidad->SetValue(wxString::Format("%.3f", producto.cantidadInventario));
        txtAnio->SetValue(wxString::Format("%d", producto.anioCaducidad));
        txtMes->SetValue(wxString::Format("%d", producto.mesCaducidad));
        txtDia->SetValue(wxString::Format("%d", producto.diaCaducidad));

        if (producto.tipoUnidad == pos::TipoUnidad::Pieza) chUnidad->SetSelection(0);
        else chUnidad->SetSelection(1);

        switch (producto.presentacion) {
            case pos::TipoPresentacion::Caja:  chPresentacion->SetSelection(1); break;
            case pos::TipoPresentacion::Bolsa: chPresentacion->SetSelection(2); break;
            default: chPresentacion->SetSelection(0); break;
        }

        txtPiezasPresentacion->SetValue(wxString::Format("%d", producto.piezasPorPresentacion));
    } else {
        producto = pos::Producto{};
    }
}

void ProductoForm::OnAceptar(wxCommandEvent& evt) {
    producto.codigoBarras = std::string(txtCodigoBarras->GetValue().utf8_str());
    producto.codigoLocal  = std::string(txtCodigoLocal->GetValue().utf8_str());
    producto.nombre       = std::string(txtNombre->GetValue().utf8_str());
    producto.descripcion  = std::string(txtDescripcion->GetValue().utf8_str());

    double precioP = 0, precioM = 0, cant = 0;
    txtPrecioPublico->GetValue().ToDouble(&precioP);
    txtPrecioMayorista->GetValue().ToDouble(&precioM);
    txtCantidad->GetValue().ToDouble(&cant);

    producto.precioPublico   = precioP;
    producto.precioMayorista = precioM;
    producto.cantidadInventario = cant;

    long anio = 0, mes = 0, dia = 0;
    txtAnio->GetValue().ToLong(&anio);
    txtMes->GetValue().ToLong(&mes);
    txtDia->GetValue().ToLong(&dia);

    producto.anioCaducidad = static_cast<int>(anio);
    producto.mesCaducidad  = static_cast<int>(mes);
    producto.diaCaducidad  = static_cast<int>(dia);

    int idxUnidad = chUnidad->GetSelection();
    producto.tipoUnidad = (idxUnidad == 0) ? pos::TipoUnidad::Pieza
                                           : pos::TipoUnidad::Granel;

    int idxPres = chPresentacion->GetSelection();
    if (idxPres == 1) producto.presentacion = pos::TipoPresentacion::Caja;
    else if (idxPres == 2) producto.presentacion = pos::TipoPresentacion::Bolsa;
    else producto.presentacion = pos::TipoPresentacion::Ninguna;

    long piezas = 0;
    txtPiezasPresentacion->GetValue().ToLong(&piezas);
    producto.piezasPorPresentacion = static_cast<int>(piezas);

    EndModal(wxID_OK);
}

pos::Producto ProductoForm::ObtenerProducto() const {
    return producto;
}

void ProductoForm::OnUnidadCambio(wxCommandEvent& evt) {
    int sel = chUnidad->GetSelection();

    if (sel == (int)pos::TipoUnidad::Pieza) {
        txtCantidad->SetValidator(wxTextValidator(wxFILTER_DIGITS));
    } else {
        txtCantidad->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
    }
}
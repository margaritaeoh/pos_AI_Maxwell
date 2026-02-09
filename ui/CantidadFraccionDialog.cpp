#include "CantidadFraccionDialog.h"
#include <wx/valnum.h>
#include <wx/wx.h>


CantidadFraccionDialog::CantidadFraccionDialog(wxWindow* parent, const wxString& nombreProducto)
    : wxDialog(parent, wxID_ANY, "Cantidad para " + nombreProducto,
               wxDefaultPosition, wxSize(300, 150))
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(new wxStaticText(this, wxID_ANY, "Ingrese cantidad (ej. 0.25, 1.5):"),
               0, wxALL, 5);

    /*
	txtCantidad = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                                 0, wxTextValidator(wxFILTER_NUMERIC));
	*/							 
	txtCantidad = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                             0);

							
    sizer->Add(txtCantidad, 0, wxEXPAND | wxALL, 5);

    wxButton* btnOk = new wxButton(this, wxID_OK, "Aceptar");
    btnOk->Bind(wxEVT_BUTTON, &CantidadFraccionDialog::OnAceptar, this);

    sizer->Add(btnOk, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizer(sizer);
}

/*
void CantidadFraccionDialog::OnAceptar(wxCommandEvent& evt) {
    txtCantidad->GetValue().ToDouble(&cantidad);
    EndModal(wxID_OK);
}
*/

void CantidadFraccionDialog::OnAceptar(wxCommandEvent& evt) {
    wxString val = txtCantidad->GetValue();

    if (!val.ToDouble(&cantidad) || cantidad <= 0) {
        wxMessageBox("Cantidad inválida. Use valores como 0.25 o 1.5", "Error",
                     wxOK | wxICON_ERROR);
        return;
    }

    EndModal(wxID_OK);
}


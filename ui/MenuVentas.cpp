#include "MenuVentas.h"
#include "CantidadFraccionDialog.h"
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
    ID_BTN_BUSCAR = wxID_HIGHEST + 200,
    ID_BTN_AGREGAR,
    ID_BTN_FINALIZAR
};

MenuVentas::MenuVentas(wxWindow* parent, const pos::Usuario& usuario)
    : wxPanel(parent),
      usuarioActual(usuario),
      archivoProductos("data/productos.csv"),
      inventario(archivoProductos),
      archivoVentas("data/ventas.txt"),
      procesador(inventario, archivoVentas)
{
    try {
        inventario.cargar();
    } catch (const std::exception& e) {
        wxLogError("Error al cargar inventario: %s", e.what());
    }

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ---------------------------
    // Buscador
    // ---------------------------
    wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);

    searchSizer->Add(new wxStaticText(this, wxID_ANY, toWxString("Código barras/local:")),
                     0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    txtCodigo = new wxTextCtrl(this, wxID_ANY);
    searchSizer->Add(txtCodigo, 1, wxEXPAND | wxALL, 5);

listaSugerencias = new wxListBox(this, wxID_ANY);
mainSizer->Add(listaSugerencias, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);

txtCodigo->Bind(wxEVT_TEXT, &MenuVentas::OnTextoCambio, this);
listaSugerencias->Bind(wxEVT_LISTBOX, &MenuVentas::OnSeleccionSugerencia, this);



    wxButton* btnBuscar = new wxButton(this, ID_BTN_BUSCAR, "Buscar");
    searchSizer->Add(btnBuscar, 0, wxALL, 5);

    mainSizer->Add(searchSizer, 0, wxEXPAND);

    lblProducto = new wxStaticText(this, wxID_ANY, "Producto: (ninguno)");
    mainSizer->Add(lblProducto, 0, wxALL, 5);

    btnBuscar->Bind(wxEVT_BUTTON, &MenuVentas::OnBuscar, this);

    // ---------------------------
    // Cantidad y agregar
    // ---------------------------
    wxBoxSizer* qtySizer = new wxBoxSizer(wxHORIZONTAL);

    qtySizer->Add(new wxStaticText(this, wxID_ANY, "Cantidad:"),
                  0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    txtCantidad = new wxTextCtrl(this, wxID_ANY, "1");
    qtySizer->Add(txtCantidad, 1, wxEXPAND | wxALL, 5);

    wxButton* btnAgregar = new wxButton(this, ID_BTN_AGREGAR, "Agregar a venta");
    qtySizer->Add(btnAgregar, 0, wxALL, 5);

    mainSizer->Add(qtySizer, 0, wxEXPAND);

    btnAgregar->Bind(wxEVT_BUTTON, &MenuVentas::OnAgregarLinea, this);

    // ---------------------------
    // Grid de líneas
    // ---------------------------
    grid = new wxGrid(this, wxID_ANY);
    grid->CreateGrid(0, 5);

    grid->SetColLabelValue(0, "ID Prod");
    grid->SetColLabelValue(1, "Nombre");
    grid->SetColLabelValue(2, "Cantidad");
    grid->SetColLabelValue(3, "Precio unit.");
    grid->SetColLabelValue(4, "Subtotal");

    mainSizer->Add(grid, 1, wxEXPAND | wxALL, 5);

    // ---------------------------
    // Total y botones
    // ---------------------------
    wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);

    lblTotal = new wxStaticText(this, wxID_ANY, "Total: 0.00");
    bottomSizer->Add(lblTotal, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    wxButton* btnFinalizar = new wxButton(this, ID_BTN_FINALIZAR, "Finalizar venta");
    wxButton* btnCancelar = new wxButton(this, wxID_ANY, "Cancelar");

    bottomSizer->Add(btnFinalizar, 0, wxALL, 5);
    bottomSizer->Add(btnCancelar, 0, wxALL, 5);

    mainSizer->Add(bottomSizer, 0, wxEXPAND);

    btnFinalizar->Bind(wxEVT_BUTTON, &MenuVentas::OnFinalizar, this);
    btnCancelar->Bind(wxEVT_BUTTON, &MenuVentas::OnCancelar, this);

    SetSizer(mainSizer);
}

// ---------------------------
// Funciones
// ---------------------------

void MenuVentas::OnBuscar(wxCommandEvent& evt) {
    wxString cod = txtCodigo->GetValue();
    std::string scod = cod.ToStdString();

    pos::Producto* p = inventario.buscarPorCodigoBarras(scod);
    if (!p) p = inventario.buscarPorCodigoLocal(scod);

    if (!p) {
        lblProducto->SetLabel("Producto: no encontrado");
    } else {
        lblProducto->SetLabel(
            wxString::Format("Producto: %s (stock: %.3f)",
                             p->nombre, p->cantidadInventario));
    }
}

void MenuVentas::OnAgregarLinea(wxCommandEvent& evt) {
    wxString cod = txtCodigo->GetValue();
    std::string scod = cod.ToStdString();

    pos::Producto* p = inventario.buscarPorCodigoBarras(scod);
    if (!p) p = inventario.buscarPorCodigoLocal(scod);
    
	if (!p) {
        wxMessageBox("Producto no encontrado.", "Error",
                     wxOK | wxICON_ERROR);
        return;
    }

double cantidad = 0.0;

// Si el producto es por pieza → cantidad normal
if (p->tipoUnidad == pos::TipoUnidad::Pieza) {

    // Cantidad entera
    cantidad = std::stoi(txtCantidad->GetValue().ToStdString());
}

// Si es Kg o Litro → abrir ventana emergente
else {

    CantidadFraccionDialog dlg(this, wxString::FromUTF8(p->nombre));

    if (dlg.ShowModal() != wxID_OK)
        return;

    cantidad = dlg.GetCantidad();
}

    //double cant = 0;
    if (cantidad <= 0) {
        wxMessageBox("Cantidad inválida.", "Error",
                     wxOK | wxICON_ERROR);
        return;
    }

    if (p->cantidadInventario < cantidad) {
        wxMessageBox("No hay suficiente stock.", "Error",
                     wxOK | wxICON_ERROR);
        return;
    }

    pos::LineaVenta l{};
    l.productoId = p->id;
    l.cantidad = cantidad;
    l.precioUnitario = p->precioPublico;
    l.subtotal = l.precioUnitario * l.cantidad;

    lineas.push_back(l);
    RefrescarGrid();
    RecalcularTotal();
}

void MenuVentas::RefrescarGrid() {
    grid->ClearGrid();
    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }

    grid->AppendRows(lineas.size());

    for (size_t i = 0; i < lineas.size(); ++i) {
        const auto& l = lineas[i];
        pos::Producto* p = inventario.buscarPorId(l.productoId);

        wxString nombre = p ? wxString::FromUTF8(p->nombre) : "(desconocido)";

        grid->SetCellValue(i, 0, wxString::Format("%lld", l.productoId));
        grid->SetCellValue(i, 1, nombre);
        grid->SetCellValue(i, 2, wxString::Format("%.3f", l.cantidad));
        grid->SetCellValue(i, 3, wxString::Format("%.2f", l.precioUnitario));
        grid->SetCellValue(i, 4, wxString::Format("%.2f", l.subtotal));
    }

    grid->AutoSizeColumns();
}

void MenuVentas::RecalcularTotal() {
    double total = 0.0;
    for (const auto& l : lineas) {
        total += l.subtotal;
    }
    lblTotal->SetLabel(wxString::Format("Total: %.2f", total));
}

void MenuVentas::OnFinalizar(wxCommandEvent& evt) {
    if (lineas.empty()) {
        wxMessageBox("No hay productos en la venta.", "Aviso",
                     wxOK | wxICON_INFORMATION);
        return;
    }

    pos::Venta venta;
    if (!procesador.realizarVenta(usuarioActual.username, lineas, venta)) {
        wxMessageBox("Error al procesar la venta.", "Error",
                     wxOK | wxICON_ERROR);
        return;
    }

    wxMessageBox(wxString::Format("Venta realizada.\nID: %lld\nTotal: %.2f",
                                  venta.id, venta.total),
                 "Éxito", wxOK | wxICON_INFORMATION);

    lineas.clear();
    RefrescarGrid();
    RecalcularTotal();
}

void MenuVentas::OnCancelar(wxCommandEvent& evt) {
    lineas.clear();
    RefrescarGrid();
    RecalcularTotal();
}

void MenuVentas::OnTextoCambio(wxCommandEvent& evt) {
    wxString texto = txtCodigo->GetValue().Lower();
    
    if (texto.Length() < 1) {
        listaSugerencias->Clear();
        searchResults.clear();
        return;
    }

    std::string searchTerm = texto.ToStdString();
    
    // Use index for fast name search (max 100 results)
    searchResults = inventario.getIndex()->searchByNombre(searchTerm, 100);
    
    // Also search codes
    auto codeResults = inventario.getIndex()->searchByCodigo(searchTerm, 50);
    for (auto p : codeResults) {
        // Avoid duplicates
        auto it = std::find_if(searchResults.begin(), searchResults.end(),
                              [p](const pos::Producto* existing) { return existing->id == p->id; });
        if (it == searchResults.end()) {
            searchResults.push_back(p);
        }
    }
    
    // Show top 20 in listbox
    listaSugerencias->Clear();
    size_t limit = std::min(searchResults.size(), size_t(20));
    for (size_t i = 0; i < limit; ++i) {
        listaSugerencias->Append(toWxString(searchResults[i]->nombre));
    }
}

void MenuVentas::OnSeleccionSugerencia(wxCommandEvent& evt) {
    int sel = listaSugerencias->GetSelection();
    if (sel >= 0 && sel < static_cast<int>(searchResults.size())) {
        txtCodigo->SetValue(toWxString(searchResults[sel]->codigoBarras));
        listaSugerencias->Clear();
        searchResults.clear();
    }
}
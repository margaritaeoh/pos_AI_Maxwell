#include "MenuVentas.h"
#include "CantidadFraccionDialog.h"
#include "ThemeHelper.h"
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <algorithm>
#include <cctype>

// Helper: convert system/UTF-8 encoding to wxString
static wxString toWxString(const std::string& str) {
    wxString result = wxString::FromUTF8(str.c_str());
    if (result.IsEmpty() && !str.empty()) {
        result = wxString(str.c_str(), wxConvISO8859_1);
    }
    return result;
}

// Helper: convert to lowercase
static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
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
    theme::Styling::ApplyActiveTheme(this);
    
    try {
        inventario.cargar();
    } catch (const std::exception& e) {
        wxLogError("Error al cargar inventario: %s", e.what());
    }

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create scrolled window for vertical resizing
    wxScrolledWindow* scrollWin = new wxScrolledWindow(this);
    scrollWin->SetScrollRate(5, 5);
    theme::Styling::ApplyActiveTheme(scrollWin);

    wxBoxSizer* scrollSizer = new wxBoxSizer(wxVERTICAL);

    // ---------------------------
    // Title
    // ---------------------------
    wxStaticText* lblTitle = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Búsqueda de Productos"));
    theme::Styling::StyleAsHeader(lblTitle);
    scrollSizer->Add(lblTitle, 0, wxALL, 10);

    // ---------------------------
    // Search box
    // ---------------------------
    wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* lblSearch = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Búsqueda:"));
    theme::Styling::StyleAsLabel(lblSearch);
    searchSizer->Add(lblSearch, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    txtCodigo = new wxTextCtrl(scrollWin, wxID_ANY);
    txtCodigo->SetHint(wxString::FromUTF8("Escriba código o nombre..."));
    theme::Styling::StyleTextControl(txtCodigo);
    searchSizer->Add(txtCodigo, 1, wxEXPAND | wxALL, 5);

    wxButton* btnBuscar = new wxButton(scrollWin, ID_BTN_BUSCAR, 
        wxString::FromUTF8("Buscar"));
    theme::Styling::StyleButtonSecondary(btnBuscar);
    searchSizer->Add(btnBuscar, 0, wxALL, 5);

    scrollSizer->Add(searchSizer, 0, wxEXPAND | wxALL, 0);

    // Suggestions listbox - with proper height allocation and styling
    wxStaticText* lblSugerencias = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Sugerencias de búsqueda:"));
    theme::Styling::StyleAsLabel(lblSugerencias);
    scrollSizer->Add(lblSugerencias, 0, wxLEFT | wxRIGHT | wxTOP, 5);
    
    listaSugerencias = new wxListBox(scrollWin, wxID_ANY);
    listaSugerencias->SetMinSize(wxSize(-1, 120));
    theme::Styling::ApplyActiveTheme(listaSugerencias);
    scrollSizer->Add(listaSugerencias, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

    txtCodigo->Bind(wxEVT_TEXT, &MenuVentas::OnTextoCambio, this);
    listaSugerencias->Bind(wxEVT_LISTBOX, &MenuVentas::OnSeleccionSugerencia, this);
    
    btnBuscar->Bind(wxEVT_BUTTON, &MenuVentas::OnBuscar, this);

    // ---------------------------
    // Product information
    // ---------------------------
    lblProducto = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Producto: (ninguno)"));
    theme::Styling::StyleAsLabel(lblProducto);
    scrollSizer->Add(lblProducto, 0, wxALL, 8);

    // ---------------------------
    // Quantity and add button
    // ---------------------------
    wxBoxSizer* qtySizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText* lblQty = new wxStaticText(scrollWin, wxID_ANY, 
        wxString::FromUTF8("Cantidad:"));
    theme::Styling::StyleAsLabel(lblQty);
    qtySizer->Add(lblQty, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    txtCantidad = new wxTextCtrl(scrollWin, wxID_ANY, "1");
    theme::Styling::StyleTextControl(txtCantidad);
    qtySizer->Add(txtCantidad, 1, wxEXPAND | wxALL, 5);

    wxButton* btnAgregar = new wxButton(scrollWin, ID_BTN_AGREGAR, 
        wxString::FromUTF8("Agregar a Venta"));
    theme::Styling::StyleButtonAccent(btnAgregar);
    qtySizer->Add(btnAgregar, 0, wxALL, 5);

    scrollSizer->Add(qtySizer, 0, wxEXPAND | wxALL, 0);

    btnAgregar->Bind(wxEVT_BUTTON, &MenuVentas::OnAgregarLinea, this);

    // ---------------------------
    // Grid de líneas de venta
    // ---------------------------
    grid = new wxGrid(scrollWin, wxID_ANY);
    grid->CreateGrid(0, 5);
    theme::Styling::StyleGrid(grid);

    grid->SetColLabelValue(0, "ID Prod");
    grid->SetColLabelValue(1, wxString::FromUTF8("Nombre"));
    grid->SetColLabelValue(2, wxString::FromUTF8("Cantidad"));
    grid->SetColLabelValue(3, wxString::FromUTF8("Precio unit."));
    grid->SetColLabelValue(4, "Subtotal");

    scrollSizer->Add(grid, 1, wxEXPAND | wxALL, 5);

    // ---------------------------
    // Total y botones
    // ---------------------------
    wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);

    lblTotal = new wxStaticText(scrollWin, wxID_ANY, "Total: 0.00");
    theme::Styling::StyleAsHeader(lblTotal);
    bottomSizer->Add(lblTotal, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    wxButton* btnFinalizar = new wxButton(scrollWin, ID_BTN_FINALIZAR, wxString::FromUTF8("Finalizar venta"));
    wxButton* btnCancelar = new wxButton(scrollWin, wxID_ANY, wxString::FromUTF8("Cancelar"));
    theme::Styling::StyleButtonAccent(btnFinalizar);
    theme::Styling::StyleButtonSecondary(btnCancelar);

    bottomSizer->Add(btnFinalizar, 0, wxALL, 5);
    bottomSizer->Add(btnCancelar, 0, wxALL, 5);

    scrollSizer->Add(bottomSizer, 0, wxEXPAND | wxALL, 0);

    btnFinalizar->Bind(wxEVT_BUTTON, &MenuVentas::OnFinalizar, this);
    btnCancelar->Bind(wxEVT_BUTTON, &MenuVentas::OnCancelar, this);

    scrollWin->SetSizer(scrollSizer);
    mainSizer->Add(scrollWin, 1, wxEXPAND);
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
        lblProducto->SetLabel(wxString::FromUTF8("Producto: no encontrado"));
    } else {
        lblProducto->SetLabel(
            wxString::Format(wxString::FromUTF8("Producto: %s (inventario: %.3f)"),
                             toWxString(p->nombre), p->cantidadInventario));
    }
}

void MenuVentas::OnAgregarLinea(wxCommandEvent& evt) {
    wxString cod = txtCodigo->GetValue();
    std::string scod = cod.ToStdString();

    pos::Producto* p = inventario.buscarPorCodigoBarras(scod);
    if (!p) p = inventario.buscarPorCodigoLocal(scod);
    
	if (!p) {
        wxMessageBox(wxString::FromUTF8("Producto no encontrado."), 
                     wxString::FromUTF8("Error"),
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
        wxMessageBox(wxString::FromUTF8("Cantidad inválida."), 
                     wxString::FromUTF8("Error"),
                     wxOK | wxICON_ERROR);
        return;
    }

    if (p->cantidadInventario < cantidad) {
        wxMessageBox(wxString::FromUTF8("No hay suficiente inventario."), 
                     wxString::FromUTF8("Error"),
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
        wxMessageBox(wxString::FromUTF8("No hay productos en la venta."), 
                     wxString::FromUTF8("Aviso"),
                     wxOK | wxICON_INFORMATION);
        return;
    }

    pos::Venta venta;
    if (!procesador.realizarVenta(usuarioActual.username, lineas, venta)) {
        wxMessageBox(wxString::FromUTF8("Error al procesar la venta."), 
                     wxString::FromUTF8("Error"),
                     wxOK | wxICON_ERROR);
        return;
    }

    wxMessageBox(wxString::Format(wxString::FromUTF8("Venta realizada.\nID: %lld\nTotal: %.2f"),
                                  venta.id, venta.total),
                 wxString::FromUTF8("Éxito"), wxOK | wxICON_INFORMATION);

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

    std::string searchTerm = toLower(texto.ToStdString());
    
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
    
    // Sort by relevance (closeness to search term)
    std::sort(searchResults.begin(), searchResults.end(),
              [this, &searchTerm](const pos::Producto* a, const pos::Producto* b) {
                  double scoreA = CalcularRelevancia(searchTerm, a);
                  double scoreB = CalcularRelevancia(searchTerm, b);
                  return scoreA > scoreB;  // Higher score = better match
              });
    
    // Show top 20 in listbox with code and name
    listaSugerencias->Clear();
    size_t limit = std::min(searchResults.size(), size_t(20));
    for (size_t i = 0; i < limit; ++i) {
        std::string displayText = searchResults[i]->codigoBarras + " | " + 
                                 searchResults[i]->nombre;
        listaSugerencias->Append(toWxString(displayText));
    }
}

void MenuVentas::OnSeleccionSugerencia(wxCommandEvent& evt) {
    int sel = listaSugerencias->GetSelection();
    if (sel >= 0 && sel < static_cast<int>(searchResults.size())) {
        const pos::Producto* p = searchResults[sel];
        txtCodigo->SetValue(toWxString(p->codigoBarras));
        
        // Show product info
        lblProducto->SetLabel(
            wxString::Format(wxString::FromUTF8("Producto: %s (inventario: %.3f)"),
                             toWxString(p->nombre),
                             p->cantidadInventario));
        
        listaSugerencias->Clear();
        searchResults.clear();
    }
}

int MenuVentas::CalcularDistanciaLevenshtein(const std::string& s1, const std::string& s2) const {
    size_t len1 = s1.length();
    size_t len2 = s2.length();
    
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));
    
    for (size_t i = 0; i <= len1; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) dp[0][j] = j;
    
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            }
        }
    }
    
    return dp[len1][len2];
}

double MenuVentas::CalcularRelevancia(const std::string& busqueda, const pos::Producto* producto) const {
    std::string codBarrasLower = toLower(producto->codigoBarras);
    std::string codLocalLower = toLower(producto->codigoLocal);
    std::string nombreLower = toLower(producto->nombre);
    
    double score = 0.0;
    
    // Exact match in barcode = 100 points
    if (codBarrasLower == busqueda) {
        return 1000.0;
    }
    
    // Exact match in local code = 95 points
    if (codLocalLower == busqueda) {
        return 950.0;
    }
    
    // Exact match in name = 90 points
    if (nombreLower == busqueda) {
        return 900.0;
    }
    
    // Prefix match in barcode = 80 points
    if (codBarrasLower.find(busqueda) == 0) {
        return 800.0;
    }
    
    // Prefix match in local code = 75 points
    if (codLocalLower.find(busqueda) == 0) {
        return 750.0;
    }
    
    // Prefix match in name = 70 points
    if (nombreLower.find(busqueda) == 0) {
        return 700.0;
    }
    
    // Contains in barcode = 60 points
    if (codBarrasLower.find(busqueda) != std::string::npos) {
        return 600.0;
    }
    
    // Contains in local code = 55 points
    if (codLocalLower.find(busqueda) != std::string::npos) {
        return 550.0;
    }
    
    // Contains in name = 50 points
    if (nombreLower.find(busqueda) != std::string::npos) {
        return 500.0;
    }
    
    // Levenshtein distance for fuzzy matching
    int distBarras = CalcularDistanciaLevenshtein(busqueda, codBarrasLower);
    int distLocal = CalcularDistanciaLevenshtein(busqueda, codLocalLower);
    int distNombre = CalcularDistanciaLevenshtein(busqueda, nombreLower);
    
    int minDist = std::min({distBarras, distLocal, distNombre});
    
    // Lower distance = higher score (100 - distance, minimum 0)
    if (minDist <= 5) {
        return 100.0 - (minDist * 5);
    }
    
    return 0.0;
}
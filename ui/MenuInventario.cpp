#include "MenuInventario.h"
#include "ThemeHelper.h"
#include <ctime>
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include "../modelos/Producto.h"

// Helper: convert system/UTF-8 encoding to wxString
static wxString toWxString(const std::string& str) {
    wxString result = wxString::FromUTF8(str.c_str());
    if (result.IsEmpty() && !str.empty()) {
        result = wxString(str.c_str(), wxConvISO8859_1);
    }
    return result;
}

enum {
    ID_BTN_AGREGAR = wxID_HIGHEST + 1,
    ID_BTN_EDITAR,
    ID_BTN_PREV,
    ID_BTN_NEXT
};

MenuInventario::MenuInventario(wxWindow* parent, const pos::Usuario& usuario)
    : wxPanel(parent),
      archivoProductos("data/productos.csv"),
      inventario(archivoProductos),
      usuarioActual(usuario)
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
    // PAGE INFO
    // ---------------------------
    lblPageInfo = new wxStaticText(scrollWin, wxID_ANY, "Página 1 de 1 (0 productos)");
    theme::Styling::StyleAsLabel(lblPageInfo);
    scrollSizer->Add(lblPageInfo, 0, wxALL, 5);

    // ---------------------------
    // GRID
    // ---------------------------
    grid = new wxGrid(scrollWin, wxID_ANY);
    grid->CreateGrid(0, 10);
    theme::Styling::StyleGrid(grid);

    grid->SetColLabelValue(0, "ID");
    grid->SetColLabelValue(1, toWxString("Código barras"));
    grid->SetColLabelValue(2, toWxString("Código local"));
    grid->SetColLabelValue(3, "Nombre");
    grid->SetColLabelValue(4, toWxString("Precio público"));
    grid->SetColLabelValue(5, toWxString("Precio mayorista"));
    grid->SetColLabelValue(6, "Unidad");
    grid->SetColLabelValue(7, "Cantidad");
    grid->SetColLabelValue(8, toWxString("Caducidad"));
    grid->SetColLabelValue(9, toWxString("Presentación"));

    scrollSizer->Add(grid, 1, wxEXPAND | wxALL, 5);

    // ---------------------------
    // PAGINATION BUTTONS
    // ---------------------------
    wxBoxSizer* paginationSizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxButton* btnPrev = new wxButton(scrollWin, ID_BTN_PREV, wxString::FromUTF8("← Anterior"));
    wxButton* btnNext = new wxButton(scrollWin, ID_BTN_NEXT, wxString::FromUTF8("Siguiente →"));
    theme::Styling::StyleButtonSecondary(btnPrev);
    theme::Styling::StyleButtonSecondary(btnNext);
    
    paginationSizer->Add(btnPrev, 0, wxALL, 5);
    paginationSizer->Add(btnNext, 0, wxALL, 5);
    
    scrollSizer->Add(paginationSizer, 0, wxALIGN_CENTER | wxALL, 5);

    btnPrev->Bind(wxEVT_BUTTON, &MenuInventario::OnPaginaAnterior, this);
    btnNext->Bind(wxEVT_BUTTON, &MenuInventario::OnPaginaSiguiente, this);

    // ---------------------------
    // BOTONES
    // ---------------------------
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    btnAgregar = new wxButton(scrollWin, ID_BTN_AGREGAR, wxString::FromUTF8("Agregar producto"));
    btnEditar  = new wxButton(scrollWin, ID_BTN_EDITAR,  wxString::FromUTF8("Editar producto"));
    theme::Styling::StyleButtonAccent(btnAgregar);
    theme::Styling::StyleButtonSecondary(btnEditar);

    buttonSizer->Add(btnAgregar, 0, wxALL, 5);
    buttonSizer->Add(btnEditar,  0, wxALL, 5);

    scrollSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 5);

    // Eventos
    btnAgregar->Bind(wxEVT_BUTTON, &MenuInventario::OnAgregar, this);
    btnEditar->Bind(wxEVT_BUTTON, &MenuInventario::OnEditar, this);

    scrollWin->SetSizer(scrollSizer);
    mainSizer->Add(scrollWin, 1, wxEXPAND);
    SetSizer(mainSizer);

    CargarProductosEnGrid();
    
    // Apply role-based restrictions
    RestringirPorRol();
}

void MenuInventario::RestringirPorRol() {
    if (usuarioActual.rol == pos::RolUsuario::Usuario) {
        // Standard users can only view, not edit
        btnAgregar->Enable(false);
        btnEditar->Enable(false);
        grid->EnableEditing(false);
    }
}

void MenuInventario::CargarProductosEnGrid() {
    grid->ClearGrid();
    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }

    // Load paginated products
    auto productos = inventario.obtenerPagina(currentPage, PAGE_SIZE);
    grid->AppendRows(productos.size());

    for (size_t i = 0; i < productos.size(); ++i) {
        const auto& p = productos[i];

        wxString caducidad = wxString::Format("%04d-%02d-%02d",
                                              p.anioCaducidad,
                                              p.mesCaducidad,
                                              p.diaCaducidad);

        wxString unidad;
        switch (p.tipoUnidad) {
            case pos::TipoUnidad::Pieza: unidad = "Pieza"; break;
            case pos::TipoUnidad::Kilogramo: unidad = "Kg"; break;
            case pos::TipoUnidad::Litro: unidad = "L"; break;
            case pos::TipoUnidad::Granel: unidad = "Granel"; break;
        }

        wxString presentacion;
        switch (p.presentacion) {
            case pos::TipoPresentacion::Caja:  presentacion = "Caja";  break;
            case pos::TipoPresentacion::Bolsa: presentacion = "Bolsa"; break;
            default: presentacion = "Ninguna"; break;
        }

        grid->SetCellValue(i, 0, wxString::Format("%lld", p.id));
        grid->SetCellValue(i, 1, toWxString(p.codigoBarras));
        grid->SetCellValue(i, 2, toWxString(p.codigoLocal));
        grid->SetCellValue(i, 3, toWxString(p.nombre));
        grid->SetCellValue(i, 4, wxString::Format("%.2f", p.precioPublico));
        grid->SetCellValue(i, 5, wxString::Format("%.2f", p.precioMayorista));
        grid->SetCellValue(i, 6, unidad);
        grid->SetCellValue(i, 7, wxString::Format("%.3f", p.cantidadInventario));
        grid->SetCellValue(i, 8, caducidad);
        grid->SetCellValue(i, 9, presentacion);

        // Resaltar bajo inventario
        if (p.cantidadInventario < 5.0) {
            for (int col = 0; col < 10; ++col) {
                grid->SetCellBackgroundColour(i, col, wxColour(255, 200, 200)); // rojo suave
            }
        }
    }

    grid->AutoSizeColumns();
    ActualizarPaginacion();
}

void MenuInventario::ActualizarPaginacion() {
    size_t totalProductos = inventario.getTotalProductos();
    size_t totalPages = inventario.getTotalPages(PAGE_SIZE);
    
    wxString pageInfo = wxString::Format(
        "Página %zu de %zu (%zu productos totales)",
        currentPage, totalPages, totalProductos);
    lblPageInfo->SetLabel(pageInfo);
}

void MenuInventario::OnAgregar(wxCommandEvent& evt) {
    ProductoForm dlg(this, nullptr);
    if (dlg.ShowModal() == wxID_OK) {
        pos::Producto nuevo = dlg.ObtenerProducto();
        nuevo.id = static_cast<long long>(std::time(nullptr));

        if (!inventario.agregarProducto(nuevo)) {
            wxMessageBox("No se pudo agregar (¿límite 10000?)", "Error",
                         wxOK | wxICON_ERROR);
            return;
        }
        inventario.guardar();
        CargarProductosEnGrid();
    }
}

void MenuInventario::OnEditar(wxCommandEvent& evt) {
    int fila = grid->GetGridCursorRow();
    if (fila < 0) {
        wxMessageBox("Selecciona un producto en la tabla.", "Aviso",
                     wxOK | wxICON_INFORMATION);
        return;
    }

    long long id;
    grid->GetCellValue(fila, 0).ToLongLong(&id);

    pos::Producto* p = inventario.buscarPorId(id);
    if (!p) {
        wxMessageBox("Producto no encontrado.", "Error",
                     wxOK | wxICON_ERROR);
        return;
    }

    ProductoForm dlg(this, p);
    if (dlg.ShowModal() == wxID_OK) {
        pos::Producto editado = dlg.ObtenerProducto();
        editado.id = p->id;
        inventario.actualizarProducto(editado);
        inventario.guardar();
        CargarProductosEnGrid();
    }
}

void MenuInventario::OnPaginaAnterior(wxCommandEvent& evt) {
    if (currentPage > 1) {
        currentPage--;
        CargarProductosEnGrid();
    }
}

void MenuInventario::OnPaginaSiguiente(wxCommandEvent& evt) {
    size_t totalPages = inventario.getTotalPages(PAGE_SIZE);
    if (currentPage < totalPages) {
        currentPage++;
        CargarProductosEnGrid();
    }
}
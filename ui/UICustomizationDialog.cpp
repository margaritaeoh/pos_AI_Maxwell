#include "UICustomizationDialog.h"
#include <wx/scrolwin.h>
#include <wx/spinctrl.h>
#include <wx/notebook.h>

UICustomizationDialog::UICustomizationDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Personalizar Vistas"),
               wxDefaultPosition, wxSize(700, 600),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      cpBgMain(nullptr), cpBgPanel(nullptr), cpBgInput(nullptr), cpBgHeader(nullptr),
      cpTextPrimary(nullptr), cpTextSecondary(nullptr), cpTextAccent(nullptr),
      cpBtnAccent(nullptr), cpBtnHover(nullptr), cpBtnText(nullptr),
      cpSuccess(nullptr), cpError(nullptr), cpWarning(nullptr), cpInfo(nullptr),
      spinTitleSize(nullptr), spinHeaderSize(nullptr), spinLabelSize(nullptr),
      spinDefaultSize(nullptr), chkBoldHeaders(nullptr),
      spinBorderWidth(nullptr), spinBorderRadius(nullptr), chkUseGradient(nullptr)
{
    theme::Styling::ApplyActiveTheme(this);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Create notebook with tabs
    notebook = new wxNotebook(this, wxID_ANY);
    theme::Styling::ApplyActiveTheme(notebook);
    
    // Create pages
    wxPanel* colorPanel = CreateColorPanel(notebook);
    wxPanel* fontPanel = CreateFontPanel(notebook);
    wxPanel* buttonPanel = CreateButtonStylePanel(notebook);
    
    notebook->AddPage(colorPanel, wxString::FromUTF8("Colores"));
    notebook->AddPage(fontPanel, wxString::FromUTF8("Fuentes"));
    notebook->AddPage(buttonPanel, wxString::FromUTF8("Estilo de Botones"));
    
    mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 10);
    
    // Buttons at the bottom
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    wxButton* btnReset = new wxButton(this, wxID_ANY, wxString::FromUTF8("Restaurar predeterminados"));
    wxButton* btnOK = new wxButton(this, wxID_OK, wxString::FromUTF8("Aceptar"));
    wxButton* btnCancel = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Cancelar"));
    
    theme::Styling::StyleButtonSecondary(btnReset);
    theme::Styling::StyleButtonAccent(btnOK);
    theme::Styling::StyleButtonSecondary(btnCancel);
    
    buttonSizer->Add(btnReset, 0, wxALL, 5);
    buttonSizer->AddStretchSpacer();
    buttonSizer->Add(btnOK, 0, wxALL, 5);
    buttonSizer->Add(btnCancel, 0, wxALL, 5);
    
    mainSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 5);
    
    // Bind events
    btnReset->Bind(wxEVT_BUTTON, &UICustomizationDialog::OnReset, this);
    Bind(wxEVT_BUTTON, &UICustomizationDialog::OnOK, this, wxID_OK);
    Bind(wxEVT_BUTTON, &UICustomizationDialog::OnCancel, this, wxID_CANCEL);
    
    SetSizer(mainSizer);
    LoadCurrentTheme();
}

wxPanel* UICustomizationDialog::CreateColorPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    theme::Styling::ApplyActiveTheme(panel);
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    wxScrolledWindow* scrollWin = new wxScrolledWindow(panel);
    scrollWin->SetScrollRate(5, 5);
    theme::Styling::ApplyActiveTheme(scrollWin);
    
    wxBoxSizer* scrollSizer = new wxBoxSizer(wxVERTICAL);
    
    // Helper lambda to add color picker with label
    auto addColorRow = [&scrollSizer, scrollWin](const wxString& label, wxColourPickerCtrl*& picker) {
        wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* lbl = new wxStaticText(scrollWin, wxID_ANY, label);
        theme::Styling::StyleAsLabel(lbl);
        
        picker = new wxColourPickerCtrl(scrollWin, wxID_ANY);
        
        rowSizer->Add(lbl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
        rowSizer->Add(picker, 0, wxALL, 5);
        scrollSizer->Add(rowSizer, 0, wxEXPAND);
    };
    
    // Backgrounds section
    wxStaticText* hdr1 = new wxStaticText(scrollWin, wxID_ANY, wxString::FromUTF8("Fondos"));
    theme::Styling::StyleAsHeader(hdr1);
    scrollSizer->Add(hdr1, 0, wxALL, 10);
    
    addColorRow(wxString::FromUTF8("Fondo Principal"), cpBgMain);
    addColorRow(wxString::FromUTF8("Fondo de Paneles"), cpBgPanel);
    addColorRow(wxString::FromUTF8("Fondo de Controles"), cpBgInput);
    addColorRow(wxString::FromUTF8("Encabezado de Grillas"), cpBgHeader);
    
    // Text section
    wxStaticText* hdr2 = new wxStaticText(scrollWin, wxID_ANY, wxString::FromUTF8("Texto"));
    theme::Styling::StyleAsHeader(hdr2);
    scrollSizer->Add(hdr2, 0, wxALL, 10);
    
    addColorRow(wxString::FromUTF8("Texto Principal"), cpTextPrimary);
    addColorRow(wxString::FromUTF8("Texto Secundario"), cpTextSecondary);
    addColorRow(wxString::FromUTF8("Texto Destacado"), cpTextAccent);
    
    // Buttons section
    wxStaticText* hdr3 = new wxStaticText(scrollWin, wxID_ANY, wxString::FromUTF8("Botones"));
    theme::Styling::StyleAsHeader(hdr3);
    scrollSizer->Add(hdr3, 0, wxALL, 10);
    
    addColorRow(wxString::FromUTF8("Color Destacado"), cpBtnAccent);
    addColorRow(wxString::FromUTF8("Color Desplazable"), cpBtnHover);
    addColorRow(wxString::FromUTF8("Texto de Botones"), cpBtnText);
    
    // Status section
    wxStaticText* hdr4 = new wxStaticText(scrollWin, wxID_ANY, wxString::FromUTF8("Estados"));
    theme::Styling::StyleAsHeader(hdr4);
    scrollSizer->Add(hdr4, 0, wxALL, 10);
    
    addColorRow(wxString::FromUTF8("Éxito"), cpSuccess);
    addColorRow(wxString::FromUTF8("Error"), cpError);
    addColorRow(wxString::FromUTF8("Advertencia"), cpWarning);
    addColorRow(wxString::FromUTF8("Información"), cpInfo);
    
    scrollWin->SetSizer(scrollSizer);
    sizer->Add(scrollWin, 1, wxEXPAND);
    panel->SetSizer(sizer);
    
    return panel;
}

wxPanel* UICustomizationDialog::CreateFontPanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    theme::Styling::ApplyActiveTheme(panel);
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* infoText = new wxStaticText(panel, wxID_ANY,
        wxString::FromUTF8("Ajustar tamaños de fuentes (en puntos)"));
    theme::Styling::StyleAsHeader(infoText);
    sizer->Add(infoText, 0, wxALL, 10);
    
    // Title size
    wxBoxSizer* sz1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* l1 = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("Títulos:"));
    theme::Styling::StyleAsLabel(l1);
    spinTitleSize = new wxSpinCtrl(panel, wxID_ANY, "16", wxDefaultPosition, wxSize(80, -1),
                                   wxSP_ARROW_KEYS, 8, 32, 16);
    sz1->Add(l1, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sz1->Add(spinTitleSize, 0, wxALL, 5);
    sizer->Add(sz1, 0, wxEXPAND | wxALL, 5);
    
    // Header size
    wxBoxSizer* sz2 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* l2 = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("Encabezados:"));
    theme::Styling::StyleAsLabel(l2);
    spinHeaderSize = new wxSpinCtrl(panel, wxID_ANY, "13", wxDefaultPosition, wxSize(80, -1),
                                    wxSP_ARROW_KEYS, 8, 24, 13);
    sz2->Add(l2, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sz2->Add(spinHeaderSize, 0, wxALL, 5);
    sizer->Add(sz2, 0, wxEXPAND | wxALL, 5);
    
    // Label size
    wxBoxSizer* sz3 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* l3 = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("Etiquetas:"));
    theme::Styling::StyleAsLabel(l3);
    spinLabelSize = new wxSpinCtrl(panel, wxID_ANY, "11", wxDefaultPosition, wxSize(80, -1),
                                   wxSP_ARROW_KEYS, 8, 20, 11);
    sz3->Add(l3, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sz3->Add(spinLabelSize, 0, wxALL, 5);
    sizer->Add(sz3, 0, wxEXPAND | wxALL, 5);
    
    // Default size
    wxBoxSizer* sz4 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* l4 = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("Predeterminado:"));
    theme::Styling::StyleAsLabel(l4);
    spinDefaultSize = new wxSpinCtrl(panel, wxID_ANY, "10", wxDefaultPosition, wxSize(80, -1),
                                     wxSP_ARROW_KEYS, 8, 18, 10);
    sz4->Add(l4, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sz4->Add(spinDefaultSize, 0, wxALL, 5);
    sizer->Add(sz4, 0, wxEXPAND | wxALL, 5);
    
    // Bold headers checkbox
    chkBoldHeaders = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8("Encabezados en negrita"));
    chkBoldHeaders->SetValue(true);
    sizer->Add(chkBoldHeaders, 0, wxALL, 10);
    
    sizer->AddStretchSpacer();
    panel->SetSizer(sizer);
    
    return panel;
}

wxPanel* UICustomizationDialog::CreateButtonStylePanel(wxWindow* parent) {
    wxPanel* panel = new wxPanel(parent);
    theme::Styling::ApplyActiveTheme(panel);
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* infoText = new wxStaticText(panel, wxID_ANY,
        wxString::FromUTF8("Opciones de estilos de botones"));
    theme::Styling::StyleAsHeader(infoText);
    sizer->Add(infoText, 0, wxALL, 10);
    
    // Border width
    wxBoxSizer* sz1 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* l1 = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("Ancho de borde:"));
    theme::Styling::StyleAsLabel(l1);
    spinBorderWidth = new wxSpinCtrlDouble(panel, wxID_ANY, "1", wxDefaultPosition, wxSize(100, -1),
                                           wxSP_ARROW_KEYS, 0.5, 5.0, 1.0, 0.5);
    sz1->Add(l1, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sz1->Add(spinBorderWidth, 0, wxALL, 5);
    sizer->Add(sz1, 0, wxEXPAND | wxALL, 5);
    
    // Border radius
    wxBoxSizer* sz2 = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* l2 = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8("Radio de borde:"));
    theme::Styling::StyleAsLabel(l2);
    spinBorderRadius = new wxSpinCtrl(panel, wxID_ANY, "3", wxDefaultPosition, wxSize(100, -1),
                                      wxSP_ARROW_KEYS, 0, 20, 3);
    sz2->Add(l2, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    sz2->Add(spinBorderRadius, 0, wxALL, 5);
    sizer->Add(sz2, 0, wxEXPAND | wxALL, 5);
    
    chkUseGradient = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8("Usar degradado en botones"));
    chkUseGradient->SetValue(false);
    sizer->Add(chkUseGradient, 0, wxALL, 10);
    
    sizer->AddStretchSpacer();
    panel->SetSizer(sizer);
    
    return panel;
}

void UICustomizationDialog::LoadCurrentTheme() {
    const theme::CustomTheme& ct = theme::ThemeManager::GetCustomTheme();
    
    cpBgMain->SetColour(ct.colors.backgroundMain);
    cpBgPanel->SetColour(ct.colors.backgroundPanel);
    cpBgInput->SetColour(ct.colors.backgroundInput);
    cpBgHeader->SetColour(ct.colors.backgroundGridHeader);
    cpTextPrimary->SetColour(ct.colors.textPrimary);
    cpTextSecondary->SetColour(ct.colors.textSecondary);
    cpTextAccent->SetColour(ct.colors.textAccent);
    cpBtnAccent->SetColour(ct.colors.buttonAccent);
    cpBtnHover->SetColour(ct.colors.buttonHover);
    cpBtnText->SetColour(ct.colors.buttonText);
    cpSuccess->SetColour(ct.colors.colorSuccess);
    cpError->SetColour(ct.colors.colorError);
    cpWarning->SetColour(ct.colors.colorWarning);
    cpInfo->SetColour(ct.colors.colorInfo);
    
    spinTitleSize->SetValue(ct.fonts.titleSize);
    spinHeaderSize->SetValue(ct.fonts.headerSize);
    spinLabelSize->SetValue(ct.fonts.labelSize);
    spinDefaultSize->SetValue(ct.fonts.defaultSize);
    chkBoldHeaders->SetValue(ct.fonts.boldHeaders);
    
    spinBorderWidth->SetValue(ct.buttonStyle.borderWidth);
    spinBorderRadius->SetValue(ct.buttonStyle.borderRadius);
    chkUseGradient->SetValue(ct.buttonStyle.useGradient);
}

theme::CustomTheme UICustomizationDialog::GetCurrentCustomTheme() {
    theme::CustomTheme ct = theme::ThemeManager::GetCustomTheme();
    
    ct.colors.backgroundMain = cpBgMain->GetColour();
    ct.colors.backgroundPanel = cpBgPanel->GetColour();
    ct.colors.backgroundInput = cpBgInput->GetColour();
    ct.colors.backgroundGridHeader = cpBgHeader->GetColour();
    ct.colors.textPrimary = cpTextPrimary->GetColour();
    ct.colors.textSecondary = cpTextSecondary->GetColour();
    ct.colors.textAccent = cpTextAccent->GetColour();
    ct.colors.buttonAccent = cpBtnAccent->GetColour();
    ct.colors.buttonHover = cpBtnHover->GetColour();
    ct.colors.buttonText = cpBtnText->GetColour();
    ct.colors.colorSuccess = cpSuccess->GetColour();
    ct.colors.colorError = cpError->GetColour();
    ct.colors.colorWarning = cpWarning->GetColour();
    ct.colors.colorInfo = cpInfo->GetColour();
    
    ct.fonts.titleSize = spinTitleSize->GetValue();
    ct.fonts.headerSize = spinHeaderSize->GetValue();
    ct.fonts.labelSize = spinLabelSize->GetValue();
    ct.fonts.defaultSize = spinDefaultSize->GetValue();
    ct.fonts.boldHeaders = chkBoldHeaders->GetValue();
    
    ct.buttonStyle.borderWidth = spinBorderWidth->GetValue();
    ct.buttonStyle.borderRadius = spinBorderRadius->GetValue();
    ct.buttonStyle.useGradient = chkUseGradient->GetValue();
    
    ct.type = theme::ThemeType::CUSTOM;
    return ct;
}

void UICustomizationDialog::OnOK(wxCommandEvent& evt) {
    theme::CustomTheme ct = GetCurrentCustomTheme();
    theme::ThemeManager::SetCustomTheme(ct);
    theme::ThemeManager::SaveCustomThemeToFile(theme::ThemeManager::GetSettingsPath());
    EndModal(wxID_OK);
}

void UICustomizationDialog::OnCancel(wxCommandEvent& evt) {
    EndModal(wxID_CANCEL);
}

void UICustomizationDialog::OnReset(wxCommandEvent& evt) {
    theme::CustomTheme defaultTheme;
    defaultTheme.type = theme::ThemeType::DARK;
    
    defaultTheme.colors.backgroundMain = theme::Colors::BACKGROUND_DARK;
    defaultTheme.colors.backgroundPanel = theme::Colors::BACKGROUND_PANEL;
    defaultTheme.colors.backgroundInput = theme::Colors::BACKGROUND_INPUT;
    defaultTheme.colors.backgroundGridHeader = theme::Colors::BACKGROUND_GRID_HEADER;
    defaultTheme.colors.textPrimary = theme::Colors::TEXT_PRIMARY;
    defaultTheme.colors.textSecondary = theme::Colors::TEXT_SECONDARY;
    defaultTheme.colors.textAccent = theme::Colors::TEXT_ACCENT;
    defaultTheme.colors.buttonAccent = theme::Colors::BUTTON_ACCENT;
    defaultTheme.colors.buttonHover = theme::Colors::BUTTON_HOVER;
    defaultTheme.colors.buttonText = theme::Colors::BUTTON_TEXT;
    defaultTheme.colors.colorSuccess = theme::Colors::COLOR_SUCCESS;
    defaultTheme.colors.colorError = theme::Colors::COLOR_ERROR;
    defaultTheme.colors.colorWarning = theme::Colors::COLOR_WARNING;
    defaultTheme.colors.colorInfo = theme::Colors::COLOR_INFO;
    
    defaultTheme.fonts.titleSize = 16;
    defaultTheme.fonts.headerSize = 13;
    defaultTheme.fonts.labelSize = 11;
    defaultTheme.fonts.defaultSize = 10;
    defaultTheme.fonts.boldHeaders = true;
    
    defaultTheme.buttonStyle.borderWidth = 1.0f;
    defaultTheme.buttonStyle.borderRadius = 3;
    defaultTheme.buttonStyle.useGradient = false;
    
    theme::ThemeManager::SetCustomTheme(defaultTheme);
    LoadCurrentTheme();
}

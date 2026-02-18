#include "ThemeHelper.h"
#include <fstream>
#include <sstream>
#include <wx/stdpaths.h>

// Initialize static theme variables
theme::ThemeType theme::ThemeManager::currentTheme = theme::ThemeType::DARK;

// Helper function to initialize custom theme with dark colors
static theme::CustomTheme InitializeDefaultCustomTheme() {
    theme::CustomTheme theme;
    theme.type = theme::ThemeType::DARK;
    
    // Initialize with dark theme colors
    theme.colors.backgroundMain = wxColour(30, 30, 30);
    theme.colors.backgroundPanel = wxColour(37, 37, 38);
    theme.colors.backgroundInput = wxColour(60, 60, 60);
    theme.colors.backgroundGridHeader = wxColour(64, 64, 64);
    theme.colors.textPrimary = wxColour(224, 224, 224);
    theme.colors.textSecondary = wxColour(152, 152, 152);
    theme.colors.textAccent = wxColour(0, 122, 204);
    theme.colors.buttonAccent = wxColour(0, 122, 204);
    theme.colors.buttonHover = wxColour(0, 90, 158);
    theme.colors.buttonText = wxColour(255, 255, 255);
    theme.colors.colorSuccess = wxColour(78, 201, 176);
    theme.colors.colorError = wxColour(244, 135, 113);
    theme.colors.colorWarning = wxColour(220, 220, 170);
    theme.colors.colorInfo = wxColour(86, 156, 214);
    
    // Default fonts
    theme.fonts.titleSize = 16;
    theme.fonts.headerSize = 13;
    theme.fonts.labelSize = 11;
    theme.fonts.defaultSize = 10;
    theme.fonts.boldHeaders = true;
    theme.fonts.fontFamily = "Default";
    
    // Default button style
    theme.buttonStyle.borderWidth = 1.0f;
    theme.buttonStyle.borderRadius = 3;
    theme.buttonStyle.useGradient = false;
    
    return theme;
}

theme::CustomTheme theme::ThemeManager::customTheme = InitializeDefaultCustomTheme();

// ============================================================================
// THEME MANAGER IMPLEMENTATION
// ============================================================================

theme::ThemeType theme::ThemeManager::GetCurrentTheme() {
    return currentTheme;
}

void theme::ThemeManager::SetCurrentTheme(theme::ThemeType type) {
    currentTheme = type;
}

bool theme::ThemeManager::IsDarkTheme() {
    return currentTheme == ThemeType::DARK;
}

theme::CustomTheme& theme::ThemeManager::GetCustomTheme() {
    return customTheme;
}

void theme::ThemeManager::SetCustomTheme(const theme::CustomTheme& theme) {
    customTheme = theme;
    currentTheme = ThemeType::CUSTOM;
}

void theme::ThemeManager::ApplyCustomTheme(const theme::CustomTheme& theme) {
    customTheme = theme;
    currentTheme = ThemeType::CUSTOM;
}

wxString theme::ThemeManager::GetSettingsPath() {
    wxString appDataPath = wxStandardPaths::Get().GetUserDataDir();
    if (!wxDirExists(appDataPath)) {
        wxMkdir(appDataPath);
    }
    return appDataPath + wxFILE_SEP_PATH + "theme_settings.ini";
}

bool theme::ThemeManager::LoadCustomThemeFromFile(const wxString& filePath) {
    std::ifstream file(filePath.ToStdString());
    if (!file.is_open()) {
        return false;
    }

    try {
        CustomTheme theme;
        std::string line;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::stringstream ss(line);
            std::string key, value;
            if (!std::getline(ss, key, '=')) continue;
            if (!std::getline(ss, value)) continue;
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Parse colors (format: R,G,B)
            auto parseColor = [](const std::string& colorStr) -> wxColour {
                try {
                    int r, g, b;
                    if (sscanf(colorStr.c_str(), "%d,%d,%d", &r, &g, &b) == 3) {
                        return wxColour(r, g, b);
                    }
                } catch (...) {}
                return *wxBLACK;
            };
            
            // Parse font settings
            if (key == "titleSize") { theme.fonts.titleSize = std::stoi(value); }
            else if (key == "headerSize") { theme.fonts.headerSize = std::stoi(value); }
            else if (key == "labelSize") { theme.fonts.labelSize = std::stoi(value); }
            else if (key == "defaultSize") { theme.fonts.defaultSize = std::stoi(value); }
            else if (key == "boldHeaders") { theme.fonts.boldHeaders = (value == "true"); }
            else if (key == "fontFamily") { theme.fonts.fontFamily = value; }
            
            // Parse colors
            else if (key == "bgMain") { theme.colors.backgroundMain = parseColor(value); }
            else if (key == "bgPanel") { theme.colors.backgroundPanel = parseColor(value); }
            else if (key == "bgInput") { theme.colors.backgroundInput = parseColor(value); }
            else if (key == "bgHeader") { theme.colors.backgroundGridHeader = parseColor(value); }
            else if (key == "textPrimary") { theme.colors.textPrimary = parseColor(value); }
            else if (key == "textSecondary") { theme.colors.textSecondary = parseColor(value); }
            else if (key == "textAccent") { theme.colors.textAccent = parseColor(value); }
            else if (key == "btnAccent") { theme.colors.buttonAccent = parseColor(value); }
            else if (key == "btnHover") { theme.colors.buttonHover = parseColor(value); }
            else if (key == "btnText") { theme.colors.buttonText = parseColor(value); }
            else if (key == "success") { theme.colors.colorSuccess = parseColor(value); }
            else if (key == "error") { theme.colors.colorError = parseColor(value); }
            else if (key == "warning") { theme.colors.colorWarning = parseColor(value); }
            else if (key == "info") { theme.colors.colorInfo = parseColor(value); }
        }
        
        customTheme = theme;
        currentTheme = ThemeType::CUSTOM;
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

bool theme::ThemeManager::SaveCustomThemeToFile(const wxString& filePath) {
    std::ofstream file(filePath.ToStdString());
    if (!file.is_open()) {
        return false;
    }

    try {
        // Helper to format color
        auto colorToString = [](const wxColour& col) -> std::string {
            char buf[32];
            snprintf(buf, sizeof(buf), "%d,%d,%d", col.Red(), col.Green(), col.Blue());
            return buf;
        };
        
        file << "# Custom Theme Settings\n";
        file << "# Font Settings\n";
        file << "titleSize=" << customTheme.fonts.titleSize << "\n";
        file << "headerSize=" << customTheme.fonts.headerSize << "\n";
        file << "labelSize=" << customTheme.fonts.labelSize << "\n";
        file << "defaultSize=" << customTheme.fonts.defaultSize << "\n";
        file << "boldHeaders=" << (customTheme.fonts.boldHeaders ? "true" : "false") << "\n";
        file << "fontFamily=" << customTheme.fonts.fontFamily.ToStdString() << "\n";
        
        file << "# Color Settings\n";
        file << "bgMain=" << colorToString(customTheme.colors.backgroundMain) << "\n";
        file << "bgPanel=" << colorToString(customTheme.colors.backgroundPanel) << "\n";
        file << "bgInput=" << colorToString(customTheme.colors.backgroundInput) << "\n";
        file << "bgHeader=" << colorToString(customTheme.colors.backgroundGridHeader) << "\n";
        file << "textPrimary=" << colorToString(customTheme.colors.textPrimary) << "\n";
        file << "textSecondary=" << colorToString(customTheme.colors.textSecondary) << "\n";
        file << "textAccent=" << colorToString(customTheme.colors.textAccent) << "\n";
        file << "btnAccent=" << colorToString(customTheme.colors.buttonAccent) << "\n";
        file << "btnHover=" << colorToString(customTheme.colors.buttonHover) << "\n";
        file << "btnText=" << colorToString(customTheme.colors.buttonText) << "\n";
        file << "success=" << colorToString(customTheme.colors.colorSuccess) << "\n";
        file << "error=" << colorToString(customTheme.colors.colorError) << "\n";
        file << "warning=" << colorToString(customTheme.colors.colorWarning) << "\n";
        file << "info=" << colorToString(customTheme.colors.colorInfo) << "\n";
        
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

// ============================================================================
// DARK THEME COLOR DEFINITIONS
// ============================================================================
const wxColour theme::Colors::BACKGROUND_DARK = wxColour(30, 30, 30);           // #1e1e1e
const wxColour theme::Colors::BACKGROUND_PANEL = wxColour(37, 37, 38);          // #252526
const wxColour theme::Colors::BACKGROUND_INPUT = wxColour(60, 60, 60);          // #3c3c3c
const wxColour theme::Colors::BACKGROUND_GRID_HEADER = wxColour(64, 64, 64);    // #404040
const wxColour theme::Colors::TEXT_PRIMARY = wxColour(224, 224, 224);           // #e0e0e0
const wxColour theme::Colors::TEXT_SECONDARY = wxColour(152, 152, 152);         // #989898
const wxColour theme::Colors::TEXT_ACCENT = wxColour(0, 122, 204);              // #007acc
const wxColour theme::Colors::COLOR_SUCCESS = wxColour(78, 201, 176);           // #4ec9b0
const wxColour theme::Colors::COLOR_ERROR = wxColour(244, 135, 113);            // #f48771
const wxColour theme::Colors::COLOR_WARNING = wxColour(220, 220, 170);          // #dcdcaa
const wxColour theme::Colors::COLOR_INFO = wxColour(86, 156, 214);              // #569cd6
const wxColour theme::Colors::BUTTON_ACCENT = wxColour(0, 122, 204);            // #007acc
const wxColour theme::Colors::BUTTON_HOVER = wxColour(0, 90, 158);              // #005a9e
const wxColour theme::Colors::BUTTON_TEXT = wxColour(255, 255, 255);            // #ffffff
const wxColour theme::Colors::ACCENT_TRANSPARENT = wxColour(0, 122, 204, 180); // #007acc with 70% opacity

// ============================================================================
// LIGHT THEME COLOR DEFINITIONS
// ============================================================================
const wxColour theme::LightColors::BACKGROUND_LIGHT = wxColour(240, 240, 240);  // #f0f0f0
const wxColour theme::LightColors::BACKGROUND_PANEL = wxColour(255, 255, 255);  // #ffffff
const wxColour theme::LightColors::BACKGROUND_INPUT = wxColour(255, 255, 255);  // #ffffff
const wxColour theme::LightColors::BACKGROUND_GRID_HEADER = wxColour(224, 224, 224); // #e0e0e0
const wxColour theme::LightColors::TEXT_PRIMARY = wxColour(0, 0, 0);             // #000000
const wxColour theme::LightColors::TEXT_SECONDARY = wxColour(102, 102, 102);    // #666666
const wxColour theme::LightColors::TEXT_ACCENT = wxColour(0, 102, 204);         // #0066cc
const wxColour theme::LightColors::COLOR_SUCCESS = wxColour(0, 128, 0);         // #008000
const wxColour theme::LightColors::COLOR_ERROR = wxColour(255, 0, 0);           // #ff0000
const wxColour theme::LightColors::COLOR_WARNING = wxColour(255, 204, 0);       // #ffcc00
const wxColour theme::LightColors::COLOR_INFO = wxColour(0, 153, 255);          // #0099ff
const wxColour theme::LightColors::BUTTON_ACCENT = wxColour(0, 102, 204);       // #0066cc
const wxColour theme::LightColors::BUTTON_HOVER = wxColour(0, 82, 163);         // #0052a3
const wxColour theme::LightColors::BUTTON_TEXT = wxColour(255, 255, 255);       // #ffffff

// ============================================================================
// FONT DEFINITIONS
// ============================================================================

wxFont theme::Fonts::GetTitleFont() {
    wxFont font(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    return font;
}

wxFont theme::Fonts::GetHeaderFont() {
    wxFont font(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    return font;
}

wxFont theme::Fonts::GetLabelFont() {
    wxFont font(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    return font;
}

wxFont theme::Fonts::GetDefaultFont() {
    wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    return font;
}

wxFont theme::Fonts::GetCustomTitleFont(const FontSettings& settings) {
    wxFontWeight weight = settings.boldHeaders ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL;
    wxFont font(settings.titleSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, weight);
    return font;
}

wxFont theme::Fonts::GetCustomHeaderFont(const FontSettings& settings) {
    wxFontWeight weight = settings.boldHeaders ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL;
    wxFont font(settings.headerSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, weight);
    return font;
}

wxFont theme::Fonts::GetCustomLabelFont(const FontSettings& settings) {
    wxFont font(settings.labelSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    return font;
}

wxFont theme::Fonts::GetCustomDefaultFont(const FontSettings& settings) {
    wxFont font(settings.defaultSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    return font;
}

// ============================================================================
// STYLING IMPLEMENTATION
// ============================================================================

void theme::Styling::ApplyDarkTheme(wxWindow* window) {
    window->SetBackgroundColour(Colors::BACKGROUND_PANEL);
    window->SetForegroundColour(Colors::TEXT_PRIMARY);
}

void theme::Styling::ApplyLightTheme(wxWindow* window) {
    window->SetBackgroundColour(LightColors::BACKGROUND_PANEL);
    window->SetForegroundColour(LightColors::TEXT_PRIMARY);
}

void theme::Styling::ApplyActiveTheme(wxWindow* window) {
    if (ThemeManager::IsDarkTheme()) {
        ApplyDarkTheme(window);
    } else {
        ApplyLightTheme(window);
    }
}

void theme::Styling::StyleButtonAccent(wxButton* button) {
    if (ThemeManager::IsDarkTheme()) {
        button->SetBackgroundColour(Colors::BUTTON_ACCENT);
        button->SetForegroundColour(Colors::BUTTON_TEXT);
    } else {
        button->SetBackgroundColour(LightColors::BUTTON_ACCENT);
        button->SetForegroundColour(LightColors::BUTTON_TEXT);
    }
    button->SetFont(Fonts::GetLabelFont());
}

void theme::Styling::StyleButtonSecondary(wxButton* button) {
    if (ThemeManager::IsDarkTheme()) {
        button->SetBackgroundColour(Colors::BACKGROUND_INPUT);
        button->SetForegroundColour(Colors::TEXT_PRIMARY);
    } else {
        button->SetBackgroundColour(LightColors::BACKGROUND_INPUT);
        button->SetForegroundColour(LightColors::TEXT_PRIMARY);
    }
    button->SetFont(Fonts::GetLabelFont());
}

void theme::Styling::StyleTextControl(wxTextCtrl* ctrl) {
    if (ThemeManager::IsDarkTheme()) {
        ctrl->SetBackgroundColour(Colors::BACKGROUND_INPUT);
        ctrl->SetForegroundColour(Colors::TEXT_PRIMARY);
    } else {
        ctrl->SetBackgroundColour(LightColors::BACKGROUND_INPUT);
        ctrl->SetForegroundColour(LightColors::TEXT_PRIMARY);
    }
    ctrl->SetFont(Fonts::GetDefaultFont());
}

void theme::Styling::StyleAsTitle(wxStaticText* text) {
    if (ThemeManager::IsDarkTheme()) {
        text->SetForegroundColour(Colors::TEXT_PRIMARY);
    } else {
        text->SetForegroundColour(LightColors::TEXT_PRIMARY);
    }
    text->SetFont(Fonts::GetTitleFont());
}

void theme::Styling::StyleAsHeader(wxStaticText* text) {
    if (ThemeManager::IsDarkTheme()) {
        text->SetForegroundColour(Colors::TEXT_PRIMARY);
    } else {
        text->SetForegroundColour(LightColors::TEXT_PRIMARY);
    }
    text->SetFont(Fonts::GetHeaderFont());
}

void theme::Styling::StyleAsLabel(wxStaticText* text) {
    if (ThemeManager::IsDarkTheme()) {
        text->SetForegroundColour(Colors::TEXT_SECONDARY);
    } else {
        text->SetForegroundColour(LightColors::TEXT_SECONDARY);
    }
    text->SetFont(Fonts::GetLabelFont());
}

void theme::Styling::StyleGridDark(wxGrid* grid) {
    grid->SetBackgroundColour(Colors::BACKGROUND_PANEL);
    grid->SetForegroundColour(Colors::TEXT_PRIMARY);
    
    // Style label background
    grid->SetLabelBackgroundColour(Colors::BACKGROUND_GRID_HEADER);
    grid->SetLabelTextColour(Colors::TEXT_PRIMARY);
    
    // Style default cell background
    grid->SetDefaultCellBackgroundColour(Colors::BACKGROUND_INPUT);
    grid->SetDefaultCellTextColour(Colors::TEXT_PRIMARY);
    grid->SetDefaultCellFont(Fonts::GetDefaultFont());
    grid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
    
    // Style grid lines
    grid->SetGridLineColour(Colors::BACKGROUND_GRID_HEADER);
}

void theme::Styling::StyleGridLight(wxGrid* grid) {
    grid->SetBackgroundColour(LightColors::BACKGROUND_PANEL);
    grid->SetForegroundColour(LightColors::TEXT_PRIMARY);
    
    // Style label background
    grid->SetLabelBackgroundColour(LightColors::BACKGROUND_GRID_HEADER);
    grid->SetLabelTextColour(LightColors::TEXT_PRIMARY);
    
    // Style default cell background
    grid->SetDefaultCellBackgroundColour(LightColors::BACKGROUND_INPUT);
    grid->SetDefaultCellTextColour(LightColors::TEXT_PRIMARY);
    grid->SetDefaultCellFont(Fonts::GetDefaultFont());
    grid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
    
    // Style grid lines
    grid->SetGridLineColour(LightColors::BACKGROUND_GRID_HEADER);
}

void theme::Styling::StyleGrid(wxGrid* grid) {
    if (ThemeManager::IsDarkTheme()) {
        StyleGridDark(grid);
    } else {
        StyleGridLight(grid);
    }
}

// ============================================================================
// THEME UTILITIES
// ============================================================================

wxColour theme::ThemeUtils::GetBackgroundColor() {
    return ThemeManager::IsDarkTheme() ? Colors::BACKGROUND_DARK : LightColors::BACKGROUND_LIGHT;
}

wxColour theme::ThemeUtils::GetPanelColor() {
    return ThemeManager::IsDarkTheme() ? Colors::BACKGROUND_PANEL : LightColors::BACKGROUND_PANEL;
}

wxColour theme::ThemeUtils::GetInputColor() {
    return ThemeManager::IsDarkTheme() ? Colors::BACKGROUND_INPUT : LightColors::BACKGROUND_INPUT;
}

wxColour theme::ThemeUtils::GetTextPrimary() {
    return ThemeManager::IsDarkTheme() ? Colors::TEXT_PRIMARY : LightColors::TEXT_PRIMARY;
}

wxColour theme::ThemeUtils::GetTextSecondary() {
    return ThemeManager::IsDarkTheme() ? Colors::TEXT_SECONDARY : LightColors::TEXT_SECONDARY;
}

wxColour theme::ThemeUtils::GetTextAccent() {
    return ThemeManager::IsDarkTheme() ? Colors::TEXT_ACCENT : LightColors::TEXT_ACCENT;
}

wxColour theme::ThemeUtils::GetButtonAccentColor() {
    return ThemeManager::IsDarkTheme() ? Colors::BUTTON_ACCENT : LightColors::BUTTON_ACCENT;
}

wxColour theme::ThemeUtils::GetGridHeaderColor() {
    return ThemeManager::IsDarkTheme() ? Colors::BACKGROUND_GRID_HEADER : LightColors::BACKGROUND_GRID_HEADER;
}

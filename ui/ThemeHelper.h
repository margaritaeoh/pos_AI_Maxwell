#ifndef THEME_HELPER_H
#define THEME_HELPER_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <string>
#include <map>

namespace theme {
    // Theme enumeration
    enum class ThemeType {
        DARK,       // Modern VS Code dark theme
        LIGHT,      // Classic Windows theme
        CUSTOM      // User-customized theme
    };

    // Font customization
    struct FontSettings {
        int titleSize = 16;
        int headerSize = 13;
        int labelSize = 11;
        int defaultSize = 10;
        bool boldHeaders = true;
        wxString fontFamily = "Default";
    };

    // Color customization
    struct ColorSettings {
        wxColour backgroundMain;
        wxColour backgroundPanel;
        wxColour backgroundInput;
        wxColour backgroundGridHeader;
        wxColour textPrimary;
        wxColour textSecondary;
        wxColour textAccent;
        wxColour buttonAccent;
        wxColour buttonHover;
        wxColour buttonText;
        wxColour colorSuccess;
        wxColour colorError;
        wxColour colorWarning;
        wxColour colorInfo;
    };

    // Button styling options
    struct ButtonStyle {
        float borderWidth = 1.0f;
        int borderRadius = 3;
        bool useGradient = false;
    };

    // Complete custom theme structure
    struct CustomTheme {
        ThemeType type = ThemeType::DARK;
        ColorSettings colors;
        FontSettings fonts;
        ButtonStyle buttonStyle;
    };

    // Global theme manager
    class ThemeManager {
    public:
        static ThemeType GetCurrentTheme();
        static void SetCurrentTheme(ThemeType type);
        static bool IsDarkTheme();
        
        // Custom theme management
        static CustomTheme& GetCustomTheme();
        static void SetCustomTheme(const CustomTheme& theme);
        static void ApplyCustomTheme(const CustomTheme& theme);
        
        // Persistence
        static bool LoadCustomThemeFromFile(const wxString& filePath);
        static bool SaveCustomThemeToFile(const wxString& filePath);
        static wxString GetSettingsPath();
        
    private:
        static ThemeType currentTheme;
        static CustomTheme customTheme;
    };

    // VS Code-like Dark Theme Colors
    class Colors {
    public:
        // Background colors
        static const wxColour BACKGROUND_DARK;      // #1e1e1e
        static const wxColour BACKGROUND_PANEL;     // #252526
        static const wxColour BACKGROUND_INPUT;     // #3c3c3c
        static const wxColour BACKGROUND_GRID_HEADER; // #404040 with transparency
        
        // Text colors
        static const wxColour TEXT_PRIMARY;         // #e0e0e0
        static const wxColour TEXT_SECONDARY;       // #989898
        static const wxColour TEXT_ACCENT;          // #007acc (VS Code blue)
        
        // Status colors
        static const wxColour COLOR_SUCCESS;        // #4ec9b0
        static const wxColour COLOR_ERROR;          // #f48771
        static const wxColour COLOR_WARNING;        // #dcdcaa
        static const wxColour COLOR_INFO;           // #569cd6
        
        // Button colors
        static const wxColour BUTTON_ACCENT;        // #007acc (Primary action)
        static const wxColour BUTTON_HOVER;         // #005a9e (Darker blue)
        static const wxColour BUTTON_TEXT;          // #ffffff
        
        // Transparent variants
        static const wxColour ACCENT_TRANSPARENT;   // #007acc with transparency
    };

    // Light/Classic Theme Colors
    class LightColors {
    public:
        static const wxColour BACKGROUND_LIGHT;     // #f0f0f0
        static const wxColour BACKGROUND_PANEL;     // #ffffff
        static const wxColour BACKGROUND_INPUT;     // #ffffff
        static const wxColour BACKGROUND_GRID_HEADER; // #e0e0e0
        
        static const wxColour TEXT_PRIMARY;         // #000000
        static const wxColour TEXT_SECONDARY;       // #666666
        static const wxColour TEXT_ACCENT;          // #0066cc
        
        static const wxColour COLOR_SUCCESS;        // #008000
        static const wxColour COLOR_ERROR;          // #ff0000
        static const wxColour COLOR_WARNING;        // #ffcc00
        static const wxColour COLOR_INFO;           // #0099ff
        
        static const wxColour BUTTON_ACCENT;        // #0066cc
        static const wxColour BUTTON_HOVER;         // #0052a3
        static const wxColour BUTTON_TEXT;          // #ffffff
    };

    class Fonts {
    public:
        // Main title font
        static wxFont GetTitleFont();
        
        // Section header font
        static wxFont GetHeaderFont();
        
        // Standard label font
        static wxFont GetLabelFont();
        
        // Input/default font
        static wxFont GetDefaultFont();
        
        // Custom fonts based on settings
        static wxFont GetCustomTitleFont(const FontSettings& settings);
        static wxFont GetCustomHeaderFont(const FontSettings& settings);
        static wxFont GetCustomLabelFont(const FontSettings& settings);
        static wxFont GetCustomDefaultFont(const FontSettings& settings);
    };

    class Styling {
    public:
        // Apply current theme to a window
        static void ApplyDarkTheme(wxWindow* window);
        
        // Apply light theme to a window
        static void ApplyLightTheme(wxWindow* window);
        
        // Apply current active theme
        static void ApplyActiveTheme(wxWindow* window);
        
        // Style a button with accent color (uses current theme)
        static void StyleButtonAccent(wxButton* button);
        
        // Style a button as secondary (uses current theme)
        static void StyleButtonSecondary(wxButton* button);
        
        // Style a text control (uses current theme)
        static void StyleTextControl(wxTextCtrl* ctrl);
        
        // Style a static text as title
        static void StyleAsTitle(wxStaticText* text);
        
        // Style a static text as section header
        static void StyleAsHeader(wxStaticText* text);
        
        // Style a static text as label
        static void StyleAsLabel(wxStaticText* text);
        
        // Style a wxGrid for dark theme with proper header colors
        static void StyleGridDark(wxGrid* grid);
        
        // Style a wxGrid for light theme
        static void StyleGridLight(wxGrid* grid);
        
        // Style a wxGrid using current theme
        static void StyleGrid(wxGrid* grid);
    };

    // Helper function to get appropriate color based on current theme
    class ThemeUtils {
    public:
        static wxColour GetBackgroundColor();
        static wxColour GetPanelColor();
        static wxColour GetInputColor();
        static wxColour GetTextPrimary();
        static wxColour GetTextSecondary();
        static wxColour GetTextAccent();
        static wxColour GetButtonAccentColor();
        static wxColour GetGridHeaderColor();
    };
}

#endif

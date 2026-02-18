#ifndef UI_CUSTOMIZATION_DIALOG_H
#define UI_CUSTOMIZATION_DIALOG_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>
#include "ThemeHelper.h"

class UICustomizationDialog : public wxDialog {
public:
    UICustomizationDialog(wxWindow* parent);
    ~UICustomizationDialog() = default;

private:
    // UI Components
    wxNotebook* notebook;
    
    // Color panel controls
    wxColourPickerCtrl* cpBgMain;
    wxColourPickerCtrl* cpBgPanel;
    wxColourPickerCtrl* cpBgInput;
    wxColourPickerCtrl* cpBgHeader;
    wxColourPickerCtrl* cpTextPrimary;
    wxColourPickerCtrl* cpTextSecondary;
    wxColourPickerCtrl* cpTextAccent;
    wxColourPickerCtrl* cpBtnAccent;
    wxColourPickerCtrl* cpBtnHover;
    wxColourPickerCtrl* cpBtnText;
    wxColourPickerCtrl* cpSuccess;
    wxColourPickerCtrl* cpError;
    wxColourPickerCtrl* cpWarning;
    wxColourPickerCtrl* cpInfo;
    
    // Font panel controls
    wxSpinCtrl* spinTitleSize;
    wxSpinCtrl* spinHeaderSize;
    wxSpinCtrl* spinLabelSize;
    wxSpinCtrl* spinDefaultSize;
    wxCheckBox* chkBoldHeaders;
    
    // Button style controls
    wxSpinCtrlDouble* spinBorderWidth;
    wxSpinCtrl* spinBorderRadius;
    wxCheckBox* chkUseGradient;
    
    // Event handlers
    void OnOK(wxCommandEvent& evt);
    void OnCancel(wxCommandEvent& evt);
    void OnReset(wxCommandEvent& evt);
    
    // Helper methods
    wxPanel* CreateColorPanel(wxWindow* parent);
    wxPanel* CreateFontPanel(wxWindow* parent);
    wxPanel* CreateButtonStylePanel(wxWindow* parent);
    
    void LoadCurrentTheme();
    theme::CustomTheme GetCurrentCustomTheme();
};

#endif // UI_CUSTOMIZATION_DIALOG_H

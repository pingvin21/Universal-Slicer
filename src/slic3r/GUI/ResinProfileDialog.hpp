#pragma once

#include "ResinPrinterProfile.hpp"
#include "ResinProfileStorage.hpp"

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/button.h>

#include <vector>
#include <functional>

namespace Slic3r::GUI {

class ResinProfileDialog : public wxDialog {
public:
    explicit ResinProfileDialog(
        wxWindow* parent,
        const ResinPrinterProfile& profile);

    const ResinPrinterProfile& profile() const {
        return m_profile;
    }

private:
    ResinPrinterProfile m_profile;

    wxTextCtrl* m_printer_name = nullptr;
    wxTextCtrl* m_build_x = nullptr;
    wxTextCtrl* m_build_y = nullptr;
    wxTextCtrl* m_build_z = nullptr;

    wxTextCtrl* m_resolution_x = nullptr;
    wxTextCtrl* m_resolution_y = nullptr;

    wxTextCtrl* m_layer_height = nullptr;
    wxTextCtrl* m_normal_exposure = nullptr;
    wxTextCtrl* m_bottom_exposure = nullptr;
    wxTextCtrl* m_bottom_layers = nullptr;

    wxTextCtrl* m_lift_distance = nullptr;
    wxTextCtrl* m_lift_speed = nullptr;
    wxTextCtrl* m_retract_speed = nullptr;

    void create_controls();
    bool read_and_validate();
    void on_save(wxCommandEvent& event);
    void on_load(wxCommandEvent& event);
};

}

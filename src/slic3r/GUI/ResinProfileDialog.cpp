#include "ResinProfileDialog.hpp"

#include <wx/msgdlg.h>
#include <wx/filedlg.h>

namespace Slic3r::GUI {

ResinProfileDialog::ResinProfileDialog(
    wxWindow* parent,
    const ResinPrinterProfile& profile)
    : wxDialog(
          parent,
          wxID_ANY,
          "Resin Printer Settings",
          wxDefaultPosition,
          wxSize(560, 720),
          wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      m_profile(profile)
{
    create_controls();
    Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        if (read_and_validate())
            EndModal(wxID_OK);
    }, wxID_OK);

    CentreOnParent();
}

void ResinProfileDialog::create_controls()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    auto* grid = new wxFlexGridSizer(2, 8, 8);
    grid->AddGrowableCol(1, 1);

    auto add_field = [&](const wxString& label,
                         const wxString& value) -> wxTextCtrl* {
        grid->Add(
            new wxStaticText(this, wxID_ANY, label),
            0,
            wxALIGN_CENTER_VERTICAL
        );

        auto* field = new wxTextCtrl(
            this, wxID_ANY, value
        );

        grid->Add(field, 1, wxEXPAND);
        return field;
    };

    m_printer_name = add_field(
        "Printer name",
        wxString::FromUTF8(m_profile.printer_name.c_str())
    );

    m_build_x = add_field(
        "Build X (mm)", wxString::Format("%.3f", m_profile.build_x_mm)
    );
    m_build_y = add_field(
        "Build Y (mm)", wxString::Format("%.3f", m_profile.build_y_mm)
    );
    m_build_z = add_field(
        "Build Z (mm)", wxString::Format("%.3f", m_profile.build_z_mm)
    );

    m_resolution_x = add_field(
        "LCD resolution X", wxString::Format("%u", m_profile.resolution_x)
    );
    m_resolution_y = add_field(
        "LCD resolution Y", wxString::Format("%u", m_profile.resolution_y)
    );

    m_layer_height = add_field(
        "Layer height (mm)",
        wxString::Format("%.3f", m_profile.layer_height_mm)
    );

    m_normal_exposure = add_field(
        "Normal exposure (s)",
        wxString::Format("%.2f", m_profile.normal_exposure_s)
    );

    m_bottom_exposure = add_field(
        "Bottom exposure (s)",
        wxString::Format("%.2f", m_profile.bottom_exposure_s)
    );

    m_bottom_layers = add_field(
        "Bottom layers", wxString::Format("%u", m_profile.bottom_layers)
    );

    m_lift_distance = add_field(
        "Lift distance (mm)",
        wxString::Format("%.2f", m_profile.lift_distance_mm)
    );

    m_lift_speed = add_field(
        "Lift speed (mm/min)",
        wxString::Format("%.2f", m_profile.lift_speed_mm_min)
    );

    m_retract_speed = add_field(
        "Retract speed (mm/min)",
        wxString::Format("%.2f", m_profile.retract_speed_mm_min)
    );

    main_sizer->Add(grid, 1, wxEXPAND | wxALL, 20);

    // Universal-Slicer: Resin profile actions
    auto* profile_buttons = new wxBoxSizer(wxHORIZONTAL);

    auto* save_button = new wxButton(
        this, wxID_ANY, "Save Profile"
    );

    auto* load_button = new wxButton(
        this, wxID_ANY, "Load Profile"
    );

    profile_buttons->Add(save_button, 0, wxRIGHT, 10);
    profile_buttons->Add(load_button, 0);

    main_sizer->Add(
        profile_buttons, 0, wxLEFT | wxRIGHT | wxBOTTOM, 20
    );

    save_button->Bind(
        wxEVT_BUTTON, &ResinProfileDialog::on_save, this
    );

    load_button->Bind(
        wxEVT_BUTTON, &ResinProfileDialog::on_load, this
    );

    auto* buttons = CreateSeparatedButtonSizer(wxOK | wxCANCEL);

    if (buttons)
        main_sizer->Add(buttons, 0, wxEXPAND | wxALL, 10);

    SetSizerAndFit(main_sizer);
    SetMinSize(FromDIP(wxSize(500, 600)));
}

}

namespace Slic3r::GUI {

bool ResinProfileDialog::read_and_validate()
{
    ResinPrinterProfile candidate = m_profile;

    candidate.printer_name = m_printer_name->GetValue().ToStdString();

    auto read_double = [](wxTextCtrl* control, double& value) {
        return control->GetValue().ToDouble(&value);
    };

    auto read_uint = [](wxTextCtrl* control, uint32_t& value) {
        unsigned long number = 0;

        if (!control->GetValue().ToULong(&number))
            return false;

        if (number > UINT32_MAX)
            return false;

        value = static_cast<uint32_t>(number);
        return true;
    };

    bool valid =
        read_double(m_build_x, candidate.build_x_mm) &&
        read_double(m_build_y, candidate.build_y_mm) &&
        read_double(m_build_z, candidate.build_z_mm) &&
        read_uint(m_resolution_x, candidate.resolution_x) &&
        read_uint(m_resolution_y, candidate.resolution_y) &&
        read_double(m_layer_height, candidate.layer_height_mm) &&
        read_double(m_normal_exposure, candidate.normal_exposure_s) &&
        read_double(m_bottom_exposure, candidate.bottom_exposure_s) &&
        read_uint(m_bottom_layers, candidate.bottom_layers) &&
        read_double(m_lift_distance, candidate.lift_distance_mm) &&
        read_double(m_lift_speed, candidate.lift_speed_mm_min) &&
        read_double(m_retract_speed, candidate.retract_speed_mm_min);

    if (!valid) {
        wxMessageBox(
            "Please check the numeric values.",
            "Invalid Resin Settings",
            wxOK | wxICON_ERROR,
            this
        );
        return false;
    }

    std::string error;

    if (!candidate.validate(error)) {
        wxMessageBox(
            wxString::FromUTF8(error.c_str()),
            "Invalid Resin Settings",
            wxOK | wxICON_ERROR,
            this
        );
        return false;
    }

    m_profile = candidate;
    return true;
}


} // namespace Slic3r::GUI

// Universal-Slicer: profile file operations
namespace Slic3r::GUI {

void ResinProfileDialog::on_save(wxCommandEvent&)
{
    if (!read_and_validate())
        return;

    wxFileDialog dialog(
        this,
        "Save Resin Profile",
        "",
        "resin_profile.json",
        "Resin profiles (*.json)|*.json",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT
    );

    if (dialog.ShowModal() != wxID_OK)
        return;

    try {
        ResinProfileStorage::save(
            m_profile,
            std::filesystem::path(dialog.GetPath().ToStdWstring())
        );

        wxMessageBox(
            "Resin profile saved successfully.",
            "Universal-Slicer",
            wxOK | wxICON_INFORMATION,
            this
        );
    }
    catch (const std::exception& e) {
        wxMessageBox(
            wxString::FromUTF8(e.what()),
            "Profile Save Error",
            wxOK | wxICON_ERROR,
            this
        );
    }
}

void ResinProfileDialog::on_load(wxCommandEvent&)
{
    wxFileDialog dialog(
        this,
        "Load Resin Profile",
        "",
        "",
        "Resin profiles (*.json)|*.json",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST
    );

    if (dialog.ShowModal() != wxID_OK)
        return;

    try {
        ResinPrinterProfile loaded = ResinProfileStorage::load(
            std::filesystem::path(dialog.GetPath().ToStdWstring())
        );

        m_profile = loaded;

        m_printer_name->SetValue(wxString::FromUTF8(loaded.printer_name.c_str()));

        m_build_x->SetValue(wxString::Format("%.3f", loaded.build_x_mm));
        m_build_y->SetValue(wxString::Format("%.3f", loaded.build_y_mm));
        m_build_z->SetValue(wxString::Format("%.3f", loaded.build_z_mm));

        m_resolution_x->SetValue(wxString::Format("%u", loaded.resolution_x));
        m_resolution_y->SetValue(wxString::Format("%u", loaded.resolution_y));

        m_layer_height->SetValue(wxString::Format("%.3f", loaded.layer_height_mm));
        m_normal_exposure->SetValue(wxString::Format("%.2f", loaded.normal_exposure_s));
        m_bottom_exposure->SetValue(wxString::Format("%.2f", loaded.bottom_exposure_s));
        m_bottom_layers->SetValue(wxString::Format("%u", loaded.bottom_layers));

        m_lift_distance->SetValue(wxString::Format("%.2f", loaded.lift_distance_mm));
        m_lift_speed->SetValue(wxString::Format("%.2f", loaded.lift_speed_mm_min));
        m_retract_speed->SetValue(wxString::Format("%.2f", loaded.retract_speed_mm_min));
    }
    catch (const std::exception& e) {
        wxMessageBox(
            wxString::FromUTF8(e.what()),
            "Profile Load Error",
            wxOK | wxICON_ERROR,
            this
        );
    }
}

} // namespace Slic3r::GUI

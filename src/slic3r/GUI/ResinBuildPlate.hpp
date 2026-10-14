#pragma once

#include "ResinPrinterProfile.hpp"

#include <wx/panel.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <algorithm>
#include <cmath>

namespace Slic3r::GUI {

class ResinBuildPlate final : public wxPanel
{
public:
    ResinBuildPlate(wxWindow* parent, const ResinPrinterProfile& profile)
        : wxPanel(parent, wxID_ANY),
          m_profile(profile)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetBackgroundColour(wxColour(28, 34, 38));
        SetMinSize(wxSize(300, 250));

        Bind(wxEVT_PAINT, &ResinBuildPlate::on_paint, this);
    }

    void set_profile(const ResinPrinterProfile& profile)
    {
        m_profile = profile;
        Refresh();
    }

private:
    ResinPrinterProfile m_profile;

    void on_paint(wxPaintEvent&)
    {
        wxAutoBufferedPaintDC dc(this);
        dc.SetBackground(wxBrush(wxColour(28, 34, 38)));
        dc.Clear();

        const wxSize size = GetClientSize();

        const double available_w = std::max(1, size.x - 100);
        const double available_h = std::max(1, size.y - 100);

        const double scale = std::min(
            available_w / m_profile.build_x_mm,
            available_h / m_profile.build_y_mm
        );

        const double plate_w = m_profile.build_x_mm * scale;
        const double plate_h = m_profile.build_y_mm * scale;

        const int left = static_cast<int>((size.x - plate_w) / 2.0);
        const int top = static_cast<int>((size.y - plate_h) / 2.0);

        dc.SetPen(wxPen(wxColour(90, 160, 175), 2));
        dc.SetBrush(wxBrush(wxColour(48, 65, 72)));
        dc.DrawRectangle(
            left,
            top,
            static_cast<int>(plate_w),
            static_cast<int>(plate_h)
        );

        dc.SetPen(wxPen(wxColour(75, 95, 105), 1));

        const double grid_mm = 10.0;

        for (double x = grid_mm; x < m_profile.build_x_mm; x += grid_mm) {
            const int px = left + static_cast<int>(x * scale);
            dc.DrawLine(px, top, px, top + static_cast<int>(plate_h));
        }

        for (double y = grid_mm; y < m_profile.build_y_mm; y += grid_mm) {
            const int py = top + static_cast<int>(y * scale);
            dc.DrawLine(left, py, left + static_cast<int>(plate_w), py);
        }

        dc.SetTextForeground(wxColour(220, 230, 235));

        const wxString dimensions = wxString::Format(
            "Build plate: %.1f x %.1f mm | Height: %.1f mm",
            m_profile.build_x_mm,
            m_profile.build_y_mm,
            m_profile.build_z_mm
        );

        dc.DrawText(dimensions, 15, 15);
    }
};

} // namespace Slic3r::GUI

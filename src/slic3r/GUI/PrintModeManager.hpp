#pragma once

namespace Slic3r::GUI {

enum class PrintMode {
    FDM,
    Resin
};

class PrintModeManager {
public:
    static PrintModeManager& instance()
    {
        static PrintModeManager manager;
        return manager;
    }

    PrintMode mode() const
    {
        return m_mode;
    }

    void set_mode(PrintMode mode)
    {
        m_mode = mode;
    }

    bool is_fdm() const
    {
        return m_mode == PrintMode::FDM;
    }

    bool is_resin() const
    {
        return m_mode == PrintMode::Resin;
    }

private:
    PrintModeManager() = default;

    PrintMode m_mode = PrintMode::FDM;
};

}

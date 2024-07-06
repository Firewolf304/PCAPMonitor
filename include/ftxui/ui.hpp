//
// Created by firewolf on 06.07.24.
//

#pragma once
#include "../include.hpp"

namespace ui {
    class ui {
    public:
        ui() : screen(ftxui::ScreenInteractive::Fullscreen()) {}
        void makeSelectionMenu(const std::vector<std::string> & elements, std::function<void()> todo = [](){});
    private:
        ftxui::ScreenInteractive screen;
    };
}

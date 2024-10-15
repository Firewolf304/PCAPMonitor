//
// Created by firewolf on 06.07.24.
//

#pragma once
#include "../include.hpp"

namespace ui {
    class ui {
    public:
        ui() : screen(ftxui::ScreenInteractive::Fullscreen()) {}
        void makeSelectionDeviceMenu(const std::vector<std::string> & elements);
        std::string makeSelectionDeviceMenuWithComments(std::unordered_map<std::string, std::unordered_map<std::string, std::string>> & elements);
        void showDevice(std::string & device);
    private:
        ftxui::ScreenInteractive screen;
    };
}

//
// Created by firewolf on 06.07.24.
//

#include "../../include/ftxui/ui.hpp"

void ui::ui::makeSelectionMenu(const std::vector<std::string> & elements, std::function<void()> todo = [](){}) {
    int selected = 0;
    auto menu = ftxui::Menu( &elements, &selected);
    auto container = ftxui::Container::Vertical({
        menu,
        ftxui::Button("Quit", [&](){ this->screen.Exit(); })
    });
    auto renderer = ftxui::Renderer(container, [&] -> std::shared_ptr<ftxui::Node> {
        return ftxui::window(ftxui::text("Select device:") | ftxui::bold,
     ftxui::vbox({
                  menu->Render() | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 10) | ftxui::size(ftxui::WIDTH, ftxui::LESS_THAN, 30),
          })
        ) | ftxui::center;
    });
    std::atomic<bool> refresh_ui_continue = true;
    /*auto thread = std::async(std::launch::async, [&](){
        while (refresh_ui_continue) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(0.05s);
            //screen.Post([&] { shift++; });
            //screen.Post(ftxui::Event::Custom);
        }
    });*/
    screen.Loop(renderer);
}

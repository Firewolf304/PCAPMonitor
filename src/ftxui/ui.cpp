//
// Created by firewolf on 06.07.24.
//

#include "../../include/ftxui/ui.hpp"

void ui::ui::makeSelectionDeviceMenu(const std::vector<std::string> & elements) {
    int selected = 0;
    auto menu = ftxui::Menu( &elements, &selected);
    auto container = ftxui::Container::Vertical({
        menu,
        ftxui::Button("Quit", [&](){ this->screen.Exit(); })
    });
    auto renderer = ftxui::Renderer(container, [&] -> std::shared_ptr<ftxui::Node> {
        auto GetInfo = [&](const std::string & device) -> std::string {
            return "DEVICE: " + device + "\n";
        };

        return ftxui::window(ftxui::text("Select device:") | ftxui::bold,
     ftxui::hbox({
                  menu->Render() | ftxui::frame | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 10),
                  ftxui::text(GetInfo(elements[selected])) | ftxui::border | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 30)
          })
        ) | ftxui::center ;
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

std::string ui::ui::makeSelectionDeviceMenuWithComments(std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &data) {
    int selected = 0;
    auto get_data = [&]() -> std::generator<const std::string&> {
        for(auto v : data) {
            co_yield v.first;
        }
    };
    std::vector<std::string> elements;
    for(auto v : get_data()) {
        elements.emplace_back(v);
    }
    ftxui::MenuOption option;
    option.on_enter = screen.ExitLoopClosure();
    auto menu = ftxui::Menu( &elements, &selected, option);
    auto container = ftxui::Container::Vertical({
                                                        menu,
                                                        //ftxui::Button("Quit", [&](){ this->screen.Exit(); })
                                                });
    auto renderer = ftxui::Renderer(container, [&] -> std::shared_ptr<ftxui::Node> {
        auto GetInfo = [&](const std::string & device) -> ftxui::Elements {
            ftxui::Elements text_elements;
            if(data.contains(device)) {
                for(auto line : data[device]) {
                    if(!line.second.empty())
                        text_elements.push_back( ftxui::text(line.first + ": " + line.second));
                }
                std::reverse(text_elements.begin(), text_elements.end());
            } else {
                text_elements.push_back(ftxui::text("NULL"));
            }
            return text_elements;
        };

        return ftxui::window(ftxui::text("Select device:") | ftxui::bold,
         ftxui::hbox({
                            ftxui::vbox(
                             ftxui::frame | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 50),
                             menu->Render()
                             ),
                            ftxui::vbox(
                             ftxui::vbox(GetInfo(elements[selected])) | ftxui::border | ftxui::flex
                            )
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
    return elements[selected];
}

void ui::ui::showDevice(std::string &device) {
    std::unordered_map<std::string, std::tuple<std::string, bool, std::function<void()>>> my_map = {
        {"status", {{}, true, [&my_map, &device]()->void{
            std::async(std::launch::async, [&device](const bool & stop){

            }, std::get<bool>(my_map["status"]));
            std::get<bool>(my_map["status"]) = false;
        }}}
    };


}

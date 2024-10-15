//
// Created by firewolf on 06.07.24.
//

#pragma once
#include "../include/local.hpp"
namespace firewolf::wifi {
    class GUI_pcap_parse {
    public:
        GUI_pcap_parse() = default;

        void CreateSelectDeviceMenu() {
            auto data = libnm.get_data_devices();
            for(auto dev : data) {
                auto add = monitor.getInfoDevice(dev.first);
                data[dev.first].insert(add.begin(), add.end());
            }
            std::print("Selected: {0}", gui.makeSelectionDeviceMenuWithComments( data )) ;
        }
    private:
        monitors::pcap_monitor monitor;
        firewolf::wifi::parsing_libnm libnm;
        ui::ui gui;
    };
}

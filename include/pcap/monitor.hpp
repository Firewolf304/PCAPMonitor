//
// Created by firewolf on 05.07.24.
//

#pragma once
#include "../include.hpp"
namespace monitors {
    class pcap_monitor {
    public:
        pcap_monitor() = default;
        pcap_monitor(const std::string & device_name) {

        }
        std::generator<std::string> getStringNameDevicesGenerator();
        std::vector<std::string> getStringNameDevicesVector();
        std::vector<pcpp::PcapLiveDevice*> getDevices();
        std::unordered_map<std::string,std::string> getInfoDevice(std::string device);
        pcpp::PcapLiveDevice* selectedDevice();
        void selectDevice(pcpp::PcapLiveDevice* liveDevice);
        void selectDevice(std::string device_name);
        void selectDevice(int device_index);
        std::unordered_map<std::string, std::tuple<std::string, bool, std::function<void()>>> createFuncList(std::string & device);
        void StartCapture();
    private:
        pcpp::PcapLiveDevice* dev;
    };
}

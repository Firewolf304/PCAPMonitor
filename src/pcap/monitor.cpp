//
// Created by firewolf on 05.07.24.
//
#include "../../include/pcap/monitor.hpp"

std::vector< pcpp::PcapLiveDevice*> monitors::pcap_monitor::getDevices() {
    return pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
}

std::generator<std::string> monitors::pcap_monitor::getStringNameDevicesGenerator() {
    for(auto dev : this->getDevices()) {
        co_yield dev->getName();
    }
}

std::vector<std::string> monitors::pcap_monitor::getStringNameDevicesVector() {
    auto vec = std::vector<std::string>();
    for(auto dev : this->getDevices()) {
        vec.push_back(dev->getName());
    }
    return vec;
}

pcpp::PcapLiveDevice *monitors::pcap_monitor::selectedDevice() {
    return this->dev;
}

void monitors::pcap_monitor::StartCapture() {

    while(true) {

    }
}

void monitors::pcap_monitor::selectDevice(pcpp::PcapLiveDevice *liveDevice) {
    this->dev = liveDevice;
}

void monitors::pcap_monitor::selectDevice(std::string device_name) {
    this->dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIpOrName(device_name);
}

void monitors::pcap_monitor::selectDevice(int device_index) {
    this->dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList()[device_index];
}

std::unordered_map<std::string,std::string> monitors::pcap_monitor::getInfoDevice(std::string device_name) {
    auto device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(device_name);
    if(device) {
        std::string ipv4;
        try {
            ipv4 = device->getIPv4Address().toString();
        } catch (const std::exception &e) {}
        std::string ipv6;
        try {
            ipv6 = device->getIPv6Address().toString();
        } catch (const std::exception &e) {}
        std::string mac;
        try {
            mac = device->getMacAddress().toString();
        } catch (const std::exception &e) {}
        std::string mtu;
        try {
            mtu = std::to_string(device->getMtu());
        } catch (const std::exception &e) {}
        return {
                {"IPv4", ipv4},
                {"IPv6", ipv6},
                {"MAC",  mac},
                {"MTU",  mtu}
        };
    }
    return {};
}

std::unordered_map<std::string, std::tuple<std::string, bool, std::function<void()>>>
monitors::pcap_monitor::createFuncList(std::string & device ) {
    std::unordered_map<std::string, std::tuple<std::string, bool, std::function<void()>>> my_map = {
            {"status", {{}, true, [&my_map, &device]()->void{
                std::async(std::launch::async, [&device](const bool & stop){
                    pcpp::
                }, std::get<bool>(my_map["status"]));
                std::get<bool>(my_map["status"]) = false;
            }}}
    };
    return my_map;
}



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




#include "../include/local.hpp"
int main() {
    std::cout << "Hello, World!" << std::endl;
    monitors::pcap_monitor monitor;
    for(auto res : monitor.getDevices()) {
        std::print("{} {} {}\n", res->getName(), res->getIPv4Address().toString(), res->captureActive());
    }
    return 0;
}

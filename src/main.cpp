#include "../include/local.hpp"
int main() {
    std::cout << "Hello, World!" << std::endl;
    monitors::pcap_monitor monitor;
    ui::ui window;
    window.makeSelectionMenu(monitor.getStringNameDevicesVector(), [&]() {
        monitor.selectDevice()
    });
    return 0;
}

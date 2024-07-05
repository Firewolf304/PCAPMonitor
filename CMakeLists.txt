cmake_minimum_required(VERSION 3.28)
project(PCAPMonitor)

set(CMAKE_CXX_STANDARD 26)
find_package(PcapPlusPlus CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)
add_executable(PCAPMonitor src/main.cpp
        include/include.hpp
        include/pcap/monitor.hpp
        src/pcap/monitor.cpp
        include/local.hpp
        include/cpp_include.hpp)

target_link_libraries(PCAPMonitor PRIVATE PCAP::PCAP PcapPlusPlus::Pcap++ PcapPlusPlus::Common++ PcapPlusPlus::Packet++)
target_link_libraries(PCAPMonitor PRIVATE ftxui::dom ftxui::screen ftxui::component)
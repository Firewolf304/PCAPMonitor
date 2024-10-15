cmake_minimum_required(VERSION 3.28)
project(PCAPMonitor)

set(CMAKE_CXX_STANDARD 26)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_INCLUDE_DIRS /usr/include/boost)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lboost_log")

find_package(PkgConfig REQUIRED)

find_package(PcapPlusPlus CONFIG REQUIRED)
find_package(ftxui CONFIG REQUIRED)
find_package(Boost 1.83.0    COMPONENTS log REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})

pkg_check_modules(LIBNM REQUIRED IMPORTED_TARGET libnm)
pkg_check_modules(GLIB REQUIRED glib-2.0)
link_libraries( ${LIBNM_LIBRARIES} ${GLIB_LIBRARIES})
include_directories( ${LIBNM_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
add_compile_options( ${LIBNM_CFLAGS_OTHER} ${GLIB_CFLAGS_OTHER})

add_definitions(-DBOOST_LOG_DYN_LINK)

add_executable(PCAPMonitor src/main.cpp
        include/include.hpp
        include/pcap/monitor.hpp
        src/pcap/monitor.cpp
        include/local.hpp
        include/cpp_include.hpp
        include/ftxui/ui.hpp
        src/ftxui/ui.cpp
        src/networking/parsing_libnm.cpp
        include/working.hpp)

#target_link_libraries(PCAPMonitor ${Boost_LIBRARIES})
target_link_libraries(PCAPMonitor PRIVATE PCAP::PCAP PcapPlusPlus::Pcap++ PcapPlusPlus::Common++ PcapPlusPlus::Packet++)
target_link_libraries(PCAPMonitor PRIVATE ftxui::dom ftxui::screen ftxui::component)
target_link_libraries(PCAPMonitor PUBLIC PkgConfig::LIBNM)
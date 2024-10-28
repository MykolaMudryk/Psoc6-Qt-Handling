cmake_minimum_required(VERSION 3.16)

project(MudrykTestTask LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick SerialPort)

qt_standard_project_setup()

qt_policy(SET QTP0001 NEW)

qt_add_executable(MudrykTestTask
    src/main.cpp
)

qt_add_qml_module(MudrykTestTask
    URI MudrykTestTask
    VERSION 1.0

    SOURCES
    src/blinking_handler.cpp
    include/blinking_handler.h

    RESOURCES
        qml.qrc

)


set_target_properties(MudrykTestTask PROPERTIES
    MACOSX_BUNDLE TRUE

)

target_link_libraries(MudrykTestTask
    PRIVATE Qt6::Quick Qt6::SerialPort
)

include(GNUInstallDirs)
install(TARGETS MudrykTestTask
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

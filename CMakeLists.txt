cmake_minimum_required(VERSION 3.16)

if (DEFINED ESP_PLATFORM)

    # idf component — header-only, no source files to compile
    idf_component_register(
        INCLUDE_DIRS src
        REQUIRES hal driver
    )
    # activate ESP32 I2C
    add_compile_definitions(ESP32 ESP32_CMAKE)
    add_compile_options(-Wno-unused-variable)

else()

    # set the project name
    project(arduino-audio-driver)

    set(CMAKE_CXX_STANDARD 17)

    # Header-only library: no source files, consumers get the include path
    add_library(arduino-audio-driver INTERFACE)

    target_compile_options(arduino-audio-driver INTERFACE -DUSE_DEFAULT_STDLIB -Wno-unused-variable)

    # define location for header files
    target_include_directories(arduino-audio-driver INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

    # Add Zephyr support
    if (DEFINED ZEPHYR_PLATFORM)
        target_compile_definitions(arduino-audio-driver INTERFACE __zephyr__)
        target_link_libraries(arduino-audio-driver INTERFACE zephyr_interface zephyr_generated_headers)
    endif()

endif()

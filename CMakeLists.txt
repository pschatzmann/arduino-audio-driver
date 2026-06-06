cmake_minimum_required(VERSION 3.16)

if (DEFINED ESP_PLATFORM)

    # idf component
    idf_component_register(
        SRC_DIRS src/Platforms src/Codecs/ac101 src/Codecs/cs42448 src/Codecs/es7243 src/Codecs/es8156 src/Codecs/es8374 src/Codecs/pcm3168 src/Codecs/wm8960 src/Codecs/wm8994 src/Codecs/cs43l22 src/Codecs/es7210 src/Codecs/es7243e src/Codecs/es8311 src/Codecs/es8388 src/Codecs/tas5805m src/Codecs/wm8978
        INCLUDE_DIRS src 
        REQUIRES hal driver
    )
    # activate ESP32 I2C
    add_compile_definitions(ESP32 ESP32_CMAKE)
    add_compile_options(-Wno-unused-variable)

else()

    # set the project name
    project(arduino_audio_driver)

    # lots of warnings and all warnings as errors
    ## add_compile_options(-Wall -Wextra )
    set(CMAKE_CXX_STANDARD 17)

    file(GLOB_RECURSE SRC_LIST_C CONFIGURE_DEPENDS  "${PROJECT_SOURCE_DIR}/src/*.c" )
    file(GLOB_RECURSE SRC_LIST_CPP CONFIGURE_DEPENDS  "${PROJECT_SOURCE_DIR}/src/*.cpp" )

    # define libraries
    add_library (arduino_audio_driver STATIC ${SRC_LIST_C} ${SRC_LIST_CPP} )

    # prevent compile errors
    target_compile_options(arduino_audio_driver PRIVATE -DUSE_DEFAULT_STDLIB -Wno-unused-variable)

    # define location for header files
    target_include_directories(arduino_audio_driver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )

    # Add Zephyr support
    if (DEFINED ZEPHYR_PLATFORM)
        target_compile_definitions(arduino_audio_driver PRIVATE __zephyr__ )
        target_link_libraries(arduino_audio_driver PRIVATE zephyr_interface zephyr_generated_headers)
    ENDIF()

endif()
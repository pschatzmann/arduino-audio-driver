cmake_minimum_required(VERSION 3.16)

# set the project name
project(audio_driver)

# lots of warnings and all warnings as errors
## add_compile_options(-Wall -Wextra )
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_LIST_C CONFIGURE_DEPENDS  "${PROJECT_SOURCE_DIR}/src/*.c" )

# define libraries
add_library (audio_driver ${SRC_LIST_C})

# prevent compile errors
target_compile_options(audio_driver PRIVATE -DUSE_DEFAULT_STDLIB)

# define location for header files
target_include_directories(audio_driver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/libhelix-mp3 ${CMAKE_CURRENT_SOURCE_DIR}/src/libhelix-aac )


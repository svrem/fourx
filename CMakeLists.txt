# cmake_minimum_required(VERSION 3.28)
# project(SDL2Test LANGUAGES CXX)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# find_package(SDL2 REQUIRED)
# find_package(SDL2_image REQUIRED)
# include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# add_custom_target(cp_assets
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/bin/assets
#     COMMENT "Copying assets directory to build directory"
#     VERBATIM)

# file(GLOB src "src/*.cpp" "src/*.hpp")


# set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})

# add_executable(main ${src})
# add_dependencies(main cp_assets)
# target_link_libraries(main ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

cmake_minimum_required(VERSION 3.28)
project(fourx LANGUAGES CXX)

# Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# FetchContent_MakeAvailable(SDL2_ttf)
list(APPEND CMAKE_PREFIX_PATH ${SDL2_ttf_PATH})

# Specify source files explicitly for better maintenance
# Add the executable
file(GLOB SOURCES "src/*.cpp" "src/*.hpp")


add_executable(fourx ${SOURCES})

# Add conditional linking for MinGW (Windows)
if(MINGW)
    target_link_libraries(fourx PRIVATE ws2_32)
endif()


# Find SDL2 and SDL2_image packages
find_package(SDL2 REQUIRED CONFIG)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})

find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIR})

# Include SDL2 directories and link libraries

# Add the executable

# Include SDL2 directories and link libraries
# target_link_libraries(fourx PRIVATE ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
target_link_libraries(fourx PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Enable C++17 (or a version you prefer)
target_compile_features(fourx PRIVATE cxx_std_17)

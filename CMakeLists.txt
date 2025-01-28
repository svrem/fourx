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

# Enable FetchContent for external dependencies
include(FetchContent)

# Add spdlog via FetchContent
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.0 # Specify a stable version tag
)
FetchContent_MakeAvailable(spdlog)

# Fetch SDL2
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.30.11 # Use a stable release tag
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(SDL2)

# Fetch SDL2_image
FetchContent_Declare(
    SDL2_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-2.8.4 # Use a stable release tag
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

set(SDL2IMAGE_INSTALL OFF)
set(BUILD_SHARED_LIBS FALSE)

FetchContent_MakeAvailable(SDL2_image)

FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-2.24.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

set(SDL2TTF_INSTALL OFF)
set(BUILD_SHARED_LIBS FALSE)

FetchContent_MakeAvailable(SDL2_ttf)
list(APPEND CMAKE_PREFIX_PATH ${SDL2_ttf_PATH})

# Specify source files explicitly for better maintenance
# Add the executable
add_executable(fourx src/utils.cpp src/entityManager.cpp src/station.cpp src/productionStation.cpp src/ship.cpp src/main.cpp)

# Link spdlog
target_link_libraries(fourx PRIVATE spdlog::spdlog)

# Add conditional linking for MinGW (Windows)
if(MINGW)
    target_link_libraries(fourx PRIVATE ws2_32)
endif()


# Find SDL2 and SDL2_image packages
find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Include SDL2 directories and link libraries
# target_link_libraries(fourx PRIVATE ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
target_link_libraries(fourx PRIVATE SDL2::SDL2main SDL2::SDL2-static SDL2_image::SDL2_image-static SDL2_ttf::SDL2_ttf-static)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Enable C++17 (or a version you prefer)
target_compile_features(fourx PRIVATE cxx_std_17)

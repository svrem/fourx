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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB src "src/*.cpp" "src/*.hpp")

add_executable(fourx ${src})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

target_link_libraries(fourx ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
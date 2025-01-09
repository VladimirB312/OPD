cmake_minimum_required(VERSION 3.29)
project(game)

set(CMAKE_CXX_STANDARD 20)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/SFML/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS system window graphics audio network REQUIRED)

file(GLOB SOURCES "src/*.cpp" "src/*.h")

add_executable(game ${SOURCES})

target_link_libraries(game sfml-system sfml-window sfml-graphics sfml-audio)

add_custom_command(
        TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:game>/resources
)
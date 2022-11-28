cmake_minimum_required(VERSION 3.10)
project(ELECTRO_BRAIN VERSION 2.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(ELECTRO_BRAIN.h.in ELECTRO_BRAIN.h)

add_subdirectory(headers)
add_executable(ELECTRO_BRAIN ELECTRO_BRAIN.cpp)

target_link_libraries(ELECTRO_BRAIN PUBLIC TSAMPLE TNET TCONF)

target_include_directories(ELECTRO_BRAIN PUBLIC
                        "${PROJECT_BINARY_DIR}"
                        "${PROJECT_SOURCE_DIR}/headers"
                        )
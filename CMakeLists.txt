cmake_minimum_required(VERSION 3.15)
project(ELECTRO_BRAIN VERSION 2.1)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_17)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)
configure_file(ELECTRO_BRAIN.h.in ELECTRO_BRAIN.h)

add_subdirectory(headers)
add_executable(ELECTRO_BRAIN ELECTRO_BRAIN.cpp)

target_link_libraries(ELECTRO_BRAIN PUBLIC TSAMPLE TNET TCONF compiler_flags)

target_include_directories(ELECTRO_BRAIN PUBLIC
                        "${PROJECT_BINARY_DIR}"
                        "${PROJECT_SOURCE_DIR}/headers"
                        )
install(TARGETS ELECTRO_BRAIN DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/ELECTRO_BRAIN.h"
    DESTINATION include
    )
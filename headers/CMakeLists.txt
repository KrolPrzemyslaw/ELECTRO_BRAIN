add_library(TCONF SOURCE.cpp)
add_library(TSAMPLE TSAMPLE.cpp)
add_library(TNET TNET.cpp)

target_link_libraries(TCONF compiler_flags)
target_link_libraries(TSAMPLE compiler_flags)
target_link_libraries(TNET compiler_flags)

set(installable_libs TCONF TSAMPLE TNET compiler_flags)
install(TARGETS ${installable_libs} DESTINATION lib)
install(FILES TCONF.h TSAMPLE.h TFILTER.h TNET.h DESTINATION include)
FILE(GLOB Utils_srcs ./*.h ./*.cpp)
if(TARGET Utils)
    message("Utils already exists")
else(TARGET Utils)
    ADD_LIBRARY (Utils STATIC ${Utils_srcs})
    target_include_directories(Utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:include>)
endif(TARGET Utils)
# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE on)

set(LIB_DIR ./libs)

#include_directories(${LIB_DIR}/include/)

add_library( lib_ocl SHARED IMPORTED )
set_target_properties(lib_ocl PROPERTIES IMPORTED_LOCATION /system/vendor/lib64/libPVROCL.so)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
add_library( cl_test SHARED cl_test.cpp )

find_library( log-lib log )

target_include_directories(cl_test PRIVATE ${LIB_DIR}/include/)

target_link_libraries( cl_test lib_ocl ${log-lib}  )

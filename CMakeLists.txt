cmake_minimum_required(VERSION 2.8)
project(buffer CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

# only for the unittests, buffer.cc/h don't depend on it
find_package(Boost 1.53
  COMPONENTS
    system
    filesystem
    unit_test_framework
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/libixxx)

#SET_SOURCE_FILES_PROPERTIES(buffer.cc PROPERTIES COMPILE_FLAGS -D_XOPEN_SOURCE=600)

# guard from super-projects, i.e. when it is added as subdirectory
if(${CMAKE_PROJECT_NAME} STREQUAL "buffer")

add_subdirectory(libixxx)

add_executable(ut
  buffer/buffer.cc
  buffer/file.cc
  unittest/main.cc
  unittest/buffer.cc
  )
target_link_libraries(ut ${Boost_LIBRARIES} ixxx_static)
endif()

set(LIB_SRC buffer/buffer.cc buffer/file.cc)
add_library(buffer SHARED
  ${LIB_SRC}
)
target_link_libraries(buffer ixxx)
add_library(buffer_static STATIC
  ${LIB_SRC}
)
add_library(bufferlite  STATIC
  buffer/buffer.cc
)

# under windows shared/static libraries have the same extension ...
if(${UNIX})
  set_target_properties(buffer_static PROPERTIES OUTPUT_NAME buffer)
endif()


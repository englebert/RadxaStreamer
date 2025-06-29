cmake_minimum_required(VERSION 3.5)
project(ruby_fifocam_streamer)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED
  gstreamer-1.0
  gstreamer-video-1.0
)

add_executable(radxa_streamer main.cpp)
target_include_directories(radxa_streamer PRIVATE ${GST_INCLUDE_DIRS})
target_link_libraries(radxa_streamer PRIVATE ${GST_LIBRARIES})
target_compile_options(radxa_streamer PRIVATE ${GST_CFLAGS_OTHER})
set_target_properties(radxa_streamer PROPERTIES
  CXX_STANDARD     11
  CXX_STANDARD_REQUIRED ON
)

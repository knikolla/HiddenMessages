cmake_minimum_required(VERSION 2.8)
project( HiddenMessages )
find_package( OpenCV REQUIRED )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable( insert Insert.cpp )
target_link_libraries( insert ${OpenCV_LIBS} )

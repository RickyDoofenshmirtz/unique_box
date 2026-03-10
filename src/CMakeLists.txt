file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*)
add_library(lib_unique_handle ${HEADER_FILES})
target_include_directories(lib_unique_handle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(unique_box ${SRC_FILES})

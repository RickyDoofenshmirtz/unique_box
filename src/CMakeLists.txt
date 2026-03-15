add_subdirectory(handles)
add_subdirectory(containers)

target_include_directories(lib_handles INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(lib_containers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

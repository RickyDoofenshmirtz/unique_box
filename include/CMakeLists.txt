add_subdirectory(memory)
target_include_directories(lib_memory INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(containers)
target_include_directories(lib_containers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

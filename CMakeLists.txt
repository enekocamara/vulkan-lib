cmake_minimum_required(VERSION 3.28)

project(vulkan-lib CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

# Set the output directory structure
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/output/lib/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/output/lib/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/output/bin/$<CONFIG>")

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)
file(GLOB_RECURSE CXX_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.ixx
)

# Automatically organize files in Visual Studio
foreach(file ${SOURCES})
    # Get relative path and convert to Windows style
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" relative_path ${file})
    string(REPLACE "/" "\\" relative_path ${relative_path})
    get_filename_component(directory ${relative_path} DIRECTORY)
    
    # Create a source group for each directory
    source_group("${directory}" FILES ${file})
endforeach()
foreach(file ${CXX_MODULES})
    # Get relative path and convert to Windows style
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" relative_path ${file})
    string(REPLACE "/" "\\" relative_path ${relative_path})
    get_filename_component(directory ${relative_path} DIRECTORY)
    
    # Create a source group for each directory
    source_group("${directory}" FILES ${file})
endforeach()




# Check if Doxygen is available
find_package(Doxygen REQUIRED)
find_package(Vulkan REQUIRED)

#enable_testing()

#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#set(CTEST_BINARY_DIRECTORY "${CMAKE_SOURCE_DIR}/Testing")

add_library(vulkan-lib STATIC)
target_sources(vulkan-lib
    PRIVATE ${SOURCES}
    PUBLIC
        FILE_SET CXX_MODULES
        FILES ${CXX_MODULES}
)
target_compile_definitions(vulkan-lib PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions(vulkan-lib PRIVATE $<$<CONFIG:Release>:RELEASE>)

target_include_directories(vulkan-lib PUBLIC
	${CMAKE_SOURCE_DIR}/modules/glm_1.0.2
	${CMAKE_SOURCE_DIR}/modules/glfw_3.4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(vulkan-lib PUBLIC Vulkan::Vulkan debug-lib)

target_compile_definitions(vulkan-lib PUBLIC)

add_compile_definitions(PROJECT_ROOT="${CMAKE_SOURCE_DIR}")


# Enable multi-processor compilation (if supported by the compiler)
if(MSVC)
    add_compile_options(/MP)
else()
    add_compile_options(-j$(nproc))
endif()

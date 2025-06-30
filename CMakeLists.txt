cmake_minimum_required(VERSION 3.5.0...3.25)
project(cstl VERSION 0.1.0 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif ()

if (USE_ASAN)
    link_libraries(asan)
    add_compile_options(-fsanitize=address)
endif ()

if (USE_UBSAN)
    link_libraries(ubsan)
    add_compile_options(-fsanitize=undefined)
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions("DEBUG=1")
else ()
    add_compile_definitions("DEBUG=0")
endif ()

include_directories("include")


file(GLOB_RECURSE LIB_SOURCES src/lib/**.c)
file(GLOB_RECURSE EXEC_SOURCES src/exec/**.c)
add_library(cstl_shared SHARED ${LIB_SOURCES})
add_library(cstl_static STATIC ${LIB_SOURCES})

add_executable(cstl_exec ${EXEC_SOURCES})
target_link_libraries(cstl_exec PRIVATE cstl_static m)

set_target_properties(cstl_shared PROPERTIES OUTPUT_NAME "cstl")
set_target_properties(cstl_static PROPERTIES OUTPUT_NAME "cstl")
set_target_properties(cstl_exec PROPERTIES OUTPUT_NAME "cstl")

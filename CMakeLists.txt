cmake_minimum_required(VERSION 3.14)
project("C++ Project Template" LANGUAGES CXX)

set(CPACK_PACKAGE_NAME "cpp-project")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

# Use C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Colored warnings
option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)
if(${FORCE_COLORED_OUTPUT})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options (-fdiagnostics-color=always)
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options (-fcolor-diagnostics)
    endif()
endif()

# Enable ccache if it exists
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message(STATUS "CCACHE was found")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

# Use the mold linker if possible
find_program(MOLD_FOUND mold)
if(MOLD_FOUND)
    message(STATUS "MOLD linker was found")
    add_compile_options(-fuse-ld=mold)
    set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Optimization flags
include(CheckCXXCompilerFlag)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
    if(COMPILER_SUPPORTS_MARCH_NATIVE)
        add_compile_options(-march=native)
    endif()
endif()

# Use TBB for execution policies in GCC
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    find_package(TBB QUIET)
    if (TBB_FOUND)
        message(STATUS "TBB was found, enabling execution policies")
        add_compile_definitions(USE_TBB)
    else()
        message(WARNING "TBB was not found, disabling execution policies")
    endif()
endif()

# Dependency management
include(FetchContent)
# Dependency management
include(FetchContent)
# set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(FETCHCONTENT_QUIET FALSE)
macro(fetch_content name tag repository)
    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${repository}
        GIT_TAG        ${tag}
        GIT_PROGRESS   TRUE
        GIT_SHALLOW    TRUE
        SOURCE_DIR     ${PROJECT_SOURCE_DIR}/external/${name}
    )
    message(STATUS "Fetching ${name} ${tag}")
    FetchContent_MakeAvailable(${name})
endmacro()

# set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# fetch_content(pybind11 v2.10.0 https://github.com/pybind/pybind11.git)
# fetch_content(csv 2.1.3 https://github.com/vincentlaucsb/csv-parser.git)
# fetch_content(json v3.10.5 https://github.com/nlohmann/json.git)

# some other libraries
# find_package(OpenMP)
# link with #{OpenMP::OpenMP_CXX}

# find_package(OpenCV REQUIRED)
# include ${OpenCV_INCLUDE_DIRS}
# link with ${OpenCV_LIBS}

fetch_content(dlib v19.24 https://github.com/davisking/dlib.git)
fetch_content(fmt 9.0.0 https://github.com/fmtlib/fmt.git)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE dlib::dlib fmt::fmt)
target_include_directories(main PRIVATE external/fmt/include)
target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
if (TBB_FOUND)
    target_link_libraries(main PRIVATE TBB::tbb)
endif()
install(TARGETS main DESTINATION bin)

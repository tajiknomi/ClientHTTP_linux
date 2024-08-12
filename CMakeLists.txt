cmake_minimum_required(VERSION 3.16)

project(clienthttp)

# Set default architecture if not specified
if(NOT DEFINED TARGET_ARCH)
    set(TARGET_ARCH "x64")
endif()

# Set architecture-specific flags
if(${TARGET_ARCH} STREQUAL "x86")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
elseif(${TARGET_ARCH} STREQUAL "x64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m64")
else()
    message(FATAL_ERROR "Unsupported architecture: ${TARGET_ARCH}")
endif()

# Enable new DTAGs to use RUNPATH instead of RPATH
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags")

# Set the build RUNPATH (used during runtime time)
set(CMAKE_BUILD_RPATH "$ORIGIN;$ORIGIN/lib")

# Set the build type to Release
set(CMAKE_BUILD_TYPE "Release")

# Set source and header file directories
set(SOURCE_DIR src)
set(HEADER_DIR include)

# Add your source files
set(SOURCES 
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/base64.cpp
    ${SOURCE_DIR}/http.cpp
    ${SOURCE_DIR}/json.cpp
    ${SOURCE_DIR}/utilities.cpp
    ${SOURCE_DIR}/operations.cpp
)

# Optionally, add headers for IDEs or reference
set(HEADERS
    ${HEADER_DIR}/base64.h
    ${HEADER_DIR}/http.h
    ${HEADER_DIR}/json.h
    ${HEADER_DIR}/utilities.h
    ${HEADER_DIR}/operations.h
)

# Create the executable (using only source files)
add_executable(clienthttp ${SOURCES})

# Set the C++17 standard
target_compile_features(clienthttp PRIVATE cxx_std_17)

# Include the header directory
target_include_directories(clienthttp PRIVATE ${HEADER_DIR})

# Link the curl library
find_package(CURL REQUIRED)
target_link_libraries(clienthttp PRIVATE CURL::libcurl)

# Link the pthread library
find_package(Threads REQUIRED)
target_link_libraries(clienthttp PRIVATE Threads::Threads)

# Link against the stdc++fs library
target_link_libraries(clienthttp PRIVATE stdc++fs)

# Set linker flags to disable symbol generation
set_target_properties(clienthttp PROPERTIES LINK_FLAGS_RELEASE "-s")

# Set compiler flags for size optimization
target_compile_options(clienthttp PRIVATE -Os)

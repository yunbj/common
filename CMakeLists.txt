cmake_minimum_required(VERSION 3.10)

project(gridcommon)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (APPLE)
    set(CMAKE_CXX_FLAGS "-g")
    set(PLATFORM_HEADER
            ${PROJECT_SOURCE_DIR}/include/base/FileSystemMacImpl.hpp)
    set(PLATFORM_SRC
            ${PROJECT_SOURCE_DIR}/src/base/StringConvertor.cpp
            ${PROJECT_SOURCE_DIR}/src/base/FileSystem.cpp)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
    find_library(INTEL_TBB
            NAME tbb
            PATHS ${PROJECT_SOURCE_DIR}/lib/tbb/mac64
            NO_DEFAULT_PATH
            )
elseif (MSVC)
    set(PLATFORM_HEADER
            ${PROJECT_SOURCE_DIR}/include/base/FileSystemWinImpl.hpp)
else()
    set(CMAKE_CXX_FLAGS "-g")
    if(Coverage)
        message("${PROJECT_NAME}: Coverage configuration")
        set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")
    endif()
    set(PLATFORM_HEADER
            ${PROJECT_SOURCE_DIR}/include/base/FileSystemLinuxImpl.hpp)
    set(PLATFORM_SRC
            ${PROJECT_SOURCE_DIR}/src/base/StringConvertorLinux.cpp
            ${PROJECT_SOURCE_DIR}/src/base/FileSystemLinux.cpp)
    find_library(INTEL_TBB
            NAME tbb
            PATHS ${PROJECT_SOURCE_DIR}/lib/tbb/x64
            NO_DEFAULT_PATH
            )
endif ()

set(BASE_SRC
        ${PROJECT_SOURCE_DIR}/src/base/Base.cpp
        ${PROJECT_SOURCE_DIR}/src/base/Buffer.cpp
        ${PROJECT_SOURCE_DIR}/src/base/ConcurrentFixedSizeMemoryPool.cpp
        ${PROJECT_SOURCE_DIR}/src/base/Config.cpp
        ${PROJECT_SOURCE_DIR}/src/base/Gcd.cpp
        ${PROJECT_SOURCE_DIR}/src/base/LogMgr.cpp
        ${PROJECT_SOURCE_DIR}/src/base/LogThread.cpp
        ${PROJECT_SOURCE_DIR}/src/base/MemoryPool.cpp
        ${PROJECT_SOURCE_DIR}/src/base/StringHelper.cpp
        ${PROJECT_SOURCE_DIR}/src/base/PidFile.cpp
        ${PROJECT_SOURCE_DIR}/src/base/RunLoopForSignal.cpp
        ${PROJECT_SOURCE_DIR}/src/base/Thread.cpp
        ${PROJECT_SOURCE_DIR}/src/base/ThreadPool.cpp
        ${PROJECT_SOURCE_DIR}/src/base/ThreadPoolImpl.cpp
        ${PLATFORM_SRC}
    )

set(BASE_HEADER
        ${PROJECT_SOURCE_DIR}/include/base/Base.h
        ${PROJECT_SOURCE_DIR}/include/base/Buffer.h
        ${PROJECT_SOURCE_DIR}/include/base/ConcurrentFixedSizeMemoryPool.h
        ${PROJECT_SOURCE_DIR}/include/base/Config.h
        ${PROJECT_SOURCE_DIR}/include/base/FileSystem.h
        ${PROJECT_SOURCE_DIR}/include/base/Gcd.h
        ${PROJECT_SOURCE_DIR}/include/base/LogMgr.h
        ${PROJECT_SOURCE_DIR}/include/base/LogThread.h
        ${PROJECT_SOURCE_DIR}/include/base/MemoryPool.h
        ${PROJECT_SOURCE_DIR}/include/base/StringHelper.h
        ${PROJECT_SOURCE_DIR}/include/base/PidFile.h
        ${PROJECT_SOURCE_DIR}/include/base/RunLoopForSignal.h
        ${PROJECT_SOURCE_DIR}/include/base/Thread.h
        ${PROJECT_SOURCE_DIR}/include/base/ThreadPool.h
        ${PROJECT_SOURCE_DIR}/include/base/ThreadPoolImpl.h
        ${PLATFORM_HEADER}
    )

add_library(${PROJECT_NAME} ${BASE_SRC} ${BASE_HEADER})
target_include_directories(${PROJECT_NAME}
        INTERFACE PUBLIC include
)
target_link_libraries(${PROJECT_NAME} ${INTEL_TBB} pthread)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

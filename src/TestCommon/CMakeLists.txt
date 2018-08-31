# 요구 CMake 최소 버전
cmake_minimum_required(VERSION 3.0)


set(CMAKE_VERBOSE_MAKEFILE true)


# 프로젝트 이름 및 버전
project("common")
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)


# 빌드 형상(Configuration)
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

 
# subdirectories


# 공통 헤더 파일 Include 디렉토리 (-I)
include_directories(../../include)


# 빌드 대상 바이너리 파일명 및 소스 파일 목록
set(COMMON "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
set(UNITTEST_COMMON ${COMMON}_unittest)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(UNITTEST_COMMON ${UNITTEST_COMMON}d)
endif()


set(COMMON_HEADER_FILES
	../../include/base/Base.h
	../../include/base/Buffer.h
	../../include/base/Config.h
	../../include/base/FileSystem.h
	../../include/base/Gcd.h
	../../include/base/MemoryPool.h
	../../include/base/ConcurrentFixedSizeMemoryPool.h
	../../include/base/ILogWriter.h
	../../include/base/LogMgr.h
	../../include/base/Pipe.h
	../../include/base/RunLoopForSignal.h
	../../include/base/StringConvertor.h
	../../include/base/StringHelper.h
	../../include/base/json.hpp
	../../include/base/tinydir.h
	../../include/base/IThreadPoolImpl.h
	../../include/base/Thread.h
	../../include/base/ThreadPool.h
	../../include/base/ThreadPoolImpl.h
	)


set(COMMON_SRC_FILES
	${COMMON_HEADER_FILES}
	../base/Base.cpp
	../base/Buffer.cpp
	../base/Config.cpp
	../base/FileSystem.cpp
	../base/MemoryPool.cpp
	../base/ConcurrentFixedSizeMemoryPool.cpp
	../base/Gcd.cpp
	../base/LogMgr.cpp
	../base/Pipe.cpp
	../base/RunLoopForSignal.cpp
	../base/StringConvertor.cpp
	../base/StringHelper.cpp
	../base/Thread.cpp
	../base/ThreadPool.cpp
	../base/ThreadPoolImpl.cpp
	)
	

set(UNITTEST_COMMON_HEADER_FILES
	TestBuffer.h
	TestFileSystem.h
	TestGcd.h
	TestStringConvertor.h
	TestStringHelper.h
	TestThreadPool.h
	)

set(UNITTEST_COMMON_SRC_FILES
	${COMMON_SRC_FILES}
	${UNITTEST_COMMON_HEADER_FILES}
	TestBuffer.cpp
	TestCFSMemoryPool.cpp
	TestFileSystem.cpp
	TestGcd.cpp
	TestMemoryPool.cpp
	TestStringConvertor.cpp
	TestStringHelper.cpp
	TestThreadPool.cpp
	main.cpp
	)


# 공통 컴파일러
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_COMPILER "gcc")

if (APPLE)
#set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_AR_COMPILER "ar")
#set(CMAKE_LD_COMPILER "g++")
	set(TBB_LINK_DIR ../../lib/tbb/mac64)
else ()
	set(TBB_LINK_DIR ../../lib/tbb/x64)
endif()


# 공통 컴파일 옵션, 링크 옵션
if(UNIX)
  add_compile_options(-Wall)
elseif(WIN32)
  add_compile_options(/W3)
endif()

add_definitions(-DTHREADED)


if(WIN32)
	add_compile_options(/MT$<$<CONFIG:Debug>:d>)
endif()


# 공통 링크 라이브러리 디렉토리 (-L)
link_directories(${TBB_LINK_DIR})


# 출력 디렉토리

 
# 빌드 대상 바이너리 추가
add_executable(${UNITTEST_COMMON} ${UNITTEST_COMMON_SRC_FILES})

# 공통 링크 라이브러리 (-l)
if (APPLE)
	target_link_libraries(${UNITTEST_COMMON} pthread tbb)
elseif (UNIX)
	target_link_libraries(${UNITTEST_COMMON} pthread tbb stdc++fs)
endif()

install(TARGETS ${UNITTEST_COMMON} DESTINATION bin)
install(DIRECTORY ${TBB_LINK_DIR}/ DESTINATION bin FILES_MATCHING PATTERN "*")


include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckLibraryExists)

cmake_minimum_required(VERSION 2.6)

project(nezvm)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")

if(MSVC)
	SET(CMAKE_C_FLAGS_RELEASE      "/D NDEBUG /W3 /Wall /MT /Ox /Ob2 /GL")
	SET(CMAKE_C_FLAGS_DEBUG        "/D_DEBUG /W3 /Wall /MTd /Zi /Ob0 /Od /RTC1")
	SET(CMAKE_C_FLAGS_COMPRESSED   "/D NDEBUG /W3 /Wall /MT /Ox /GL")
	SET(CMAKE_CXX_FLAGS_RELEASE      "/D NDEBUG /W3 /Wall /MT /Ox /Ob2 /GL")
	SET(CMAKE_CXX_FLAGS_DEBUG        "/D_DEBUG /W3 /Wall /MTd /Zi /Ob0 /Od /RTC1")
	SET(CMAKE_CXX_FLAGS_COMPRESSED   "/D NDEBUG /W3 /Wall /MT /Ox /GL")
else(MSVC)
    SET(CFLAGS "-Wall -Wsign-compare")
	SET(CFLAGS "${CFLAGS} -Wall -Wcomment -Wunused-macros -std=c99")
	SET(CMAKE_C_FLAGS "${CFLAGS}")
	SET(CMAKE_CXX_FLAGS "${CFLAGS}")
	SET(CMAKE_C_FLAGS_RELEASE      "${CMAKE_C_FLAGS} -O2 -Wl,-s -g -DNDEBUG")
	SET(CMAKE_C_FLAGS_DEBUG        "${CMAKE_C_FLAGS} -O0 -g3")
	SET(CMAKE_C_FLAGS_COMPRESSED   "${CMAKE_C_FLAGS} -Os -g0")
	SET(CMAKE_CXX_FLAGS_RELEASE    "${CMAKE_CXX_FLAGS} -O3 -g")
	SET(CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS} -O0 -g3")
	SET(CMAKE_CXX_FLAGS_COMPRESSED "${CMAKE_CXX_FLAGS} -Os -g0")
endif(MSVC)

## Source Code settings
set(NEZVM_SOURCE 
			nezvm.c
			main.c
			loader.c
)

set(PACKAGE_NAME    ${PROJECT_NAME})
set(PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(PACKAGE_STRING  ${CMAKE_PROJECT_NAME}-${PACKAGE_VERSION})
set(SO_VERSION      ${VERSION_MAJOR}.${VERSION_MINOR})
set(PACKAGE_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

string(TOUPPER ${CMAKE_BUILD_TYPE} uppercase_CMAKE_BUILD_TYPE)

check_type_size("void *" SIZEOF_VOIDP)
check_type_size(long     SIZEOF_LONG)
check_type_size(int      SIZEOF_INT)
check_type_size(float    SIZEOF_FLOAT)
check_type_size(double   SIZEOF_DOUBLE)

set(_HEADERS stdio.h stdlib.h string.h ctype.h)
if(UNIX)
	set(_HEADERS ${_HEADERS} sys/mman.h unistd.h)
endif(UNIX)

if(APPLE)
	set(_HEADERS ${_HEADERS} mach-o/dyld.h)
endif(APPLE)

check_symbol_exists(posix_memalign "${_HEADERS}" HAVE_POSIX_MEMALIGN)
check_symbol_exists(memalign       "${_HEADERS}" HAVE_MEMALIGN)
check_symbol_exists(__builtin_ctzl "${_HEADERS}" HAVE_BUILTIN_CTZL)
check_symbol_exists(bzero "${_HEADERS}" HAVE_BZERO)

FIND_FILE(HAS_DOTGIT .git PATHS "${CMAKE_CURRENT_SOURCE_DIR}")
if("${HAS_DOTGIT}" MATCHES ".git")
	find_package(Git)
	if(GIT_EXECUTABLE)
		execute_process(
			COMMAND ${GIT_EXECUTABLE} rev-parse -q --short=8 HEAD
			OUTPUT_VARIABLE GIT_SHA1
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	set(NEZVM_REVISION "${GIT_SHA1}")
	set(NEZVM_REVISION_PREFIX "git:")
	endif(GIT_EXECUTABLE)
else()
	set(NEZVM_REVISION "${PACKAGE_VERSION}")
	set(NEZVM_REVISION_PREFIX "release:")
endif()

add_definitions(-DHAVE_CONFIG_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
		${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../libnez/ ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${INCLUDE_DIRS})

add_library(nez ${NEZVM_SOURCE})
add_executable(nezvm ${NEZVM_SOURCE})
target_link_libraries(nezvm)

install(TARGETS nezvm nezvm
		RUNTIME DESTINATION bin
		)

MESSAGE(STATUS)
MESSAGE(STATUS "-----------------------------------------------------------------" )
MESSAGE(STATUS "CMAKE_BUILD_TYPE = ${uppercase_CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
MESSAGE(STATUS "CMAKE_C_FLAGS   = ${CMAKE_C_FLAGS_${uppercase_CMAKE_BUILD_TYPE}}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS_${uppercase_CMAKE_BUILD_TYPE}}")
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "Change a value with: cmake -D<Variable>=<Value>" )
MESSAGE(STATUS "-----------------------------------------------------------------" )
MESSAGE(STATUS)

project(TUMORS) 

cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0005 OLD)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Set RPATH-handing (CMake parameters)
set(CMAKE_SKIP_BUILD_RPATH 0)
set(CMAKE_BUILD_WITH_INSTALL_RPATH 0)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")

if( NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "Release")
endif()

include(CheckCXXSourceRuns)
include(CheckIncludeFiles)

include(cmake/options.cmake)

if( NOPCH )
	set(USE_PCH 0)
endif()

include(CheckPlatform)

set(OPENSSL_EXPECTED_VERSION 1.0.1)

find_package(PCHSupport)
find_package(OpenSSL ${OPENSSL_EXPECTED_VERSION} REQUIRED)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.55.0 COMPONENTS filesystem system thread) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(progname ${Boost_LIBRARIES})                                                                                                                                                                                                                            
endif()

if( UNIX )
	find_package(Readline)
endif()

if(NOT WITHOUT_GIT)
  find_package(Git)
endif()

# Find revision ID and hash of the sourcetree
include(cmake/genrev.cmake)

include(cmake/showoptions.cmake)

# add dependencies
add_subdirectory(dep)

# add core sources
add_subdirectory(src)

# Platform-specfic options

#option(USE_MYSQL_SOURCES "Use included MySQL-sources to build libraries" 1)

# Package overloads
#set(ACE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/acelite)
#set(ACE_LIBRARY "ace")
#set(BZIP2_LIBRARIES "bzip2")
#set(ZLIB_LIBRARIES "zlib")

if( USE_MYSQL_SOURCES )
  set(MYSQL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/mysqllite/include)
  set(MYSQL_LIBRARY "libmysql")
  set( MYSQL_FOUND 1 )
  message(STATUS "Using supplied MySQL sources")
endif()

if ( MSVC )
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/msvc/settings.cmake)
elseif ( MINGW )
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/mingw/settings.cmake)
endif()
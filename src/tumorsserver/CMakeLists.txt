if( USE_PCH )
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif()

file(GLOB_RECURSE sources_CommandLine CommandLine/*.cpp CommandLine/*.h)
file(GLOB sources_localdir *.cpp *.h)

if (USE_PCH)
  set(tumorsserver_PCH_HDR PrecompiledHeaders/serverPCH.h)
  set(tumorsserver_PCH_SRC PrecompiledHeaders/serverPCH.cpp)
endif()

set(tumorsserver_SRCS
  ${tumorsserver_SRCS}
  ${sources_CommandLine}
  ${sources_localdir}
)

if( WIN32 )
  set(tumorsserver_SRCS
    ${tumorsserver_SRCS}
    #${sources_windows_Debugging}
  )
  if ( MSVC )
    set(tumorsserver_SRCS
      ${tumorsserver_SRCS}
      tumorsserver.rc
    )
  endif()
endif()

include_directories(
  	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}/src/shared
	${CMAKE_SOURCE_DIR}/src/shared/Configuration
	${CMAKE_SOURCE_DIR}/src/shared/Cryptography
	${CMAKE_SOURCE_DIR}/src/shared/Logging
	${CMAKE_SOURCE_DIR}/src/shared/Networking
	${CMAKE_SOURCE_DIR}/src/shared/Threading
	${CMAKE_SOURCE_DIR}/src/shared/Utilities
	${CMAKE_SOURCE_DIR}/src/gameworld
	${CMAKE_SOURCE_DIR}/src/gameworld/World
	${CMAKE_SOURCE_DIR}/src/gameworld/Server
	${CMAKE_CURRENT_SOURCE_DIR}
	${OPENSSL_INCLUDE_DIR}
)

add_executable(tumorsserver
	${tumorsserver_SRCS}
  	${tumorsserver_PCH_SRC}
)

if( NOT WIN32 )
 	set_target_properties(tumorsserver PROPERTIES
    	COMPILE_DEFINITIONS _TUMORS_CORE_CONFIG="${CONF_DIR}/tumors.cfg"
)
endif()

add_dependencies(tumorsserver revision.h)

if( UNIX AND NOT NOJEM AND NOT APPLE )
  set(tumorsserver_LINK_FLAGS "-pthread -lncurses ${tumorsserver_LINK_FLAGS}")
endif()

set_target_properties(tumorsserver PROPERTIES LINK_FLAGS "${tumorsserver_LINK_FLAGS}")

target_link_libraries(tumorsserver
  shared
  gameworld
  ${JEMALLOC_LIBRARY}
  ${READLINE_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
)


if( WIN32 )
  if ( MSVC )
	add_custom_command(TARGET tumorsserver
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tumors.cfg ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
    )
  elseif ( MINGW )
    add_custom_command(TARGET tumorsserver
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/tumors.cfg ${CMAKE_BINARY_DIR}/bin/
    )
  endif()
endif()

if( UNIX )
	install(TARGETS tumorsserver DESTINATION bin)
	install(FILES tumors.cfg DESTINATION ${CONF_DIR})
elseif( WIN32 )
	install(TARGETS tumorsserver DESTINATION "${CMAKE_INSTALL_PREFIX}")
	install(FILES tumors.cfg DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()


if( USE_PCH )
	add_cxx_pch(tumorsserver ${tumorsserver_PCH_HDR} ${tumorsserver_PCH_SRC})
endif()
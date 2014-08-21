if( USE_PCH )
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif()

file(GLOB_RECURSE sources_Server Server/*.cpp Server/*.h)
file(GLOB_RECURSE sources_World World/*.cpp World/*.h)

file(GLOB sources_localdir *.cpp *.h)


if (USE_PCH)
  set(gameworld_PCH_HDR PrecompiledHeaders/gameworldPCH.h)
  set(gameworld_PCH_SRC PrecompiledHeaders/gameworldPCH.cpp)
endif()

set(gameworld_SRCS
  ${gameworld_SRCS}
  ${sources_Server}
  ${sources_World}
  ${sources_localdir}
)


include_directories(
  	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/shared
	${CMAKE_SOURCE_DIR}/src/shared/Configuration
	${CMAKE_SOURCE_DIR}/src/shared/Cryptography
	${CMAKE_SOURCE_DIR}/src/shared/Logging
	${CMAKE_SOURCE_DIR}/src/shared/Networking
	${CMAKE_SOURCE_DIR}/src/shared/Utilities
	
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/World
	${OPENSSL_INCLUDE_DIR}
)

add_library(gameworld STATIC
  ${gameworld_SRCS}
  ${gameworld_PCH_SRC}
)

target_link_libraries(gameworld
	${Boost_LIBRARIES}
)

if( USE_PCH )
	add_cxx_pch(gameworld ${gameworld_PCH_HDR} ${gameworld_PCH_SRC})
endif()
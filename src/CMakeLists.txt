# Enforce compile-parameters for builds under GCC
# This to stop a few silly crashes that could happen if Brad or
# someone else decides to do some -O3 psychooptimizations etc.
if(CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
  add_definitions(-fno-delete-null-pointer-checks)
endif()

add_subdirectory(genrev)

if( SERVER )

add_subdirectory(shared)
add_subdirectory(tumorsserver)
add_subdirectory(gameworld)

endif()